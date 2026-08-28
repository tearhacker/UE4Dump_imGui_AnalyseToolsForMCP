#include "StructLayout.hpp"

#include <algorithm>
#include <limits>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace UEMemory;

namespace AutoFixStructLayout
{
namespace
{
constexpr int32_t kMaxFieldCount = 2048;
constexpr int32_t kMaxStructSize = 0x400000;
constexpr int32_t kMaxArrayDim = 0x10000;
constexpr int32_t kDefaultClassAlignment = static_cast<int32_t>(sizeof(void *));

std::unordered_map<uintptr_t, StructLayoutInfo> gStructInfos;
std::vector<UE_UStruct> gAllStructs;
uint32_t gCachedGeneration = 0;
bool gLayoutBuilt = false;
bool gLayoutBuilding = false;

void ResetCaches()
{
    gStructInfos.clear();
    gAllStructs.clear();
    gLayoutBuilt = false;
    gLayoutBuilding = false;
}

bool IsPowerOfTwo(int32_t value)
{
    return value > 0 && (value & (value - 1)) == 0;
}

int32_t AlignValue(int32_t value, int32_t alignment)
{
    if (value <= 0)
        return 0;
    if (alignment <= 1)
        return value;
    return (value + alignment - 1) & ~(alignment - 1);
}

int32_t NormalizeAlignment(int32_t value)
{
    if (!IsPowerOfTwo(value) || value > 0x100)
        return 1;
    return value;
}

int32_t NormalizeSize(int32_t value)
{
    if (value < 0 || value > kMaxStructSize)
        return 0;
    return value;
}

int32_t RawStructSize(const UE_UStruct &strct)
{
    return strct ? NormalizeSize(strct.GetSize()) : 0;
}

int32_t ReadStructMinAlignment(const UE_UStruct &strct)
{
    if (!strct)
        return 1;

    auto *offsets = UEWrappers::GetOffsets();
    if (!offsets)
        return 1;

    const uintptr_t base = reinterpret_cast<uintptr_t>(strct.GetAddress());
    const uintptr_t explicitOffset = offsets->UStruct.MinAlignment;
    const uintptr_t inferredOffset = offsets->UStruct.PropertiesSize ? offsets->UStruct.PropertiesSize + sizeof(int32_t) : 0;
    const uintptr_t candidates[] = {explicitOffset, inferredOffset};

    for (uintptr_t offset : candidates)
    {
        if (!offset)
            continue;

        const int32_t value32 = vm_rpm_ptr<int32_t>(reinterpret_cast<void *>(base + offset));
        const int16_t value16 = vm_rpm_ptr<int16_t>(reinterpret_cast<void *>(base + offset));

        const int32_t normalized32 = NormalizeAlignment(value32);
        if (normalized32 > 1 || value32 == 1)
            return normalized32;

        const int32_t normalized16 = NormalizeAlignment(value16);
        if (normalized16 > 1 || value16 == 1)
            return normalized16;
    }

    return 1;
}

std::string PropertyClassName(const UE_FProperty &field)
{
    return field ? field.GetClass().GetName() : std::string();
}

bool IsPropertyClassName(const std::string &name)
{
    return !name.empty() && name.find("Property") != std::string::npos;
}

UE_UStruct GetUnderlyingStruct(const UE_FProperty &field)
{
    if (!field || !UEWrappers::GetOffsets() || !UEWrappers::GetOffsets()->StructProperty.Struct)
        return {};
    return field.Cast<UE_FStructProperty>().GetStruct();
}

UE_FProperty GetEnumUnderlyingProperty(const UE_FProperty &field)
{
    if (!field || !UEWrappers::GetOffsets() || !UEWrappers::GetOffsets()->EnumProperty.UnderlayingProp)
        return {};
    return field.Cast<UE_FEnumProperty>().GetUnderlayingProperty();
}

bool IsBoolFieldMask(uint8_t mask)
{
    return mask == 0xFF || (mask != 0 && (mask & (mask - 1)) == 0);
}

bool IsNativeBoolProperty(const UE_FProperty &field)
{
    const uint8_t mask = field.Cast<UE_FBoolProperty>().GetFieldMask();
    return mask == 0xFF || !IsBoolFieldMask(mask);
}

uint8_t GetBoolBitIndex(const UE_FProperty &field)
{
    const auto boolProp = field.Cast<UE_FBoolProperty>();
    const uint8_t byteOffset = boolProp.GetByteOffset();
    const uint8_t fieldMask = boolProp.GetFieldMask();
    if (fieldMask == 0xFF || !IsBoolFieldMask(fieldMask))
        return 0xFF;

    for (uint8_t bit = 0; bit < 8; ++bit)
    {
        if (fieldMask == (1u << bit))
            return static_cast<uint8_t>(byteOffset * 8 + bit);
    }
    return 0xFF;
}

int32_t FallbackAlignmentFromSize(int32_t size)
{
    if (size >= 8)
        return 8;
    if (size >= 4)
        return 4;
    if (size >= 2)
        return 2;
    return 1;
}

int32_t KnownPropertyAlignment(const std::string &propertyClassName)
{
    if (propertyClassName == "BoolProperty" || propertyClassName == "ByteProperty" || propertyClassName == "Int8Property")
        return 1;
    if (propertyClassName == "Int16Property" || propertyClassName == "UInt16Property")
        return 2;
    if (propertyClassName == "IntProperty" || propertyClassName == "Int32Property" || propertyClassName == "UInt32Property" ||
        propertyClassName == "FloatProperty" || propertyClassName == "NameProperty")
        return 4;
    if (propertyClassName == "Int64Property" || propertyClassName == "UInt64Property" || propertyClassName == "DoubleProperty" ||
        propertyClassName == "ObjectProperty" || propertyClassName == "ClassProperty" || propertyClassName == "SoftObjectProperty" ||
        propertyClassName == "WeakObjectProperty" || propertyClassName == "LazyObjectProperty" || propertyClassName == "SoftClassProperty" ||
        propertyClassName == "ArrayProperty" || propertyClassName == "MapProperty" || propertyClassName == "SetProperty" ||
        propertyClassName == "StrProperty" || propertyClassName == "TextProperty" || propertyClassName == "DelegateProperty" ||
        propertyClassName == "MulticastDelegateProperty" || propertyClassName == "MulticastInlineDelegateProperty" ||
        propertyClassName == "MulticastSparseDelegateProperty" || propertyClassName == "InterfaceProperty" ||
        propertyClassName == "FieldPathProperty")
    {
        return kDefaultClassAlignment;
    }

    return 0;
}

int32_t GetPropertyAlignmentInternal(const UE_FProperty &field, bool allowStructInfo);
int32_t GetPropertyElementSizeInternal(const UE_FProperty &field, bool allowStructInfo);

int32_t GetPropertyAlignmentInternal(const UE_FProperty &field, bool allowStructInfo)
{
    const std::string propertyClassName = PropertyClassName(field);

    if (propertyClassName == "StructProperty")
    {
        const UE_UStruct underlying = GetUnderlyingStruct(field);
        if (underlying)
        {
            if (allowStructInfo && gLayoutBuilt)
                return GetStructLayoutInfo(underlying).Alignment;
            return ReadStructMinAlignment(underlying);
        }
    }

    if (propertyClassName == "EnumProperty")
    {
        const UE_FProperty underlying = GetEnumUnderlyingProperty(field);
        if (underlying)
            return GetPropertyAlignmentInternal(underlying, allowStructInfo);
    }

    const int32_t knownAlignment = KnownPropertyAlignment(propertyClassName);
    if (knownAlignment > 0)
        return knownAlignment;

    return FallbackAlignmentFromSize(GetPropertyElementSizeInternal(field, false));
}

int32_t KnownPropertyElementSize(const std::string &propertyClassName)
{
    if (propertyClassName == "BoolProperty" || propertyClassName == "ByteProperty" || propertyClassName == "Int8Property")
        return 1;
    if (propertyClassName == "Int16Property" || propertyClassName == "UInt16Property")
        return 2;
    if (propertyClassName == "IntProperty" || propertyClassName == "Int32Property" || propertyClassName == "UInt32Property" ||
        propertyClassName == "FloatProperty")
        return 4;
    if (propertyClassName == "Int64Property" || propertyClassName == "UInt64Property" || propertyClassName == "DoubleProperty" ||
        propertyClassName == "NameProperty" || propertyClassName == "ObjectProperty" || propertyClassName == "ClassProperty" ||
        propertyClassName == "InterfaceProperty")
        return 8;
    if (propertyClassName == "StrProperty" || propertyClassName == "ArrayProperty" || propertyClassName == "DelegateProperty")
        return 0x10;
    if (propertyClassName == "TextProperty")
        return 0x18;
    if (propertyClassName == "MapProperty" || propertyClassName == "SetProperty")
        return 0x50;

    return 0;
}

int32_t GetPropertyElementSizeInternal(const UE_FProperty &field, bool allowStructInfo)
{
    const std::string propertyClassName = PropertyClassName(field);
    const int32_t rawSize = field ? NormalizeSize(field.GetSize()) : 0;

    if (propertyClassName == "BoolProperty")
    {
        const uint8_t fieldSize = field.Cast<UE_FBoolProperty>().GetFieldSize();
        if (fieldSize == 1 || fieldSize == 2 || fieldSize == 4 || fieldSize == 8)
            return fieldSize;
    }

    if (propertyClassName == "StructProperty")
    {
        const UE_UStruct underlying = GetUnderlyingStruct(field);
        if (underlying && allowStructInfo && gLayoutBuilt)
        {
            const int32_t structSize = GetStructLayoutInfo(underlying).AlignedSize;
            if (structSize > 0)
                return structSize;
        }
    }

    if (propertyClassName == "EnumProperty")
    {
        const UE_FProperty underlying = GetEnumUnderlyingProperty(field);
        if (underlying)
        {
            const int32_t enumSize = GetPropertyElementSizeInternal(underlying, allowStructInfo);
            if (enumSize > 0)
                return enumSize;
        }
    }

    if (rawSize > 0)
        return rawSize;

    const int32_t knownSize = KnownPropertyElementSize(propertyClassName);
    return knownSize > 0 ? knownSize : 1;
}

bool HasMembers(const UE_UStruct &strct)
{
    return strct && strct.GetChildProperties();
}

bool IsClassStruct(const UE_UStruct &strct)
{
    return strct && strct.IsA<UE_UClass>();
}

bool IsInterfaceStruct(const UE_UStruct &strct)
{
    static UE_UClass interfaceClass;
    static uint32_t interfaceGen = 0;
    const uint32_t generation = UEWrappers::GetInitGeneration();
    if (interfaceGen != generation)
    {
        interfaceClass = UEWrappers::GetObjects()->FindObject<UE_UClass>("Class CoreUObject.Interface");
        interfaceGen = generation;
    }
    if (!interfaceClass)
        return false;

    std::unordered_set<uintptr_t> seen;
    for (UE_UStruct current = strct; current; current = current.GetSuper())
    {
        const uintptr_t currentAddr = reinterpret_cast<uintptr_t>(current.GetAddress());
        if (!seen.insert(currentAddr).second)
            break;
        if (current == interfaceClass)
            return true;
    }
    return false;
}

void CollectStructs()
{
    if (!gAllStructs.empty() || !UEWrappers::GetObjects())
        return;

    UE_UClass structClass = UE_UStruct::StaticClass();
    if (!structClass)
        return;

    gAllStructs.reserve(10000);
    UEWrappers::GetObjects()->ForEachObjectOfClass(structClass, [&](UE_UObject object)
    {
        if (object.IsA<UE_UFunction>())
            return false;
        gAllStructs.push_back(object.Cast<UE_UStruct>());
        return false;
    });
}

StructLayoutInfo BuildSingleInfo(const UE_UStruct &strct)
{
    StructLayoutInfo info;
    if (!strct)
        return info;

    if (IsInterfaceStruct(strct))
    {
        info.Alignment = 1;
        info.AlignedSize = 0;
        info.Size = 0;
        info.IsFinal = true;
        return info;
    }

    const int32_t minAlignment = ReadStructMinAlignment(strct);
    int32_t highestMemberAlignment = 1;
    int32_t lastMemberEnd = 0;

    int32_t fieldCount = 0;
    for (UE_FField fieldBase = strct.GetChildProperties(); fieldBase && fieldCount < kMaxFieldCount; fieldBase = fieldBase.GetNext(), ++fieldCount)
    {
        const UE_FProperty field = fieldBase.Cast<UE_FProperty>();
        const std::string propertyClassName = PropertyClassName(field);
        if (!IsPropertyClassName(propertyClassName))
            continue;

        highestMemberAlignment = std::max(highestMemberAlignment, GetPropertyAlignmentInternal(field, false));

        const int32_t offset = field.GetOffset();
        const int32_t elementSize = GetPropertyElementSizeInternal(field, false);
        const int32_t arrayDim = std::clamp(field.GetArrayDim(), 1, kMaxArrayDim);
        if (offset >= 0 && elementSize > 0)
            lastMemberEnd = std::max(lastMemberEnd, offset + elementSize * arrayDim);
    }

    const UE_UStruct super = strct.GetSuper();
    const bool hasSuper = static_cast<bool>(super);
    if (IsClassStruct(strct) && hasSuper && highestMemberAlignment < kDefaultClassAlignment)
    {
        info.Alignment = kDefaultClassAlignment;
    }
    else
    {
        info.UseExplicitAlignment = minAlignment > highestMemberAlignment;
        info.Alignment = std::max(minAlignment, highestMemberAlignment);
    }

    info.Alignment = NormalizeAlignment(info.Alignment);
    info.Size = RawStructSize(strct);
    if (info.Size == 0 && hasSuper)
        info.Size = RawStructSize(super);
    info.LastMemberEnd = lastMemberEnd;
    info.AlignedSize = AlignValue(info.Size, info.Alignment);
    return info;
}

void BuildBasicInfos()
{
    for (const UE_UStruct &strct : gAllStructs)
        gStructInfos[reinterpret_cast<uintptr_t>(strct.GetAddress())] = BuildSingleInfo(strct);
}

void FixAlignmentsFromSupers()
{
    for (const UE_UStruct &strct : gAllStructs)
    {
        if (IsInterfaceStruct(strct))
            continue;

        std::vector<UE_UStruct> stack;
        std::unordered_set<uintptr_t> seen;
        for (UE_UStruct current = strct; current; current = current.GetSuper())
        {
            const uintptr_t currentAddr = reinterpret_cast<uintptr_t>(current.GetAddress());
            if (!seen.insert(currentAddr).second || gStructInfos.find(currentAddr) == gStructInfos.end())
                break;
            stack.push_back(current);
            if (stack.size() >= 0x30)
                break;
        }

        int32_t highestAlignment = 0;
        for (auto it = stack.rbegin(); it != stack.rend(); ++it)
        {
            StructLayoutInfo &info = gStructInfos[reinterpret_cast<uintptr_t>(it->GetAddress())];
            if (highestAlignment < info.Alignment)
            {
                highestAlignment = info.Alignment;
            }
            else
            {
                info.UseExplicitAlignment = false;
                info.Alignment = highestAlignment > 0 ? highestAlignment : 1;
                info.AlignedSize = AlignValue(info.Size, info.Alignment);
            }
        }
    }
}

void FixSizesAndFinalFlags()
{
    for (const UE_UStruct &strct : gAllStructs)
    {
        if (IsInterfaceStruct(strct))
            continue;

        StructLayoutInfo &info = gStructInfos[reinterpret_cast<uintptr_t>(strct.GetAddress())];
        const int32_t rawStructSize = RawStructSize(strct);
        if (info.Size == 0 || (rawStructSize > 0 && info.Size > rawStructSize))
            info.Size = rawStructSize;

        int32_t lastMemberEnd = 0;
        int32_t lowestOffset = std::numeric_limits<int32_t>::max();
        int32_t fieldCount = 0;
        for (UE_FField fieldBase = strct.GetChildProperties(); fieldBase && fieldCount < kMaxFieldCount; fieldBase = fieldBase.GetNext(), ++fieldCount)
        {
            const UE_FProperty field = fieldBase.Cast<UE_FProperty>();
            const std::string propertyClassName = PropertyClassName(field);
            if (!IsPropertyClassName(propertyClassName))
                continue;

            const int32_t offset = field.GetOffset();
            const int32_t elementSize = GetPropertyElementSizeInternal(field, false);
            const int32_t arrayDim = std::clamp(field.GetArrayDim(), 1, kMaxArrayDim);
            if (offset < 0 || elementSize <= 0)
                continue;

            lowestOffset = std::min(lowestOffset, offset);
            lastMemberEnd = std::max(lastMemberEnd, offset + elementSize * arrayDim);
        }

        info.LastMemberEnd = lastMemberEnd;

        const UE_UStruct super = strct.GetSuper();
        if (!super)
            continue;

        std::unordered_set<uintptr_t> seen;
        for (UE_UStruct current = super; current; current = current.GetSuper())
        {
            const uintptr_t currentAddr = reinterpret_cast<uintptr_t>(current.GetAddress());
            if (!seen.insert(currentAddr).second)
                break;

            auto it = gStructInfos.find(currentAddr);
            if (it == gStructInfos.end())
                break;

            StructLayoutInfo &superInfo = it->second;
            superInfo.IsFinal = false;

            if (lowestOffset != std::numeric_limits<int32_t>::max())
            {
                const int32_t sizeToCheck = superInfo.Size > 0 ? superInfo.Size : RawStructSize(current);
                if (AlignValue(sizeToCheck, superInfo.Alignment) > lowestOffset)
                {
                    if (superInfo.Size == 0 || superInfo.Size > lowestOffset)
                        superInfo.Size = lowestOffset;
                    superInfo.HasReusedTrailingPadding = true;
                    superInfo.AlignedSize = AlignValue(superInfo.Size, superInfo.Alignment);
                }
            }

            if (HasMembers(current))
                break;
        }
    }

    for (auto &[_, info] : gStructInfos)
        info.AlignedSize = AlignValue(info.Size, info.Alignment);
}

void EnsureCacheFresh()
{
    const uint32_t generation = UEWrappers::GetInitGeneration();
    if (gCachedGeneration != generation)
    {
        gCachedGeneration = generation;
        ResetCaches();
    }
}

void BuildLayoutCache()
{
    EnsureCacheFresh();
    if (gLayoutBuilt || gLayoutBuilding)
        return;

    gLayoutBuilding = true;
    CollectStructs();
    BuildBasicInfos();
    FixAlignmentsFromSupers();
    FixSizesAndFinalFlags();
    gLayoutBuilding = false;
    gLayoutBuilt = true;
}
}  // namespace

const StructLayoutInfo &GetStructLayoutInfo(const UE_UStruct &strct)
{
    BuildLayoutCache();

    const uintptr_t key = reinterpret_cast<uintptr_t>(strct.GetAddress());
    auto it = gStructInfos.find(key);
    if (it != gStructInfos.end())
        return it->second;

    StructLayoutInfo info = BuildSingleInfo(strct);
    info.AlignedSize = AlignValue(info.Size, info.Alignment);
    return gStructInfos.emplace(key, info).first->second;
}

std::vector<FieldLayoutInfo> GetSortedFields(const UE_UStruct &strct)
{
    BuildLayoutCache();

    std::vector<FieldLayoutInfo> fields;
    if (!strct)
        return fields;

    int32_t fieldCount = 0;
    for (UE_FField fieldBase = strct.GetChildProperties(); fieldBase && fieldCount < kMaxFieldCount; fieldBase = fieldBase.GetNext(), ++fieldCount)
    {
        const UE_FProperty field = fieldBase.Cast<UE_FProperty>();
        const std::string propertyClassName = PropertyClassName(field);
        if (!IsPropertyClassName(propertyClassName))
            continue;

        const int32_t offset = field.GetOffset();
        if (offset < 0)
            continue;

        FieldLayoutInfo item;
        item.Field = field;
        item.Offset = offset;
        item.ElementSize = GetPropertyElementSizeInternal(field, true);
        item.ArrayDim = std::clamp(field.GetArrayDim(), 1, kMaxArrayDim);
        item.TotalSize = item.ElementSize * item.ArrayDim;
        if (propertyClassName == "BoolProperty" && !IsNativeBoolProperty(field))
        {
            item.BitIndex = GetBoolBitIndex(field);
            item.IsBitfield = item.BitIndex != 0xFF;
            item.FieldMask = field.Cast<UE_FBoolProperty>().GetFieldMask();
        }
        if (item.ElementSize <= 0 || item.TotalSize <= 0)
            continue;

        fields.push_back(item);
    }

    std::stable_sort(fields.begin(), fields.end(), [](const FieldLayoutInfo &lhs, const FieldLayoutInfo &rhs)
    {
        if (lhs.Offset != rhs.Offset)
            return lhs.Offset < rhs.Offset;
        if (lhs.IsBitfield != rhs.IsBitfield)
            return lhs.IsBitfield;
        if (lhs.IsBitfield && rhs.IsBitfield && lhs.BitIndex != rhs.BitIndex)
            return lhs.BitIndex < rhs.BitIndex;
        return reinterpret_cast<uintptr_t>(lhs.Field.GetAddress()) < reinterpret_cast<uintptr_t>(rhs.Field.GetAddress());
    });

    return fields;
}

int32_t GetOwnMemberStart(const UE_UStruct &strct)
{
    const UE_UStruct super = strct.GetSuper();
    if (!super)
        return 0;

    const StructLayoutInfo &superInfo = GetStructLayoutInfo(super);
    return superInfo.HasReusedTrailingPadding ? superInfo.Size : superInfo.AlignedSize;
}

int32_t GetDisplayStructSize(const UE_UStruct &strct)
{
    const StructLayoutInfo &info = GetStructLayoutInfo(strct);
    if (info.AlignedSize > 0)
        return info.AlignedSize;
    return strct.GetSize();
}

void Warmup()
{
    BuildLayoutCache();
}

void Invalidate()
{
    gCachedGeneration = 0;
    ResetCaches();
}
}  // namespace AutoFixStructLayout
