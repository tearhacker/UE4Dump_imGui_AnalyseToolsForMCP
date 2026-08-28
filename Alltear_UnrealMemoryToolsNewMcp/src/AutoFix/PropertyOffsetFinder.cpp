#include "PropertyOffsetFinder.hpp"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "../UE/UEWrappers.hpp"
#include "../Utils/Logger.hpp"

using namespace UEMemory;

namespace AutoFixPropertyOffsets
{
namespace
{
constexpr uintptr_t kNotFound = 0;
constexpr int32_t kMaxFieldCount = 2048;

uint32_t gCachedGeneration = 0;
bool gResolved = false;

bool IsLikelyPointer(uintptr_t value)
{
    return value >= 0x10000;
}

uintptr_t ReadPtr(uintptr_t address)
{
    return vm_rpm_ptr<uintptr_t>(reinterpret_cast<void *>(address));
}

uint8_t ReadU8(uintptr_t address)
{
    return vm_rpm_ptr<uint8_t>(reinterpret_cast<void *>(address));
}

bool IsPowerOfTwoByte(uint8_t value)
{
    return value != 0 && (value & (value - 1)) == 0;
}

bool IsBoolFieldMask(uint8_t value)
{
    return value == 0xFF || IsPowerOfTwoByte(value);
}

UE_UClass FindClassByName(const std::string &name)
{
    UE_UClass result;
    if (auto *objects = UEWrappers::GetObjects())
    {
        objects->ForEachObjectOfClass(UE_UClass::StaticClass(), [&](UE_UObject object)
        {
            if (object.GetName() == name)
            {
                result = object.Cast<UE_UClass>();
                return true;
            }
            return false;
        });
    }
    return result;
}

UE_UStruct FindStructByName(const std::string &name)
{
    UE_UStruct result;
    if (auto *objects = UEWrappers::GetObjects())
    {
        objects->ForEachObjectOfClass(UE_UStruct::StaticClass(), [&](UE_UObject object)
        {
            if (object.IsA<UE_UFunction>())
                return false;
            if (object.GetName() == name)
            {
                result = object.Cast<UE_UStruct>();
                return true;
            }
            return false;
        });
    }
    return result;
}

UE_UEnum FindEnumByName(const std::string &name)
{
    UE_UEnum result;
    if (auto *objects = UEWrappers::GetObjects())
    {
        objects->ForEachObjectOfClass(UE_UEnum::StaticClass(), [&](UE_UObject object)
        {
            if (object.GetName() == name)
            {
                result = object.Cast<UE_UEnum>();
                return true;
            }
            return false;
        });
    }
    return result;
}

UE_UFunction FindFunctionByName(const std::string &name)
{
    UE_UFunction result;
    if (auto *objects = UEWrappers::GetObjects())
    {
        objects->ForEachObjectOfClass(UE_UFunction::StaticClass(), [&](UE_UObject object)
        {
            if (object.GetName() == name)
            {
                result = object.Cast<UE_UFunction>();
                return true;
            }
            return false;
        });
    }
    return result;
}

std::string GetFieldClassName(const UE_FField &field)
{
    return field ? field.GetClass().GetName() : std::string();
}

UE_FProperty FindMember(UE_UStruct owner, const std::string &memberName, const std::string &propertyClassName = {}, bool includeSupers = true)
{
    for (int32_t depth = 0; owner && depth < 64; ++depth)
    {
        int32_t count = 0;
        for (UE_FField field = owner.GetChildProperties(); field && count < kMaxFieldCount; field = field.GetNext(), ++count)
        {
            if (field.GetName() != memberName)
                continue;
            if (!propertyClassName.empty() && GetFieldClassName(field) != propertyClassName)
                continue;
            return field.Cast<UE_FProperty>();
        }

        if (!includeSupers)
            break;
        owner = owner.GetSuper();
    }

    return {};
}

bool IsPropertyPointer(uintptr_t value)
{
    if (!IsLikelyPointer(value))
        return false;
    const std::string className = UE_FField(reinterpret_cast<uint8_t *>(value)).GetClass().GetName();
    return className.find("Property") != std::string::npos;
}

uintptr_t AlignPointerOffset(uintptr_t value)
{
    const uintptr_t mask = sizeof(void *) - 1;
    return (value + mask) & ~mask;
}

uintptr_t FindPointerOffset(const std::vector<std::pair<UE_FProperty, uintptr_t>> &infos, uintptr_t minOffset, uintptr_t maxOffset, int32_t minRequired = 1)
{
    if (infos.empty())
        return kNotFound;

    for (uintptr_t offset = AlignPointerOffset(minOffset); offset <= maxOffset; offset += sizeof(void *))
    {
        int32_t validPairs = 0;
        int32_t matches = 0;
        for (const auto &[field, expected] : infos)
        {
            if (!field || !IsLikelyPointer(expected))
                continue;
            ++validPairs;
            if (ReadPtr(reinterpret_cast<uintptr_t>(field.GetAddress()) + offset) == expected)
                ++matches;
        }

        if (validPairs >= minRequired && matches == validPairs)
            return offset;
    }

    return kNotFound;
}

uintptr_t FindByteOffset(const std::vector<std::pair<UE_FProperty, uint8_t>> &infos, uintptr_t minOffset, uintptr_t maxOffset, int32_t minRequired = 1)
{
    if (infos.empty())
        return kNotFound;

    for (uintptr_t offset = minOffset; offset <= maxOffset; ++offset)
    {
        int32_t validPairs = 0;
        int32_t matches = 0;
        for (const auto &[field, expected] : infos)
        {
            if (!field)
                continue;
            ++validPairs;
            if (ReadU8(reinterpret_cast<uintptr_t>(field.GetAddress()) + offset) == expected)
                ++matches;
        }

        if (validPairs >= minRequired && matches == validPairs)
            return offset;
    }

    return kNotFound;
}

uintptr_t FindFirstPropertyPointerOffset(const UE_FProperty &field, uintptr_t minOffset, uintptr_t maxOffset)
{
    if (!field)
        return kNotFound;

    for (uintptr_t offset = AlignPointerOffset(minOffset); offset <= maxOffset; offset += sizeof(void *))
    {
        if (IsPropertyPointer(ReadPtr(reinterpret_cast<uintptr_t>(field.GetAddress()) + offset)))
            return offset;
    }

    return kNotFound;
}

uintptr_t FindMapPropertyBaseOffset(const UE_FProperty &field, uintptr_t minOffset, uintptr_t maxOffset)
{
    if (!field)
        return kNotFound;

    for (uintptr_t offset = AlignPointerOffset(minOffset); offset + sizeof(void *) <= maxOffset; offset += sizeof(void *))
    {
        const uintptr_t keyProp = ReadPtr(reinterpret_cast<uintptr_t>(field.GetAddress()) + offset);
        const uintptr_t valueProp = ReadPtr(reinterpret_cast<uintptr_t>(field.GetAddress()) + offset + sizeof(void *));
        if (IsPropertyPointer(keyProp) && IsPropertyPointer(valueProp))
            return offset;
    }

    return kNotFound;
}

uintptr_t FindBoolPropertyBaseOffset(const UE_Offsets &offsets)
{
    const uintptr_t minOffset = offsets.FProperty.Offset_Internal ? offsets.FProperty.Offset_Internal : offsets.FProperty.Size;
    const uintptr_t maxOffset = offsets.FProperty.Size + 0x40;

    std::vector<std::pair<UE_FProperty, uint8_t>> anchors;
    if (UE_UClass engine = FindClassByName("Engine"))
    {
        anchors.emplace_back(FindMember(engine, "bIsOverridingSelectedColor", "BoolProperty"), 0xFF);
        anchors.emplace_back(FindMember(engine, "bEnableOnScreenDebugMessagesDisplay", "BoolProperty"), 0x02);
    }
    if (UE_UClass playerController = FindClassByName("PlayerController"))
        anchors.emplace_back(FindMember(playerController, "bAutoManageActiveCameraTarget", "BoolProperty"), 0xFF);

    const uintptr_t fieldMaskOffset = FindByteOffset(anchors, minOffset, maxOffset, 2);
    if (fieldMaskOffset >= 3)
        return fieldMaskOffset - 3;

    std::vector<UE_FProperty> boolFields;
    if (auto *objects = UEWrappers::GetObjects())
    {
        objects->ForEachObjectOfClass(UE_UStruct::StaticClass(), [&](UE_UObject object)
        {
            if (object.IsA<UE_UFunction>())
                return false;

            int32_t count = 0;
            for (UE_FField field = object.Cast<UE_UStruct>().GetChildProperties(); field && count < kMaxFieldCount; field = field.GetNext(), ++count)
            {
                if (GetFieldClassName(field) == "BoolProperty")
                    boolFields.push_back(field.Cast<UE_FProperty>());
                if (boolFields.size() >= 128)
                    return true;
            }
            return false;
        });
    }

    uintptr_t bestOffset = kNotFound;
    int32_t bestScore = 0;
    for (uintptr_t base = offsets.FProperty.Size; base <= maxOffset; ++base)
    {
        int32_t score = 0;
        for (const UE_FProperty &field : boolFields)
        {
            const uintptr_t address = reinterpret_cast<uintptr_t>(field.GetAddress()) + base;
            const uint8_t fieldSize = ReadU8(address);
            const uint8_t byteOffset = ReadU8(address + 1);
            const uint8_t byteMask = ReadU8(address + 2);
            const uint8_t fieldMask = ReadU8(address + 3);
            if ((fieldSize == 1 || fieldSize == 2 || fieldSize == 4 || fieldSize == 8) &&
                byteOffset < fieldSize &&
                (byteMask == 0xFF || IsPowerOfTwoByte(byteMask)) &&
                IsBoolFieldMask(fieldMask))
            {
                ++score;
            }
        }

        if (score > bestScore)
        {
            bestScore = score;
            bestOffset = base;
        }
    }

    return bestScore >= 4 ? bestOffset : kNotFound;
}

void LogFound(const char *name, uintptr_t value)
{
    if (value)
        LOGI("[AutoFix] %s=0x%lx", name, static_cast<unsigned long>(value));
}

void ResolveOffsets()
{
    auto *offsets = UEWrappers::GetOffsets();
    if (!offsets || !offsets->Config.IsUsingFNamePool || !offsets->UStruct.ChildProperties || !offsets->FProperty.Size)
        return;

    const uintptr_t propertySize = offsets->FProperty.Size;
    const uintptr_t searchMin = offsets->FProperty.Offset_Internal ? offsets->FProperty.Offset_Internal : propertySize;
    const uintptr_t searchMax = propertySize + 0x50;

    if (UE_UClass controller = FindClassByName("Controller"))
    {
        const UE_UClass playerStateClass = FindClassByName("PlayerState");
        const UE_UClass pawnClass = FindClassByName("Pawn");
        const uintptr_t objectClassOffset = FindPointerOffset({
                                                                   {FindMember(controller, "PlayerState", "ObjectProperty"), reinterpret_cast<uintptr_t>(playerStateClass.GetAddress())},
                                                                   {FindMember(controller, "Pawn", "ObjectProperty"), reinterpret_cast<uintptr_t>(pawnClass.GetAddress())},
                                                               },
                                                               searchMin, searchMax, 1);
        if (objectClassOffset)
            offsets->ObjectProperty.PropertyClass = objectClassOffset;
    }

    if (UE_UClass world = FindClassByName("World"))
    {
        const UE_UClass levelClass = FindClassByName("Level");
        const uintptr_t objectClassOffset = FindPointerOffset({{FindMember(world, "PersistentLevel", "ObjectProperty"), reinterpret_cast<uintptr_t>(levelClass.GetAddress())}},
                                                              searchMin, searchMax, 1);
        if (objectClassOffset)
            offsets->ObjectProperty.PropertyClass = objectClassOffset;
    }

    if (UE_UStruct twoVectors = FindStructByName("TwoVectors"))
    {
        UE_UStruct vectorStruct = FindStructByName("Vector");
        if (!vectorStruct)
            vectorStruct = FindStructByName("vector");
        const uintptr_t structOffset = FindPointerOffset({
                                                             {FindMember(twoVectors, "v1", "StructProperty"), reinterpret_cast<uintptr_t>(vectorStruct.GetAddress())},
                                                             {FindMember(twoVectors, "v2", "StructProperty"), reinterpret_cast<uintptr_t>(vectorStruct.GetAddress())},
                                                         },
                                                         searchMin, searchMax, 1);
        if (structOffset)
            offsets->StructProperty.Struct = structOffset;
    }

    if (UE_UStruct collisionResponseContainer = FindStructByName("CollisionResponseContainer"))
    {
        const UE_UEnum collisionResponseEnum = FindEnumByName("ECollisionResponse");
        const uintptr_t byteEnumOffset = FindPointerOffset({
                                                                {FindMember(collisionResponseContainer, "GameTraceChannel1", "ByteProperty"), reinterpret_cast<uintptr_t>(collisionResponseEnum.GetAddress())},
                                                                {FindMember(collisionResponseContainer, "GameTraceChannel2", "ByteProperty"), reinterpret_cast<uintptr_t>(collisionResponseEnum.GetAddress())},
                                                            },
                                                            searchMin, searchMax, 1);
        if (byteEnumOffset)
            offsets->ByteProperty.Enum = byteEnumOffset;
    }

    if (const uintptr_t boolBaseOffset = FindBoolPropertyBaseOffset(*offsets))
        offsets->BoolProperty.Base = boolBaseOffset;

    if (UE_UClass actorComponent = FindClassByName("ActorComponent"))
    {
        const UE_UEnum creationMethodEnum = FindEnumByName("EComponentCreationMethod");
        const UE_FProperty creationMethod = FindMember(actorComponent, "CreationMethod", "EnumProperty");
        if (creationMethod && creationMethodEnum)
        {
            const uintptr_t enumOffset = FindPointerOffset({{creationMethod, reinterpret_cast<uintptr_t>(creationMethodEnum.GetAddress())}}, searchMin, searchMax, 1);
            if (enumOffset >= sizeof(void *))
            {
                offsets->EnumProperty.UnderlayingProp = enumOffset - sizeof(void *);
                offsets->EnumProperty.Enum = enumOffset;
            }
        }
    }

    if (UE_UClass pawn = FindClassByName("Pawn"))
    {
        const UE_UEnum autoPossessAIEnum = FindEnumByName("EAutoPossessAI");
        const UE_FProperty autoPossessAI = FindMember(pawn, "AutoPossessAI", "EnumProperty");
        if (autoPossessAI && autoPossessAIEnum)
        {
            const uintptr_t enumOffset = FindPointerOffset({{autoPossessAI, reinterpret_cast<uintptr_t>(autoPossessAIEnum.GetAddress())}}, searchMin, searchMax, 1);
            if (enumOffset >= sizeof(void *))
            {
                offsets->EnumProperty.UnderlayingProp = enumOffset - sizeof(void *);
                offsets->EnumProperty.Enum = enumOffset;
            }
        }
    }

    if (const UE_UFunction delegateSignature = FindFunctionByName("TimerDynamicDelegate__DelegateSignature"))
    {
        const UE_UFunction elapsed = FindFunctionByName("K2_GetTimerElapsedTimeDelegate");
        const UE_UFunction remaining = FindFunctionByName("K2_GetTimerRemainingTimeDelegate");
        const uintptr_t delegateOffset = FindPointerOffset({
                                                                {elapsed ? FindMember(elapsed, "Delegate", "DelegateProperty") : UE_FProperty(), reinterpret_cast<uintptr_t>(delegateSignature.GetAddress())},
                                                                {remaining ? FindMember(remaining, "Delegate", "DelegateProperty") : UE_FProperty(), reinterpret_cast<uintptr_t>(delegateSignature.GetAddress())},
                                                            },
                                                            searchMin, searchMax, 1);
        if (delegateOffset)
            offsets->DelegateProperty.SignatureFunction = delegateOffset;
    }

    if (UE_UClass gameViewportClient = FindClassByName("GameViewportClient"))
    {
        const uintptr_t innerOffset = FindFirstPropertyPointerOffset(FindMember(gameViewportClient, "DebugProperties", "ArrayProperty"), propertySize, searchMax);
        if (innerOffset)
            offsets->ArrayProperty.Inner = innerOffset;
    }

    if (UE_UStruct levelCollection = FindStructByName("LevelCollection"))
    {
        const uintptr_t setOffset = FindFirstPropertyPointerOffset(FindMember(levelCollection, "Levels", "SetProperty"), propertySize, searchMax);
        if (setOffset)
            offsets->SetProperty.ElementProp = setOffset;
    }

    if (UE_UClass userDefinedEnum = FindClassByName("UserDefinedEnum"))
    {
        const uintptr_t mapOffset = FindMapPropertyBaseOffset(FindMember(userDefinedEnum, "DisplayNameMap", "MapProperty"), propertySize, searchMax);
        if (mapOffset)
        {
            offsets->MapProperty.KeyProp = mapOffset;
            offsets->MapProperty.ValueProp = mapOffset + sizeof(void *);
        }
    }

    if (offsets->ObjectProperty.PropertyClass)
        offsets->ClassProperty.MetaClass = offsets->ObjectProperty.PropertyClass + sizeof(void *);
    if (offsets->ObjectProperty.PropertyClass)
        offsets->InterfaceProperty.InterfaceClass = offsets->ObjectProperty.PropertyClass;

    LogFound("ObjectProperty.PropertyClass", offsets->ObjectProperty.PropertyClass);
    LogFound("StructProperty.Struct", offsets->StructProperty.Struct);
    LogFound("ByteProperty.Enum", offsets->ByteProperty.Enum);
    LogFound("BoolProperty.Base", offsets->BoolProperty.Base);
    LogFound("EnumProperty.UnderlayingProp", offsets->EnumProperty.UnderlayingProp);
    LogFound("EnumProperty.Enum", offsets->EnumProperty.Enum);
    LogFound("DelegateProperty.SignatureFunction", offsets->DelegateProperty.SignatureFunction);
    LogFound("ArrayProperty.Inner", offsets->ArrayProperty.Inner);
    LogFound("SetProperty.ElementProp", offsets->SetProperty.ElementProp);
    LogFound("MapProperty.KeyProp", offsets->MapProperty.KeyProp);
    LogFound("MapProperty.ValueProp", offsets->MapProperty.ValueProp);
    LogFound("ClassProperty.MetaClass", offsets->ClassProperty.MetaClass);
    LogFound("InterfaceProperty.InterfaceClass", offsets->InterfaceProperty.InterfaceClass);
}
}  // namespace

void Invalidate()
{
    gCachedGeneration = 0;
    gResolved = false;
}

void EnsureResolved()
{
    const uint32_t generation = UEWrappers::GetInitGeneration();
    if (gResolved && gCachedGeneration == generation)
        return;

    gCachedGeneration = generation;
    gResolved = true;
    ResolveOffsets();
}
}  // namespace AutoFixPropertyOffsets
