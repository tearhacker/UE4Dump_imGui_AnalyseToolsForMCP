#include "AutoFixOffsets.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <string>
#include <vector>

#include "PropertyOffsetFinder.hpp"
#include "StructLayout.hpp"
#include "../UE/UEMemory.hpp"
#include "../UE/UEGameProfile.hpp"
#include "../UE/UEWrappers.hpp"
#include "../Utils/Logger.hpp"

using namespace UEMemory;

namespace AutoFix
{
    namespace
    {
        constexpr uintptr_t kNameScanStart = 0x08;
        constexpr uintptr_t kNameScanEnd = 0x7C;
        constexpr int kStrictSampleCap = 0x40;
        constexpr int kLooseSampleCap = 0x400;

        UE_Offsets *Offsets() { return UEWrappers::GetOffsets(); }
        const UEVars *Vars() { return UEWrappers::GetUEVars(); }
        UE_UObjectArray *Objects() { return UEWrappers::GetObjects(); }
        std::string ReadObjectName(uintptr_t object, uintptr_t nameOffset);
        bool IsLikelyPtr(uintptr_t p);

        uintptr_t BaseAddress()
        {
            return UEWrappers::GetBaseAddress();
        }

        uintptr_t ReadPtr(uintptr_t address)
        {
            return vm_rpm_ptr<uintptr_t>((const void *)address);
        }

        int32_t ReadI32(uintptr_t address)
        {
            return vm_rpm_ptr<int32_t>((const void *)address);
        }

        int16_t ReadI16(uintptr_t address)
        {
            return vm_rpm_ptr<int16_t>((const void *)address);
        }

        int8_t ReadI8(uintptr_t address)
        {
            return vm_rpm_ptr<int8_t>((const void *)address);
        }

        uint32_t ReadU32(uintptr_t address)
        {
            return vm_rpm_ptr<uint32_t>((const void *)address);
        }

        uint64_t ReadU64(uintptr_t address)
        {
            return vm_rpm_ptr<uint64_t>((const void *)address);
        }

        bool IsPlausibleArrayCount(int32_t count, int32_t maxCount, int32_t maxAllowed = 0x1000)
        {
            return count >= 0 && count <= maxAllowed && maxCount >= count && maxCount <= std::max(maxAllowed, count + 0x100);
        }

        bool IsObjectAByClassName(uintptr_t object, const char *targetClassName, int maxDepth = 16)
        {
            UE_Offsets *off = Offsets();
            if (!off || !object || !off->UObject.ClassPrivate || !off->UObject.NamePrivate || !off->UStruct.SuperStruct)
                return false;

            uintptr_t classPtr = ReadPtr(object + off->UObject.ClassPrivate);
            int depth = 0;
            while (IsLikelyPtr(classPtr) && depth++ < maxDepth)
            {
                if (ReadObjectName(classPtr, off->UObject.NamePrivate) == targetClassName)
                    return true;
                classPtr = ReadPtr(classPtr + off->UStruct.SuperStruct);
            }
            return false;
        }

        UE_UClass FindClassByFullName(const char *fullName)
        {
            return Objects() ? Objects()->FindObject(fullName).Cast<UE_UClass>() : UE_UClass();
        }

        UE_UClass FindClassByName(const char *name)
        {
            return Objects() ? Objects()->FindObjectFast(name).Cast<UE_UClass>() : UE_UClass();
        }

        UE_UStruct FindStructByName(const char *name)
        {
            return Objects() ? Objects()->FindObjectFast(name).Cast<UE_UStruct>() : UE_UStruct();
        }

        std::string ReadNameById(int32_t id)
        {
            if (id <= 0 || id > 0x800000)
                return "";
            return UEWrappers::GetNameByID(id);
        }

        std::string ReadNameAt(uintptr_t address)
        {
            return ReadNameById(ReadI32(address));
        }

        std::string ReadObjectName(uintptr_t object, uintptr_t nameOffset)
        {
            if (object == 0 || nameOffset == 0)
                return "";
            return ReadNameAt(object + nameOffset);
        }

        bool IsLikelyPtr(uintptr_t p)
        {
            return p >= 0x10000 && kPtrValidator.isPtrReadable((const void *)p);
        }

        bool IsExecutableInModule(uintptr_t p)
        {
            const uintptr_t base = BaseAddress();
            if (!base || p <= base)
                return false;

            const uintptr_t rva = p - base;
            return rva < 0x30000000 && kPtrValidator.isPtrExecutable((const void *)p);
        }

        bool StartsWith(const std::string &value, const char *prefix)
        {
            const std::string p = prefix ? prefix : "";
            return value.size() >= p.size() && value.compare(0, p.size(), p) == 0;
        }

        bool IsIdentifierLike(const std::string &name)
        {
            if (name.size() < 2 || name.size() > 128)
                return false;
            if (!std::isalpha(static_cast<unsigned char>(name[0])) && name[0] != '_')
                return false;

            for (unsigned char c : name)
            {
                if (!(std::isalnum(c) || c == '_'))
                    return false;
            }
            return name != "None" && name != "NULL";
        }

        bool IsUELikeName(const std::string &name)
        {
            if (name.empty())
                return false;
            if (StartsWith(name, "/Script/"))
                return true;
            if (name.find('.') != std::string::npos)
                return true;
            return IsIdentifierLike(name);
        }

        bool IsClassNameLike(const std::string &name)
        {
            if (name.size() < 2 || name.size() > 64)
                return false;
            const unsigned char first = static_cast<unsigned char>(name[0]);
            if (!std::isupper(first))
                return false;
            for (unsigned char c : name)
            {
                if (!(std::isalnum(c) || c == '_'))
                    return false;
            }
            return name != "None" && name != "NULL";
        }

        bool IsPropertyClassName(const std::string &name)
        {
            return !name.empty() && name.find("Property") != std::string::npos;
        }

        bool IsFFieldClassNameLike(const std::string &name)
        {
            return IsPropertyClassName(name) ||
                   name == "Field" ||
                   name == "Property" ||
                   name == "EnumProperty" ||
                   name == "StructProperty" ||
                   name == "ObjectProperty" ||
                   name == "ObjectPropertyBase" ||
                   name == "ClassProperty" ||
                   name == "InterfaceProperty" ||
                   name == "DelegateProperty" ||
                   name == "MulticastDelegateProperty" ||
                   name == "FieldPathProperty";
        }

        bool IsLikelyBitmaskU64(uint64_t value)
        {
            return value != 0 && value != std::numeric_limits<uint64_t>::max() && !IsLikelyPtr(static_cast<uintptr_t>(value));
        }

        std::string ReadFFieldClassName(uintptr_t classPtr)
        {
            UE_Offsets *off = Offsets();
            if (!off || !IsLikelyPtr(classPtr))
                return "";

            const std::string configured = ReadNameAt(classPtr + off->FFieldClass.Name);
            if (IsFFieldClassNameLike(configured) || IsClassNameLike(configured))
                return configured;

            const std::string nameAtZero = ReadNameAt(classPtr);
            if (IsFFieldClassNameLike(nameAtZero))
                return nameAtZero;

            if (off->UObject.NamePrivate)
            {
                const std::string objectStyleName = ReadObjectName(classPtr, off->UObject.NamePrivate);
                if (IsPropertyClassName(objectStyleName))
                    return objectStyleName;
                if (IsClassNameLike(objectStyleName))
                    return objectStyleName;
            }

            if (IsClassNameLike(nameAtZero))
                return nameAtZero;

            return "";
        }

        bool MatchesAnyName(const std::string &value, std::initializer_list<const char *> candidates)
        {
            for (const char *candidate : candidates)
            {
                if (candidate && value == candidate)
                    return true;
            }
            return false;
        }

        bool IsKnownWorldPropertyName(const std::string &name)
        {
            return MatchesAnyName(name, {"PersistentLevel", "OwningGameInstance", "NetDriver", "DemoNetDriver"});
        }

        bool IsKnownWorldFunctionName(const std::string &name)
        {
            return MatchesAnyName(name, {"K2_GetWorldSettings", "UserConstructionScript", "ReceiveTick", "ExecuteUbergraph"});
        }

        uintptr_t FindNamedFFieldInOwner(const char *ownerFullName, std::initializer_list<const char *> fieldNames)
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects() || !off->UStruct.ChildProperties || !off->FField.NamePrivate || !off->FField.Next)
                return 0;

            const UE_UClass owner = Objects()->FindObject(ownerFullName).Cast<UE_UClass>();
            if (!owner)
                return 0;

            uintptr_t prop = ReadPtr(reinterpret_cast<uintptr_t>(owner.GetAddress()) + off->UStruct.ChildProperties);
            int depth = 0;
            while (IsLikelyPtr(prop) && depth < 256)
            {
                const std::string propName = ReadObjectName(prop, off->FField.NamePrivate);
                if (MatchesAnyName(propName, fieldNames))
                    return prop;
                prop = ReadPtr(prop + off->FField.Next);
                ++depth;
            }

            return 0;
        }

        uintptr_t FindAnyFFieldClassByName(const char *className)
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects() || !off->UStruct.ChildProperties || !off->FField.ClassPrivate || !off->FField.Next)
                return 0;

            const UE_UClass classClass = UE_UClass::StaticClass();
            if (!classClass)
                return 0;

            uintptr_t found = 0;
            int scannedClasses = 0;
            Objects()->ForEachObjectOfClass(classClass, [&](UE_UObject object)
            {
                if (found || scannedClasses >= 512)
                    return true;
                ++scannedClasses;

                const UE_UClass owner = object.Cast<UE_UClass>();
                int guard = 0;
                for (UE_FField field = owner.GetChildProperties(); field && guard < 256; field = field.GetNext(), ++guard)
                {
                    const uintptr_t fclass = reinterpret_cast<uintptr_t>(field.GetClass().GetAddress());
                    if (!IsLikelyPtr(fclass))
                        continue;
                    if (ReadFFieldClassName(fclass) == className)
                    {
                        found = fclass;
                        return true;
                    }
                }
                return false;
            });

            return found;
        }

        uintptr_t FindPropertyLayoutAnchor()
        {
            if (const uintptr_t worldProp = FindNamedFFieldInOwner("Class Engine.World",
                                                                   {"PersistentLevel", "OwningGameInstance", "NetDriver", "DemoNetDriver"}))
            {
                return worldProp;
            }

            if (const uintptr_t engineProp = FindNamedFFieldInOwner("Class Engine.Engine",
                                                                    {"TinyFont", "SmallFont", "MediumFont", "LargeFont"}))
            {
                return engineProp;
            }

            return 0;
        }

        uintptr_t FindPropertySizeAnchor()
        {
            if (const uintptr_t engineProp = FindNamedFFieldInOwner("Class Engine.Engine",
                                                                    {"TinyFont", "SmallFont", "MediumFont", "LargeFont"}))
            {
                return engineProp;
            }

            return FindPropertyLayoutAnchor();
        }

        struct FPropertyLayoutScanResult
        {
            uintptr_t ArrayDim = 0;
            uintptr_t ElementSize = 0;
            uintptr_t PropertyFlags = 0;
            uintptr_t OffsetInternal = 0;
            uintptr_t Size = 0;
        };

        bool IsReasonablePropertyMemberOffset(int32_t value)
        {
            return value > 0 && value < 0x2000 && (value % 4) == 0;
        }

        bool MatchesExpectedPropertyClassName(const std::string &anchorFieldName, const std::string &targetObjectName)
        {
            if (anchorFieldName == "PersistentLevel")
                return targetObjectName == "Level";
            if (anchorFieldName == "OwningGameInstance")
                return targetObjectName == "GameInstance";
            if (anchorFieldName == "NetDriver" || anchorFieldName == "DemoNetDriver")
                return targetObjectName == "NetDriver";
            if (anchorFieldName == "TinyFont" || anchorFieldName == "SmallFont" ||
                anchorFieldName == "MediumFont" || anchorFieldName == "LargeFont")
            {
                return targetObjectName == "Font";
            }
            return false;
        }

        bool ProbeFPropertySizeFromAnchor(uintptr_t prop, uintptr_t &outSize)
        {
            UE_Offsets *off = Offsets();
            if (!prop || !off || !off->FField.NamePrivate || !off->UObject.NamePrivate)
                return false;

            const std::string anchorName = ReadObjectName(prop, off->FField.NamePrivate);
            if (anchorName.empty())
                return false;

            for (uintptr_t sizeOff = 0x40; sizeOff <= 0xA0; sizeOff += sizeof(int32_t))
            {
                const uintptr_t candidate = ReadPtr(prop + sizeOff);
                if (!IsLikelyPtr(candidate))
                    continue;

                const std::string targetName = ReadObjectName(candidate, off->UObject.NamePrivate);
                if (!MatchesExpectedPropertyClassName(anchorName, targetName))
                    continue;

                outSize = sizeOff;
                return true;
            }

            return false;
        }

        bool TryResolveFPropertyLayout(uintptr_t prop,
                                       uintptr_t arrayDimOff,
                                       uintptr_t sizeOff,
                                       FPropertyLayoutScanResult &out)
        {
            if (!prop || !arrayDimOff)
                return false;

            const int32_t arrayDim = ReadI32(prop + arrayDimOff);
            const int32_t elemSize = ReadI32(prop + arrayDimOff + sizeof(int32_t));
            if (arrayDim != 1 || (elemSize != 4 && elemSize != 8))
                return false;

            for (uintptr_t offDelta : {static_cast<uintptr_t>(0x14), static_cast<uintptr_t>(0x18)})
            {
                const uintptr_t offsetInternalOff = arrayDimOff + offDelta;
                const int32_t offsetInternal = ReadI32(prop + offsetInternalOff);
                if (!IsReasonablePropertyMemberOffset(offsetInternal))
                    continue;

                out.ArrayDim = arrayDimOff;
                out.ElementSize = arrayDimOff + sizeof(int32_t);
                out.PropertyFlags = arrayDimOff + 0x8;
                out.OffsetInternal = offsetInternalOff;
                out.Size = sizeOff;
                return true;
            }

            return false;
        }

        bool ProbeFPropertyLayoutFromAnchor(uintptr_t prop,
                                            uintptr_t knownSize,
                                            FPropertyLayoutScanResult &out)
        {
            if (!prop)
                return false;

            for (uintptr_t arrayDimOff = 0x2C; arrayDimOff <= 0x48; arrayDimOff += sizeof(int32_t))
            {
                if (TryResolveFPropertyLayout(prop, arrayDimOff, knownSize, out))
                    return true;
            }

            if (knownSize)
            {
                for (uintptr_t delta : {static_cast<uintptr_t>(0x40), static_cast<uintptr_t>(0x44)})
                {
                    if (knownSize <= delta)
                        continue;

                    if (TryResolveFPropertyLayout(prop, knownSize - delta, knownSize, out))
                        return true;
                }
            }

            return false;
        }

        void ApplyDefaultsOnly()
        {
            UE_Offsets *off = Offsets();
            if (!off)
                return;

            if (off->FName.Size == 0)
            {
                off->FName.Size = UE_DefaultOffsets::kGetFNameSize(off->Config.isUsingCasePreservingName,
                                                                   off->Config.isUsingOutlineNumberName);
            }

            if (off->FUObjectArray.ObjObjects == 0)
                off->FUObjectArray.ObjObjects = sizeof(int32_t) * 4;
            if (off->TUObjectArray.Objects == 0)
                off->TUObjectArray.Objects = 0;
            if (off->TUObjectArray.NumElements == 0)
            {
                off->TUObjectArray.NumElements = (off->TUObjectArray.NumElementsPerChunk > 0)
                    ? ((sizeof(void *) * 2) + sizeof(int32_t))
                    : (sizeof(void *) + sizeof(int32_t));
            }
            if (off->FUObjectItem.Object == 0)
                off->FUObjectItem.Object = 0;
            if (off->FUObjectItem.Size == 0)
                off->FUObjectItem.Size = GetPtrAlignedOf(sizeof(void *) + (sizeof(int32_t) * 3));
        }

        void PrintField(const char *label, uintptr_t before, uintptr_t after)
        {
            if (before == after)
                LOGI("[AutoFix]   %-32s = 0x%lx", label, (unsigned long)after);
            else
                LOGI("[AutoFix]   %-32s = 0x%lx  (was 0x%lx)", label, (unsigned long)after, (unsigned long)before);
        }

        void DumpAutoFoundOffsets(const UE_Offsets &before, const UE_Offsets &after)
        {
            LOGI("[AutoFix] ============== Auto-Found Offsets ==============");
            PrintField("FUObjectArray.ObjObjects", before.FUObjectArray.ObjObjects, after.FUObjectArray.ObjObjects);
            PrintField("TUObjectArray.Objects", before.TUObjectArray.Objects, after.TUObjectArray.Objects);
            PrintField("TUObjectArray.NumElements", before.TUObjectArray.NumElements, after.TUObjectArray.NumElements);
            PrintField("TUObjectArray.NumElementsPerChunk", before.TUObjectArray.NumElementsPerChunk, after.TUObjectArray.NumElementsPerChunk);
            PrintField("FUObjectItem.Object", before.FUObjectItem.Object, after.FUObjectItem.Object);
            PrintField("FUObjectItem.Size", before.FUObjectItem.Size, after.FUObjectItem.Size);
            PrintField("UObject.ObjectFlags", before.UObject.ObjectFlags, after.UObject.ObjectFlags);
            PrintField("UObject.InternalIndex", before.UObject.InternalIndex, after.UObject.InternalIndex);
            PrintField("UObject.ClassPrivate", before.UObject.ClassPrivate, after.UObject.ClassPrivate);
            PrintField("UObject.NamePrivate", before.UObject.NamePrivate, after.UObject.NamePrivate);
            PrintField("UObject.OuterPrivate", before.UObject.OuterPrivate, after.UObject.OuterPrivate);
            PrintField("UField.Next", before.UField.Next, after.UField.Next);
            PrintField("UEnum.Names", before.UEnum.Names, after.UEnum.Names);
            PrintField("UStruct.SuperStruct", before.UStruct.SuperStruct, after.UStruct.SuperStruct);
            PrintField("UStruct.Children", before.UStruct.Children, after.UStruct.Children);
            PrintField("UStruct.ChildProperties", before.UStruct.ChildProperties, after.UStruct.ChildProperties);
            PrintField("UStruct.PropertiesSize", before.UStruct.PropertiesSize, after.UStruct.PropertiesSize);
            PrintField("UStruct.MinAlignment", before.UStruct.MinAlignment, after.UStruct.MinAlignment);
            PrintField("UClass.ClassDefaultObject", before.UClass.ClassDefaultObject, after.UClass.ClassDefaultObject);
            PrintField("UClass.ImplementedInterfaces", before.UClass.ImplementedInterfaces, after.UClass.ImplementedInterfaces);
            PrintField("UClass.CastFlags", before.UClass.CastFlags, after.UClass.CastFlags);
            PrintField("UClass.ClassFlags", before.UClass.ClassFlags, after.UClass.ClassFlags);
            PrintField("UFunction.EFunctionFlags", before.UFunction.EFunctionFlags, after.UFunction.EFunctionFlags);
            PrintField("UFunction.NumParams", before.UFunction.NumParams, after.UFunction.NumParams);
            PrintField("UFunction.ParamSize", before.UFunction.ParamSize, after.UFunction.ParamSize);
            PrintField("UFunction.Func", before.UFunction.Func, after.UFunction.Func);
            PrintField("UProperty.ArrayDim", before.UProperty.ArrayDim, after.UProperty.ArrayDim);
            PrintField("UProperty.ElementSize", before.UProperty.ElementSize, after.UProperty.ElementSize);
            PrintField("UProperty.PropertyFlags", before.UProperty.PropertyFlags, after.UProperty.PropertyFlags);
            PrintField("UProperty.Offset_Internal", before.UProperty.Offset_Internal, after.UProperty.Offset_Internal);
            PrintField("UProperty.Size", before.UProperty.Size, after.UProperty.Size);
            PrintField("FField.ClassPrivate", before.FField.ClassPrivate, after.FField.ClassPrivate);
            PrintField("FField.Owner", before.FField.Owner, after.FField.Owner);
            PrintField("FField.Next", before.FField.Next, after.FField.Next);
            PrintField("FField.NamePrivate", before.FField.NamePrivate, after.FField.NamePrivate);
            PrintField("FField.FlagsPrivate", before.FField.FlagsPrivate, after.FField.FlagsPrivate);
            PrintField("FFieldClass.Name", before.FFieldClass.Name, after.FFieldClass.Name);
            PrintField("FFieldClass.SuperClass", before.FFieldClass.SuperClass, after.FFieldClass.SuperClass);
            PrintField("FFieldClass.CastFlags", before.FFieldClass.CastFlags, after.FFieldClass.CastFlags);
            PrintField("FProperty.ArrayDim", before.FProperty.ArrayDim, after.FProperty.ArrayDim);
            PrintField("FProperty.ElementSize", before.FProperty.ElementSize, after.FProperty.ElementSize);
            PrintField("FProperty.PropertyFlags", before.FProperty.PropertyFlags, after.FProperty.PropertyFlags);
            PrintField("FProperty.Offset_Internal", before.FProperty.Offset_Internal, after.FProperty.Offset_Internal);
            PrintField("FProperty.Size", before.FProperty.Size, after.FProperty.Size);
            PrintField("ULevel.Actors", before.ULevel.Actors, after.ULevel.Actors);
            PrintField("UDataTable.RowMap", before.UDataTable.RowMap, after.UDataTable.RowMap);
            LOGI("[AutoFix] ================================================");
        }

        void DumpObjectNameCandidates(int32_t idx, uintptr_t object)
        {
            const int32_t id18 = ReadI32(object + 0x18);
            const int32_t id20 = ReadI32(object + 0x20);
            const int32_t id28 = ReadI32(object + 0x28);

            const std::string n18 = ReadNameById(id18);
            const std::string n20 = ReadNameById(id20);
            const std::string n28 = ReadNameById(id28);

            LOGW("[AutoFix] Dump idx=%d obj=%p id@0x18=%d('%s') id@0x20=%d('%s') id@0x28=%d('%s')",
                 idx,
                 (void *)object,
                 id18,
                 n18.c_str(),
                 id20,
                 n20.c_str(),
                 id28,
                 n28.c_str());
        }

        bool ValidateUObjectLayout(uintptr_t nameOffset, int sampleCap, int *decodedOut = nullptr, int *totalOut = nullptr, int *classHitsOut = nullptr, int *scriptAnchorsOut = nullptr)
        {
            if (!Objects() || nameOffset == 0)
                return false;

            UE_Offsets *off = Offsets();
            if (!off)
                return false;

            const int32_t total = Objects()->GetNumElements();
            const int32_t limit = std::min(total, sampleCap);

            int decoded = 0;
            int scriptAnchors = 0;
            int classHits = 0;

            for (int32_t i = 0; i < limit; ++i)
            {
                const uintptr_t object = reinterpret_cast<uintptr_t>(Objects()->GetObjectPtr(i));
                if (!IsLikelyPtr(object))
                    continue;

                const std::string name = ReadObjectName(object, nameOffset);
                if (name.empty())
                    continue;

                if (IsUELikeName(name))
                    ++decoded;
                if (StartsWith(name, "/Script/"))
                    ++scriptAnchors;

                if (off->UObject.ClassPrivate)
                {
                    const uintptr_t classPtr = ReadPtr(object + off->UObject.ClassPrivate);
                    if (IsLikelyPtr(classPtr))
                    {
                        const std::string className = ReadObjectName(classPtr, nameOffset);
                        if (IsClassNameLike(className) || IsPropertyClassName(className))
                            ++classHits;
                    }
                }
            }

            if (decodedOut)
                *decodedOut = decoded;
            if (totalOut)
                *totalOut = limit;
            if (classHitsOut)
                *classHitsOut = classHits;
            if (scriptAnchorsOut)
                *scriptAnchorsOut = scriptAnchors;

            if (scriptAnchors >= 16)
                return true;

            if (!off->Config.IsUsingFNamePool && decoded >= 24 && classHits >= 24)
                return true;

            return false;
        }

        bool IsLikelyObjectFlagsValue(uint32_t flags)
        {
            constexpr uint32_t kKnownObjectFlagsMask = 0x0FFFFFFF;
            if (flags == 0 || (flags & ~kKnownObjectFlagsMask) != 0)
                return false;

            if ((flags & static_cast<uint32_t>(EObjectFlags::ClassDefaultObject)) != 0)
                return (flags & static_cast<uint32_t>(EObjectFlags::Public)) != 0;

            return true;
        }

        bool ValidateObjectFlagsOffset(uintptr_t candidate, int sampleCap, int *plausibleOut = nullptr, int *cdoHitsOut = nullptr)
        {
            if (!candidate || !Objects())
                return false;

            UE_Offsets *off = Offsets();
            const int32_t total = Objects()->GetNumElements();
            const int32_t limit = std::min(total, sampleCap);

            int plausible = 0;
            int defaultObjects = 0;
            int cdoHits = 0;
            for (int32_t i = 0; i < limit; ++i)
            {
                const uintptr_t object = reinterpret_cast<uintptr_t>(Objects()->GetObjectPtr(i));
                if (!IsLikelyPtr(object))
                    continue;

                const uint32_t flags = ReadU32(object + candidate);
                if (IsLikelyObjectFlagsValue(flags))
                    ++plausible;

                if (off && off->UObject.NamePrivate)
                {
                    const std::string name = ReadObjectName(object, off->UObject.NamePrivate);
                    if (StartsWith(name, "Default__"))
                    {
                        ++defaultObjects;
                        if ((flags & static_cast<uint32_t>(EObjectFlags::ClassDefaultObject)) != 0)
                            ++cdoHits;
                    }
                }
            }

            if (plausibleOut)
                *plausibleOut = plausible;
            if (cdoHitsOut)
                *cdoHitsOut = cdoHits;

            if (plausible < std::max(12, limit / 4))
                return false;

            return defaultObjects == 0 || cdoHits >= std::min(defaultObjects, 2);
        }

        uintptr_t FindCommonObjectFlagsOffset()
        {
            if (!Objects())
                return 0;

            constexpr uint32_t kCommonFlagsValue = 0x43;
            constexpr int kMinMatches = 0xA0;
            const int32_t total = Objects()->GetNumElements();
            const int32_t probeObjects = std::min(total, 0x20);
            const int32_t verifyObjects = std::min(total, 0x100);

            for (int32_t i = 0; i < probeObjects; ++i)
            {
                const uintptr_t object = reinterpret_cast<uintptr_t>(Objects()->GetObjectPtr(i));
                if (!IsLikelyPtr(object))
                    continue;

                for (uintptr_t candidate = sizeof(void *); candidate <= 0x40; candidate += sizeof(int32_t))
                {
                    if (ReadU32(object + candidate) != kCommonFlagsValue)
                        continue;

                    int matches = 0;
                    for (int32_t j = 0; j < verifyObjects; ++j)
                    {
                        const uintptr_t probe = reinterpret_cast<uintptr_t>(Objects()->GetObjectPtr(j));
                        if (!IsLikelyPtr(probe))
                            continue;
                        if (ReadU32(probe + candidate) == kCommonFlagsValue)
                            ++matches;
                    }

                    if (matches >= kMinMatches)
                        return candidate;
                }
            }

            return 0;
        }

        bool FixupObjectFlags()
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects())
                return false;

            const uintptr_t before = off->UObject.ObjectFlags;
            int plausible = 0;
            int cdoHits = 0;
            if (before && ValidateObjectFlagsOffset(before, kLooseSampleCap, &plausible, &cdoHits))
            {
                LOGI("[AutoFix] UObject.ObjectFlags=0x%lx validated (plausible=%d cdoHits=%d)",
                     (unsigned long)before,
                     plausible,
                     cdoHits);
                return true;
            }

            const uintptr_t commonCandidate = FindCommonObjectFlagsOffset();
            if (commonCandidate && ValidateObjectFlagsOffset(commonCandidate, kLooseSampleCap, &plausible, &cdoHits))
            {
                LOGI("[AutoFix] Rescanned UObject.ObjectFlags=0x%lx (was 0x%lx) from common 0x43 flags sample",
                     (unsigned long)commonCandidate,
                     (unsigned long)before);
                off->UObject.ObjectFlags = commonCandidate;
                return true;
            }

            uintptr_t bestCandidate = 0;
            int bestScore = -1;
            for (uintptr_t candidate = sizeof(void *); candidate <= 0x40; candidate += sizeof(int32_t))
            {
                if (candidate == off->UObject.NamePrivate || candidate == off->UObject.ClassPrivate ||
                    candidate == off->UObject.OuterPrivate || candidate == off->UObject.InternalIndex)
                {
                    continue;
                }

                int candidatePlausible = 0;
                int candidateCdoHits = 0;
                if (!ValidateObjectFlagsOffset(candidate, kLooseSampleCap, &candidatePlausible, &candidateCdoHits))
                    continue;

                const int score = candidatePlausible + (candidateCdoHits * 8);
                if (score > bestScore)
                {
                    bestScore = score;
                    bestCandidate = candidate;
                }
            }

            if (bestCandidate)
            {
                LOGI("[AutoFix] Rescanned UObject.ObjectFlags=0x%lx (was 0x%lx) from generic flags scan",
                     (unsigned long)bestCandidate,
                     (unsigned long)before);
                off->UObject.ObjectFlags = bestCandidate;
                return true;
            }

            LOGI("[AutoFix] UObject.ObjectFlags not confirmed by scan; keeping preset");
            return false;
        }

        bool ValidateInternalIndexOffset(uintptr_t candidate, int sampleCap, int *hitsOut = nullptr, int *checkedOut = nullptr)
        {
            if (!candidate || !Objects())
                return false;

            const int32_t total = Objects()->GetNumElements();
            const int32_t limit = std::min(total, sampleCap);
            int checked = 0;
            int hits = 0;

            for (int32_t i = 0; i < limit; ++i)
            {
                const uintptr_t object = reinterpret_cast<uintptr_t>(Objects()->GetObjectPtr(i));
                if (!IsLikelyPtr(object))
                    continue;

                ++checked;
                if (ReadI32(object + candidate) == i)
                    ++hits;
            }

            if (hitsOut)
                *hitsOut = hits;
            if (checkedOut)
                *checkedOut = checked;

            return checked >= 16 && hits >= std::max(16, (checked * 3) / 4);
        }

        bool FixupInternalIndex()
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects())
                return false;

            const uintptr_t before = off->UObject.InternalIndex;
            int hits = 0;
            int checked = 0;
            if (before && ValidateInternalIndexOffset(before, 0x100, &hits, &checked))
            {
                LOGI("[AutoFix] UObject.InternalIndex=0x%lx validated (hits=%d/%d)",
                     (unsigned long)before,
                     hits,
                     checked);
                return true;
            }

            uintptr_t bestCandidate = 0;
            int bestHits = 0;
            int bestChecked = 0;
            for (uintptr_t candidate = sizeof(void *); candidate <= 0x40; candidate += sizeof(int32_t))
            {
                if (candidate == off->UObject.NamePrivate || candidate == off->UObject.ClassPrivate ||
                    candidate == off->UObject.OuterPrivate || candidate == off->UObject.ObjectFlags)
                {
                    continue;
                }

                int candidateHits = 0;
                int candidateChecked = 0;
                if (!ValidateInternalIndexOffset(candidate, 0x100, &candidateHits, &candidateChecked))
                    continue;

                if (candidateHits > bestHits)
                {
                    bestCandidate = candidate;
                    bestHits = candidateHits;
                    bestChecked = candidateChecked;
                }
            }

            if (bestCandidate)
            {
                LOGI("[AutoFix] Rescanned UObject.InternalIndex=0x%lx (was 0x%lx) hits=%d/%d",
                     (unsigned long)bestCandidate,
                     (unsigned long)before,
                     bestHits,
                     bestChecked);
                off->UObject.InternalIndex = bestCandidate;
                return true;
            }

            LOGI("[AutoFix] UObject.InternalIndex not confirmed by scan; keeping preset");
            return false;
        }

        bool FixupNamePrivate()
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects())
                return false;

            const uintptr_t before = off->UObject.NamePrivate;
            if (before)
            {
                int decoded = 0;
                int total = 0;
                int classHits = 0;
                int scriptAnchors = 0;
                if (ValidateUObjectLayout(before, kLooseSampleCap, &decoded, &total, &classHits, &scriptAnchors))
                {
                    LOGI("[AutoFix] UObject.NamePrivate=0x%lx validated (decoded=%d classHits=%d scriptAnchors=%d)",
                         (unsigned long)before,
                         decoded,
                         classHits,
                         scriptAnchors);
                    return true;
                }

                LOGW("[AutoFix] Preset NamePrivate=0x%lx not validated (decoded=%d classHits=%d scriptAnchors=%d)",
                     (unsigned long)before,
                     decoded,
                     classHits,
                     scriptAnchors);
            }

            const int32_t total = Objects()->GetNumElements();
            const int32_t strictLimit = std::min(total, kStrictSampleCap);

            for (int32_t i = 0; i < strictLimit; ++i)
            {
                const uintptr_t object = reinterpret_cast<uintptr_t>(Objects()->GetObjectPtr(i));
                if (!IsLikelyPtr(object))
                    continue;

                for (uintptr_t candidate = kNameScanStart; candidate <= kNameScanEnd; candidate += 0x4)
                {
                    if (ReadObjectName(object, candidate) == "/Script/CoreUObject")
                    {
                        LOGI("[AutoFix] BruteForce NamePrivate=0x%lx (was 0x%lx) anchor='/Script/CoreUObject' @ idx=%d",
                             (unsigned long)candidate,
                             (unsigned long)before,
                             i);
                        off->UObject.NamePrivate = candidate;
                        return true;
                    }
                }
            }

            uintptr_t bestCandidate = 0;
            int bestScore = -1;
            int bestIndex = -1;
            std::string bestAnchor;

            const int32_t looseLimit = std::min(total, kLooseSampleCap);
            for (int32_t i = 0; i < looseLimit; ++i)
            {
                const uintptr_t object = reinterpret_cast<uintptr_t>(Objects()->GetObjectPtr(i));
                if (!IsLikelyPtr(object))
                    continue;

                for (uintptr_t candidate = kNameScanStart; candidate <= kNameScanEnd; candidate += 0x4)
                {
                    const std::string name = ReadObjectName(object, candidate);
                    if (!StartsWith(name, "/Script/"))
                        continue;

                    int decoded = 0;
                    int totalChecked = 0;
                    int classHits = 0;
                    int scriptAnchors = 0;
                    if (!ValidateUObjectLayout(candidate, kLooseSampleCap, &decoded, &totalChecked, &classHits, &scriptAnchors))
                        continue;

                    const int score = decoded + (classHits * 3) + (scriptAnchors * 8);
                    if (score > bestScore)
                    {
                        bestScore = score;
                        bestCandidate = candidate;
                        bestIndex = i;
                        bestAnchor = name;
                    }
                }
            }

            if (bestCandidate)
            {
                LOGI("[AutoFix] Rescanned NamePrivate=0x%lx (was 0x%lx) anchor='%s' @ idx=%d",
                     (unsigned long)bestCandidate,
                     (unsigned long)before,
                     bestAnchor.c_str(),
                     bestIndex);
                off->UObject.NamePrivate = bestCandidate;
                return true;
            }

            const uintptr_t dumpObject = reinterpret_cast<uintptr_t>(Objects()->GetObjectPtr(0));
            if (IsLikelyPtr(dumpObject))
                DumpObjectNameCandidates(0, dumpObject);

            LOGE("[AutoFix] UObject.NamePrivate fixup failed; keeping preset");
            return false;
        }

        bool FixupFNameSettings()
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects() || !off->UObject.NamePrivate)
                return false;

            const uintptr_t beforeSize = off->FName.Size;
            const uintptr_t beforeNumber = off->FName.Number;
            const uintptr_t beforeDisplay = off->FName.DisplayIndex;

            const bool nameBeforeClass = off->UObject.ClassPrivate && off->UObject.NamePrivate < off->UObject.ClassPrivate;
            const uintptr_t inferredSize = !nameBeforeClass && off->UObject.OuterPrivate > off->UObject.NamePrivate
                ? (off->UObject.OuterPrivate - off->UObject.NamePrivate)
                : (off->UObject.ClassPrivate > off->UObject.NamePrivate ? (off->UObject.ClassPrivate - off->UObject.NamePrivate) : 0);

            const uintptr_t firstObject = reinterpret_cast<uintptr_t>(Objects()->GetObjectPtr(0));
            if (!IsLikelyPtr(firstObject))
                return false;

            const int32_t firstInt = ReadI32(firstObject + off->UObject.NamePrivate);
            const int32_t secondInt = ReadI32(firstObject + off->UObject.NamePrivate + sizeof(int32_t));

            int lowNumberHits = 0;
            const int32_t total = std::min(Objects()->GetNumElements(), 0x200);
            for (int32_t i = 0; i < total; ++i)
            {
                const uintptr_t object = reinterpret_cast<uintptr_t>(Objects()->GetObjectPtr(i));
                if (!IsLikelyPtr(object))
                    continue;

                const int32_t value = ReadI32(object + off->UObject.NamePrivate + sizeof(int32_t));
                if (value > 0 && value < 5)
                    ++lowNumberHits;
            }

            bool changed = false;
            if (inferredSize == 0x8 && firstInt == secondInt)
            {
                off->Config.isUsingCasePreservingName = true;
                off->Config.isUsingOutlineNumberName = true;
                off->FName.Size = 0x8;
                off->FName.ComparisonIndex = 0x0;
                off->FName.DisplayIndex = 0x4;
                off->FName.Number = 0x0;
                changed = true;
            }
            else if (inferredSize == 0xC)
            {
                off->Config.isUsingCasePreservingName = true;
                off->Config.isUsingOutlineNumberName = false;
                off->FName.Size = 0xC;
                off->FName.ComparisonIndex = 0x0;
                off->FName.Number = (firstInt == secondInt) ? 0x8 : 0x4;
                off->FName.DisplayIndex = (off->FName.Number == 0x4) ? 0x8 : 0x4;
                changed = true;
            }
            else if (inferredSize == 0x4 || lowNumberHits < std::max(4, total / 50))
            {
                off->Config.isUsingCasePreservingName = false;
                off->Config.isUsingOutlineNumberName = true;
                off->FName.Size = 0x4;
                off->FName.ComparisonIndex = 0x0;
                off->FName.DisplayIndex = 0x0;
                off->FName.Number = 0x0;
                changed = true;
            }
            else if (inferredSize == 0x8)
            {
                off->Config.isUsingCasePreservingName = false;
                off->Config.isUsingOutlineNumberName = false;
                off->FName.Size = 0x8;
                off->FName.ComparisonIndex = 0x0;
                off->FName.DisplayIndex = 0x0;
                off->FName.Number = 0x4;
                changed = true;
            }

            if (!changed)
            {
                LOGI("[AutoFix] FName settings not changed; keeping preset");
                return false;
            }

            if (beforeSize == off->FName.Size && beforeNumber == off->FName.Number && beforeDisplay == off->FName.DisplayIndex)
            {
                LOGI("[AutoFix] FName settings validated: Size=0x%lx Number=0x%lx DisplayIndex=0x%lx",
                     (unsigned long)off->FName.Size,
                     (unsigned long)off->FName.Number,
                     (unsigned long)off->FName.DisplayIndex);
                return true;
            }

            LOGI("[AutoFix] FName settings updated: Size=0x%lx (was 0x%lx) Number=0x%lx (was 0x%lx) DisplayIndex=0x%lx (was 0x%lx)",
                 (unsigned long)off->FName.Size,
                 (unsigned long)beforeSize,
                 (unsigned long)off->FName.Number,
                 (unsigned long)beforeNumber,
                 (unsigned long)off->FName.DisplayIndex,
                 (unsigned long)beforeDisplay);
            return true;
        }

        void ApplyFNameDependentFieldFixups()
        {
            UE_Offsets *off = Offsets();
            if (!off || !off->Config.IsUsingFNamePool)
                return;

            if (!off->FField.Owner && off->FField.ClassPrivate)
                off->FField.Owner = off->FField.ClassPrivate + sizeof(void *);

            if (off->FField.Next)
            {
                const uintptr_t derivedName = off->FField.Next + sizeof(void *);
                if (!off->FField.NamePrivate || off->FField.NamePrivate < off->FField.Next || off->FField.NamePrivate > derivedName + 0x20)
                    off->FField.NamePrivate = derivedName;

                const uintptr_t derivedFlags = off->FField.NamePrivate + off->FName.Size;
                if (!off->FField.FlagsPrivate || off->FField.FlagsPrivate != derivedFlags)
                    off->FField.FlagsPrivate = derivedFlags;
            }

            if (!off->FFieldClass.Name && off->Config.IsUsingFNamePool)
                off->FFieldClass.Name = 0;
        }

        void CollectSampleFFieldClassPtrs(std::vector<uintptr_t> &out)
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects() || !off->Config.IsUsingFNamePool || !off->UStruct.ChildProperties || !off->FField.ClassPrivate)
                return;

            const UE_UClass classClass = UE_UClass::StaticClass();
            if (!classClass)
                return;

            int scannedClasses = 0;
            Objects()->ForEachObjectOfClass(classClass, [&](UE_UObject object)
            {
                if (scannedClasses >= 128 || out.size() >= 24)
                    return true;
                ++scannedClasses;

                int guard = 0;
                for (UE_FField field = object.Cast<UE_UClass>().GetChildProperties(); field && guard < 96; field = field.GetNext(), ++guard)
                {
                    const uintptr_t fieldClass = reinterpret_cast<uintptr_t>(field.GetClass().GetAddress());
                    if (!IsLikelyPtr(fieldClass))
                        continue;
                    if (std::find(out.begin(), out.end(), fieldClass) != out.end())
                        continue;
                    out.push_back(fieldClass);
                    if (out.size() >= 24)
                        return true;
                }
                return false;
            });
        }

        bool ValidateFFieldClassNameOffset(uintptr_t candidate)
        {
            std::vector<uintptr_t> samples;
            CollectSampleFFieldClassPtrs(samples);
            if (samples.size() < 4)
                return false;

            int valid = 0;
            int propertyLike = 0;
            for (uintptr_t fieldClass : samples)
            {
                const std::string name = ReadNameAt(fieldClass + candidate);
                if (!IsIdentifierLike(name))
                    continue;
                ++valid;
                if (IsFFieldClassNameLike(name))
                    ++propertyLike;
            }

            return valid >= 4 && propertyLike >= std::max(3, valid * 2 / 3);
        }

        bool FixupFFieldClassName()
        {
            UE_Offsets *off = Offsets();
            if (!off || !off->Config.IsUsingFNamePool)
                return false;

            const uintptr_t before = off->FFieldClass.Name;
            if (ValidateFFieldClassNameOffset(before))
            {
                LOGI("[AutoFix] FFieldClass.Name=0x%lx validated", (unsigned long)before);
                return true;
            }

            for (uintptr_t candidate = 0; candidate <= 0x18; candidate += sizeof(int32_t))
            {
                if (!ValidateFFieldClassNameOffset(candidate))
                    continue;

                LOGI("[AutoFix] Rescanned FFieldClass.Name=0x%lx (was 0x%lx)",
                     (unsigned long)candidate,
                     (unsigned long)before);
                off->FFieldClass.Name = candidate;
                return true;
            }

            LOGI("[AutoFix] FFieldClass.Name not confirmed by scan; keeping preset");
            return false;
        }

        bool FollowSuperChain(uintptr_t start, uintptr_t superOffset, uintptr_t nameOffset, std::string *endName, int *depthOut)
        {
            if (!start || !superOffset || !nameOffset)
                return false;

            uintptr_t current = start;
            int depth = 0;
            std::string lastName;

            while (current && depth < 32)
            {
                current = ReadPtr(current + superOffset);
                if (!IsLikelyPtr(current))
                    break;

                lastName = ReadObjectName(current, nameOffset);
                if (lastName == "Object")
                {
                    if (endName)
                        *endName = lastName;
                    if (depthOut)
                        *depthOut = depth + 1;
                    return true;
                }
                ++depth;
            }

            if (endName)
                *endName = lastName;
            if (depthOut)
                *depthOut = depth;
            return false;
        }

        bool FixupSuperStruct()
        {
            UE_Offsets *off = Offsets();
            if (!off || off->UObject.NamePrivate == 0 || !Objects())
                return false;

            const uintptr_t before = off->UStruct.SuperStruct;
            const UE_UClass worldClass = Objects()->FindObject("Class Engine.World").Cast<UE_UClass>();
            if (!worldClass)
            {
                LOGW("[AutoFix] UStruct anchor 'World' class not found");
                return false;
            }

            const uintptr_t worldAddr = reinterpret_cast<uintptr_t>(worldClass.GetAddress());
            std::string endName;
            int depth = 0;

            if (before && FollowSuperChain(worldAddr, before, off->UObject.NamePrivate, &endName, &depth))
            {
                LOGI("[AutoFix] UStruct.SuperStruct=0x%lx validated (chain reached '%s' depth=%d)",
                     (unsigned long)before,
                     endName.c_str(),
                     depth);
                return true;
            }

            if (before)
            {
                LOGW("[AutoFix] UStruct.SuperStruct=0x%lx: chain didn't reach Object", (unsigned long)before);
            }

            for (uintptr_t candidate = 0x0; candidate <= 0x100; candidate += 0x4)
            {
                if (!FollowSuperChain(worldAddr, candidate, off->UObject.NamePrivate, &endName, &depth))
                    continue;

                LOGI("[AutoFix] Rescanned UStruct.SuperStruct=0x%lx (was 0x%lx) chain end='%s'",
                     (unsigned long)candidate,
                     (unsigned long)before,
                     endName.c_str());
                off->UStruct.SuperStruct = candidate;
                return true;
            }

            LOGW("[AutoFix] UStruct.SuperStruct fixup failed; keeping preset");
            return false;
        }

        bool FixupUFunctionFunc()
        {
            UE_Offsets *off = Offsets();
            if (!off || off->UObject.NamePrivate == 0 || !Objects())
                return false;

            const uintptr_t before = off->UFunction.Func;
            const UE_UClass functionClass = Objects()->FindObject("Class CoreUObject.Function").Cast<UE_UClass>();
            if (!functionClass)
            {
                LOGW("[AutoFix] UFunction.Func fixup failed; keeping preset");
                return false;
            }

            auto validateOffset = [&](uintptr_t candidate, bool logOk) -> bool
            {
                bool ok = false;
                Objects()->ForEachObjectOfClass(functionClass, [&](UE_UObject object)
                {
                    const uintptr_t address = reinterpret_cast<uintptr_t>(object.GetAddress());
                    const uintptr_t func = ReadPtr(address + candidate);
                    if (!IsExecutableInModule(func))
                        return false;

                    if (logOk)
                    {
                        LOGI("[AutoFix] UFunction.Func=0x%lx validated ('%s'.Func=%p)",
                             (unsigned long)candidate,
                             object.GetName().c_str(),
                             (void *)func);
                    }
                    ok = true;
                    return true;
                });
                return ok;
            };

            if (before && validateOffset(before, true))
                return true;

            int probed = 0;
            for (uintptr_t candidate = 0x80; candidate <= 0x140; candidate += 0x8)
            {
                ++probed;
                if (!validateOffset(candidate, false))
                    continue;

                std::string ownerName;
                Objects()->ForEachObjectOfClass(functionClass, [&](UE_UObject object)
                {
                    const uintptr_t address = reinterpret_cast<uintptr_t>(object.GetAddress());
                    const uintptr_t func = ReadPtr(address + candidate);
                    if (!IsExecutableInModule(func))
                        return false;
                    ownerName = object.GetName();
                    return true;
                });

                LOGI("[AutoFix] Rescanned UFunction.Func=0x%lx (was 0x%lx) on '%s'",
                     (unsigned long)candidate,
                     (unsigned long)before,
                     ownerName.c_str());
                off->UFunction.Func = candidate;
                return true;
            }

            LOGW("[AutoFix] UFunction.Func=0x%lx: no probed function had executable Func (probed=%d)",
                 (unsigned long)before,
                 probed);
            LOGW("[AutoFix] UFunction.Func fixup failed; keeping preset");
            return false;
        }

        bool ValidateLevelActorsOffset(uintptr_t candidate)
        {
            if (!candidate || !Objects())
                return false;

            const UE_UClass levelClass = FindClassByFullName("Class Engine.Level");
            if (!levelClass)
                return false;

            uintptr_t sampleLevel = 0;
            Objects()->ForEachObjectOfClass(levelClass, [&](UE_UObject object)
            {
                if (object && !object.HasFlags(EObjectFlags::ClassDefaultObject))
                {
                    sampleLevel = reinterpret_cast<uintptr_t>(object.GetAddress());
                    return true;
                }
                return false;
            });
            if (!sampleLevel)
                return false;

            const uintptr_t dataPtr = ReadPtr(sampleLevel + candidate);
            const int32_t count = ReadI32(sampleLevel + candidate + sizeof(void *));
            const int32_t maxCount = ReadI32(sampleLevel + candidate + sizeof(void *) + sizeof(int32_t));
            if (!IsLikelyPtr(dataPtr) || !IsPlausibleArrayCount(count, maxCount, 0x8000) || count <= 0)
                return false;

            int actorHits = 0;
            const int32_t probeCount = std::min(count, 6);
            for (int32_t i = 0; i < probeCount; ++i)
            {
                const uintptr_t actor = ReadPtr(dataPtr + (i * sizeof(void *)));
                if (!actor)
                    continue;
                if (IsObjectAByClassName(actor, "Actor"))
                    ++actorHits;
            }

            return actorHits >= 1;
        }

        bool FixupLevelActors()
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects())
                return false;

            const uintptr_t before = off->ULevel.Actors;
            if (before && ValidateLevelActorsOffset(before))
            {
                LOGI("[AutoFix] ULevel.Actors=0x%lx validated", (unsigned long)before);
                return true;
            }

            for (uintptr_t candidate = 0x40; candidate <= 0x180; candidate += sizeof(void *))
            {
                if (!ValidateLevelActorsOffset(candidate))
                    continue;
                LOGI("[AutoFix] Rescanned ULevel.Actors=0x%lx (was 0x%lx)",
                     (unsigned long)candidate,
                     (unsigned long)before);
                off->ULevel.Actors = candidate;
                return true;
            }

            LOGI("[AutoFix] ULevel.Actors not confirmed by scan; keeping preset");
            return false;
        }

        bool TryDeriveDataTableRowMapOffset(uintptr_t &outOffset)
        {
            const UE_UClass dataTableClass = FindClassByFullName("Class Engine.DataTable");
            if (!dataTableClass)
                return false;

            if (const UE_FField field = dataTableClass.FindChildProp("RowStruct"))
            {
                const UE_FProperty prop = field.Cast<UE_FProperty>();
                const int32_t fieldOffset = prop.GetOffset();
                const int32_t fieldSize = prop.GetSize();
                if (fieldOffset >= 0 && fieldSize > 0)
                {
                    outOffset = static_cast<uintptr_t>(fieldOffset + fieldSize);
                    return true;
                }
            }

            if (const UE_UField field = dataTableClass.FindChild("RowStruct"))
            {
                const UE_UProperty prop = field.Cast<UE_UProperty>();
                const int32_t fieldOffset = prop.GetOffset();
                const int32_t fieldSize = prop.GetSize();
                if (fieldOffset >= 0 && fieldSize > 0)
                {
                    outOffset = static_cast<uintptr_t>(fieldOffset + fieldSize);
                    return true;
                }
            }

            return false;
        }

        bool FixupDataTableRowMap()
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects())
                return false;

            uintptr_t derived = 0;
            if (!TryDeriveDataTableRowMapOffset(derived))
            {
                LOGI("[AutoFix] UDataTable.RowMap skipped; RowStruct anchor not found");
                return false;
            }

            if (off->UDataTable.RowMap == derived)
            {
                LOGI("[AutoFix] UDataTable.RowMap=0x%lx validated from RowStruct tail", (unsigned long)derived);
                return true;
            }

            LOGI("[AutoFix] Derived UDataTable.RowMap=0x%lx (was 0x%lx) from RowStruct tail",
                 (unsigned long)derived,
                 (unsigned long)off->UDataTable.RowMap);
            off->UDataTable.RowMap = derived;
            return true;
        }

        // ----- UObject.ClassPrivate fixup ------------------------------------------------
        // 验证：ClassPrivate 指向的对象自身是 "Class" 类（自指环），并且其 NamePrivate
        // 解出的名字以大写字母开头。沿用 dumper-7 中通过 "Class" 锚定 UObjectToClass 的
        // 思路（见 main.cpp DumpOffset_ 的 Engine 入口），但用更稳健的双重链路验证。
        bool ValidateClassPrivate(uintptr_t candidate, int sampleCap)
        {
            if (!candidate || !Objects())
                return false;
            UE_Offsets *off = Offsets();
            const uintptr_t nameOff = off->UObject.NamePrivate;
            if (!nameOff)
                return false;

            const int32_t total = Objects()->GetNumElements();
            const int32_t limit = std::min(total, sampleCap);

            int hits = 0;
            int strictHits = 0;
            for (int32_t i = 0; i < limit; ++i)
            {
                const uintptr_t object = reinterpret_cast<uintptr_t>(Objects()->GetObjectPtr(i));
                if (!IsLikelyPtr(object))
                    continue;
                const uintptr_t classPtr = ReadPtr(object + candidate);
                if (!IsLikelyPtr(classPtr) || classPtr == object)
                    continue;
                const std::string className = ReadObjectName(classPtr, nameOff);
                if (!IsClassNameLike(className) && !IsPropertyClassName(className))
                    continue;

                const uintptr_t metaClass = ReadPtr(classPtr + candidate);
                if (IsLikelyPtr(metaClass))
                {
                    const std::string metaClassName = ReadObjectName(metaClass, nameOff);
                    if (metaClassName == "Class")
                        ++strictHits;
                }
                ++hits;
            }

            if (strictHits >= 4)
                return true;

            if (off->Config.IsUsingFNamePool)
                return hits >= std::max(8, limit / 4);

            return hits >= 8;
        }

        bool FixupClassPrivate()
        {
            UE_Offsets *off = Offsets();
            if (!off || off->UObject.NamePrivate == 0 || !Objects())
                return false;

            const uintptr_t before = off->UObject.ClassPrivate;
            if (before && ValidateClassPrivate(before, kStrictSampleCap))
            {
                LOGI("[AutoFix] UObject.ClassPrivate=0x%lx validated", (unsigned long)before);
                return true;
            }

            for (uintptr_t candidate = 0x8; candidate <= 0x40; candidate += 0x4)
            {
                if (candidate == off->UObject.NamePrivate)
                    continue;
                if (!ValidateClassPrivate(candidate, kStrictSampleCap))
                    continue;
                LOGI("[AutoFix] Rescanned UObject.ClassPrivate=0x%lx (was 0x%lx)",
                     (unsigned long)candidate, (unsigned long)before);
                off->UObject.ClassPrivate = candidate;
                return true;
            }

            LOGI("[AutoFix] UObject.ClassPrivate not confirmed by scan; keeping preset");
            return false;
        }

        // ----- UObject.OuterPrivate fixup ------------------------------------------------
        // 验证：跟随 OuterPrivate 直到 nullptr，链尾对象的名字应当是包名形式 "/Script/*"
        // 或全局根。dumper-7 中通过 "/Script/Engine" 锚定 UObjectToOuter，这里改为对样本
        // 集做"链路终点 /Script/* 命中数"统计。
        bool ValidateOuterPrivate(uintptr_t candidate, int sampleCap)
        {
            if (!candidate || !Objects())
                return false;
            UE_Offsets *off = Offsets();
            const uintptr_t nameOff = off->UObject.NamePrivate;
            if (!nameOff)
                return false;

            const int32_t total = Objects()->GetNumElements();
            const int32_t limit = std::min(total, sampleCap);

            int packageHits = 0;
            int readableHits = 0;
            for (int32_t i = 0; i < limit; ++i)
            {
                uintptr_t cur = reinterpret_cast<uintptr_t>(Objects()->GetObjectPtr(i));
                if (!IsLikelyPtr(cur))
                    continue;

                std::string lastName;
                int depth = 0;
                while (depth < 16)
                {
                    const uintptr_t outer = ReadPtr(cur + candidate);
                    if (!IsLikelyPtr(outer))
                        break;
                    ++readableHits;
                    cur = outer;
                    lastName = ReadObjectName(cur, nameOff);
                    ++depth;
                }
                if (StartsWith(lastName, "/Script/") || StartsWith(lastName, "/Game/"))
                    ++packageHits;
                else if (!off->Config.IsUsingFNamePool && (IsUELikeName(lastName) || IsClassNameLike(lastName)))
                    ++packageHits;
            }

            if (off->Config.IsUsingFNamePool)
                return packageHits >= std::max(4, limit / 4);

            return packageHits >= 4 || readableHits >= 16;
        }

        bool FixupOuterPrivate()
        {
            UE_Offsets *off = Offsets();
            if (!off || off->UObject.NamePrivate == 0 || !Objects())
                return false;

            const uintptr_t before = off->UObject.OuterPrivate;
            if (before && ValidateOuterPrivate(before, kStrictSampleCap))
            {
                LOGI("[AutoFix] UObject.OuterPrivate=0x%lx validated", (unsigned long)before);
                return true;
            }

            for (uintptr_t candidate = 0x10; candidate <= 0x50; candidate += 0x4)
            {
                if (candidate == off->UObject.NamePrivate || candidate == off->UObject.ClassPrivate)
                    continue;
                if (!ValidateOuterPrivate(candidate, kStrictSampleCap))
                    continue;
                LOGI("[AutoFix] Rescanned UObject.OuterPrivate=0x%lx (was 0x%lx)",
                     (unsigned long)candidate, (unsigned long)before);
                off->UObject.OuterPrivate = candidate;
                return true;
            }

            LOGI("[AutoFix] UObject.OuterPrivate not confirmed by scan; keeping preset");
            return false;
        }

        bool ValidateStructSizeOffset(uintptr_t candidate)
        {
            if (!candidate || !Objects())
                return false;

            int valid = 0;
            int matches = 0;
            auto checkAnchor = [&](const char *name, int32_t expected)
            {
                const UE_UStruct strct = FindStructByName(name);
                if (!strct)
                    return;
                ++valid;
                if (ReadI32(reinterpret_cast<uintptr_t>(strct.GetAddress()) + candidate) == expected)
                    ++matches;
            };

            checkAnchor("Color", 0x04);
            checkAnchor("Guid", 0x10);
            checkAnchor("Vector2D", 0x08);
            return valid >= 2 && matches == valid;
        }

        bool FixupStructSize()
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects())
                return false;

            const uintptr_t before = off->UStruct.PropertiesSize;
            if (before && ValidateStructSizeOffset(before))
            {
                LOGI("[AutoFix] UStruct.PropertiesSize=0x%lx validated as struct-size field", (unsigned long)before);
                return true;
            }

            for (uintptr_t candidate = off->UStruct.Children ? off->UStruct.Children + sizeof(void *) : 0x20;
                 candidate <= 0x80;
                 candidate += sizeof(int32_t))
            {
                if (!ValidateStructSizeOffset(candidate))
                    continue;
                LOGI("[AutoFix] Rescanned UStruct.PropertiesSize=0x%lx (was 0x%lx)",
                     (unsigned long)candidate,
                     (unsigned long)before);
                off->UStruct.PropertiesSize = candidate;
                return true;
            }

            LOGI("[AutoFix] UStruct.PropertiesSize not confirmed by scan; keeping preset");
            return false;
        }

        bool ValidateMinAlignmentOffset(uintptr_t candidate)
        {
            if (!candidate || !Objects())
                return false;

            int valid = 0;
            int matches = 0;
            auto checkAnchor = [&](const char *name, int32_t expected)
            {
                UE_UObject object = Objects()->FindObjectFast(name);
                if (!object)
                    return;
                ++valid;
                const uintptr_t address = reinterpret_cast<uintptr_t>(object.GetAddress());
                const int32_t value32 = ReadI32(address + candidate);
                const int16_t value16 = ReadI16(address + candidate);
                if (value32 == expected || value16 == expected)
                    ++matches;
            };

            checkAnchor("Transform", 0x10);
            if constexpr (sizeof(void *) == 4)
                checkAnchor("InterpCurveLinearColor", 0x04);
            else
                checkAnchor("PlayerController", 0x08);

            return valid >= 2 && matches == valid;
        }

        bool FixupMinAlignment()
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects())
                return false;

            const uintptr_t before = off->UStruct.MinAlignment;
            if (before && ValidateMinAlignmentOffset(before))
            {
                LOGI("[AutoFix] UStruct.MinAlignment=0x%lx validated", (unsigned long)before);
                return true;
            }

            const uintptr_t derived = off->UStruct.PropertiesSize ? (off->UStruct.PropertiesSize + sizeof(int32_t)) : 0;
            if (derived && ValidateMinAlignmentOffset(derived))
            {
                LOGI("[AutoFix] Derived UStruct.MinAlignment=0x%lx (was 0x%lx)",
                     (unsigned long)derived,
                     (unsigned long)before);
                off->UStruct.MinAlignment = derived;
                return true;
            }

            for (uintptr_t candidate = off->UStruct.PropertiesSize ? off->UStruct.PropertiesSize : 0x20;
                 candidate <= 0x90;
                 candidate += sizeof(int32_t))
            {
                if (!ValidateMinAlignmentOffset(candidate))
                    continue;
                LOGI("[AutoFix] Rescanned UStruct.MinAlignment=0x%lx (was 0x%lx)",
                     (unsigned long)candidate,
                     (unsigned long)before);
                off->UStruct.MinAlignment = candidate;
                return true;
            }

            LOGI("[AutoFix] UStruct.MinAlignment not confirmed by scan; keeping preset");
            return false;
        }

        bool ValidateClassDefaultObjectOffset(uintptr_t candidate)
        {
            if (!candidate || !Objects())
                return false;

            int valid = 0;
            int matches = 0;
            auto checkAnchor = [&](const char *classFullName, const char *defaultName)
            {
                const UE_UClass cls = FindClassByFullName(classFullName);
                const UE_UObject cdo = Objects()->FindObjectFast(defaultName);
                if (!cls || !cdo)
                    return;
                ++valid;
                if (ReadPtr(reinterpret_cast<uintptr_t>(cls.GetAddress()) + candidate) == reinterpret_cast<uintptr_t>(cdo.GetAddress()))
                    ++matches;
            };

            checkAnchor("Class CoreUObject.Object", "Default__Object");
            checkAnchor("Class CoreUObject.Field", "Default__Field");
            checkAnchor("Class Engine.Actor", "Default__Actor");
            return valid >= 2 && matches >= 2 && matches == valid;
        }

        bool FixupClassDefaultObject()
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects())
                return false;

            const uintptr_t before = off->UClass.ClassDefaultObject;
            if (before && ValidateClassDefaultObjectOffset(before))
            {
                LOGI("[AutoFix] UClass.ClassDefaultObject=0x%lx validated", (unsigned long)before);
                return true;
            }

            for (uintptr_t candidate = 0x28; candidate <= 0x200; candidate += sizeof(void *))
            {
                if (!ValidateClassDefaultObjectOffset(candidate))
                    continue;
                LOGI("[AutoFix] Rescanned UClass.ClassDefaultObject=0x%lx (was 0x%lx)",
                     (unsigned long)candidate,
                     (unsigned long)before);
                off->UClass.ClassDefaultObject = candidate;
                return true;
            }

            LOGI("[AutoFix] UClass.ClassDefaultObject not confirmed by scan; keeping preset");
            return false;
        }

        bool ValidateImplementedInterfacesOffset(uintptr_t candidate)
        {
            UE_Offsets *off = Offsets();
            if (!candidate || !Objects() || !off || !off->UObject.NamePrivate)
                return false;

            const UE_UClass actorComponent = FindClassByFullName("Class Engine.ActorComponent");
            const UE_UClass interfaceClass = FindClassByName("Interface_AssetUserData");
            if (!actorComponent || !interfaceClass)
                return false;

            const uintptr_t address = reinterpret_cast<uintptr_t>(actorComponent.GetAddress());
            const uintptr_t dataPtr = ReadPtr(address + candidate);
            const int32_t count = ReadI32(address + candidate + sizeof(void *));
            const int32_t maxCount = ReadI32(address + candidate + sizeof(void *) + sizeof(int32_t));
            if (!IsLikelyPtr(dataPtr) || !IsPlausibleArrayCount(count, maxCount, 0x40) || count <= 0)
                return false;

            const uintptr_t target = reinterpret_cast<uintptr_t>(interfaceClass.GetAddress());
            const uintptr_t entrySizes[] = {sizeof(void *) == 8 ? 0x10 : 0x0C, sizeof(void *) * 2};
            for (uintptr_t entrySize : entrySizes)
            {
                const int32_t probeCount = std::min(count, 4);
                for (int32_t i = 0; i < probeCount; ++i)
                {
                    if (ReadPtr(dataPtr + (entrySize * i)) == target)
                        return true;
                }
            }

            return false;
        }

        bool FixupImplementedInterfaces()
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects())
                return false;

            const uintptr_t before = off->UClass.ImplementedInterfaces;
            if (before && ValidateImplementedInterfacesOffset(before))
            {
                LOGI("[AutoFix] UClass.ImplementedInterfaces=0x%lx validated", (unsigned long)before);
                return true;
            }

            const uintptr_t start = off->UClass.ClassDefaultObject ? off->UClass.ClassDefaultObject + sizeof(void *) : 0x80;
            for (uintptr_t candidate = start; candidate <= 0x350; candidate += sizeof(void *))
            {
                if (!ValidateImplementedInterfacesOffset(candidate))
                    continue;
                LOGI("[AutoFix] Rescanned UClass.ImplementedInterfaces=0x%lx (was 0x%lx)",
                     (unsigned long)candidate,
                     (unsigned long)before);
                off->UClass.ImplementedInterfaces = candidate;
                return true;
            }

            LOGI("[AutoFix] UClass.ImplementedInterfaces not confirmed by scan; keeping preset");
            return false;
        }

        bool Validate_UClassCastFlags(uintptr_t candidate)
        {
            struct ClassCastAnchor
            {
                const char *Derived;
                const char *Base;
            };

            static const ClassCastAnchor kAnchors[] = {
                {"Class CoreUObject.Function", "Class CoreUObject.Struct"},
                {"Class CoreUObject.Class", "Class CoreUObject.Struct"},
                {"Class CoreUObject.ScriptStruct", "Class CoreUObject.Struct"},
            };

            int valid = 0;
            int hits = 0;
            for (const ClassCastAnchor &anchor : kAnchors)
            {
                const UE_UClass derived = FindClassByFullName(anchor.Derived);
                const UE_UClass base = FindClassByFullName(anchor.Base);
                if (!derived || !base)
                    continue;

                ++valid;
                const uint64_t derivedFlags = ReadU64(reinterpret_cast<uintptr_t>(derived.GetAddress()) + candidate);
                const uint64_t baseFlags = ReadU64(reinterpret_cast<uintptr_t>(base.GetAddress()) + candidate);
                if (IsLikelyBitmaskU64(derivedFlags) && IsLikelyBitmaskU64(baseFlags) && derivedFlags != baseFlags &&
                    (derivedFlags & baseFlags) == baseFlags)
                {
                    ++hits;
                }
            }

            return valid >= 2 && hits >= std::min(valid, 2);
        }

        bool Fixup_UClassCastFlags()
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects())
                return false;

            const uintptr_t before = off->UClass.CastFlags;
            if (before && Validate_UClassCastFlags(before))
            {
                LOGI("[AutoFix] UClass.CastFlags=0x%lx validated", (unsigned long)before);
                return true;
            }

            for (uintptr_t candidate = 0x50; candidate <= 0x250; candidate += sizeof(uint64_t))
            {
                if (candidate == off->UClass.ClassDefaultObject || candidate == off->UClass.ImplementedInterfaces)
                    continue;
                if (!Validate_UClassCastFlags(candidate))
                    continue;

                LOGI("[AutoFix] Rescanned UClass.CastFlags=0x%lx (was 0x%lx)",
                     (unsigned long)candidate,
                     (unsigned long)before);
                off->UClass.CastFlags = candidate;
                return true;
            }

            LOGI("[AutoFix] UClass.CastFlags not confirmed by scan; keeping preset");
            return false;
        }

        bool Validate_UClassClassFlags(uintptr_t candidate)
        {
            static const char *kAnchors[] = {
                "Class CoreUObject.Object",
                "Class CoreUObject.Class",
                "Class CoreUObject.Function",
                "Class CoreUObject.ScriptStruct",
                "Class Engine.Actor",
            };

            int valid = 0;
            int nonZero = 0;
            uint32_t first = 0;
            int distinct = 0;
            for (const char *anchor : kAnchors)
            {
                const UE_UClass cls = FindClassByFullName(anchor);
                if (!cls)
                    continue;

                ++valid;
                const uint32_t value = ReadU32(reinterpret_cast<uintptr_t>(cls.GetAddress()) + candidate);
                if (value == 0 || value == 0xFFFFFFFFu)
                    continue;
                if (value & 0x80000000u)
                    continue;
                ++nonZero;
                if (!first)
                {
                    first = value;
                    distinct = 1;
                }
                else if (value != first)
                {
                    ++distinct;
                }
            }

            return valid >= 3 && nonZero >= 3 && distinct >= 2;
        }

        bool Fixup_UClassClassFlags()
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects())
                return false;

            const uintptr_t before = off->UClass.ClassFlags;
            if (before && Validate_UClassClassFlags(before))
            {
                LOGI("[AutoFix] UClass.ClassFlags=0x%lx validated", (unsigned long)before);
                return true;
            }

            const uintptr_t start = off->UClass.CastFlags ? off->UClass.CastFlags + sizeof(uint64_t) : 0x70;
            for (uintptr_t candidate = start; candidate <= 0x250; candidate += sizeof(uint32_t))
            {
                if (candidate == off->UClass.ClassDefaultObject || candidate == off->UClass.ImplementedInterfaces)
                    continue;
                if (!Validate_UClassClassFlags(candidate))
                    continue;

                LOGI("[AutoFix] Rescanned UClass.ClassFlags=0x%lx (was 0x%lx)",
                     (unsigned long)candidate,
                     (unsigned long)before);
                off->UClass.ClassFlags = candidate;
                return true;
            }

            LOGI("[AutoFix] UClass.ClassFlags not confirmed by scan; keeping preset");
            return false;
        }

        // ----- UStruct.Children fixup ----------------------------------------------------
        // dumper-7 对 Children 的锚点分两类：
        // 1. 老版本/无 FField：Children 链里能看到 PersistentLevel / NetDriver 这类 UProperty。
        // 2. 使用 FNamePool/ChildProperties：Children 更常指向 UFunction 链，典型名字是 K2_GetWorldSettings。
        // 因此这里不能只拿属性名验证，否则会把正确 preset 误判成失败。
        bool ValidateChildrenChain(uintptr_t candidate)
        {
            UE_Offsets *off = Offsets();
            if (!candidate || !off->UObject.NamePrivate || !Objects())
                return false;

            const UE_UClass worldClass = Objects()->FindObject("Class Engine.World").Cast<UE_UClass>();
            if (!worldClass)
                return false;

            const uintptr_t worldAddr = reinterpret_cast<uintptr_t>(worldClass.GetAddress());
            const uintptr_t nameOff = off->UObject.NamePrivate;
            const uintptr_t fieldNext = off->UField.Next ? off->UField.Next : (sizeof(void *) * 4);

            uintptr_t child = ReadPtr(worldAddr + candidate);
            int depth = 0;
            while (IsLikelyPtr(child) && depth < 64)
            {
                const std::string name = ReadObjectName(child, nameOff);
                if (IsKnownWorldPropertyName(name) || IsKnownWorldFunctionName(name))
                    return true;

                if (off->Config.IsUsingFNamePool)
                {
                    const uintptr_t classPtr = ReadPtr(child + off->UObject.ClassPrivate);
                    const std::string className = ReadObjectName(classPtr, nameOff);
                    if (className == "Function" && IsIdentifierLike(name))
                        return true;
                }

                child = ReadPtr(child + fieldNext);
                ++depth;
            }
            return false;
        }

        bool FixupChildren()
        {
            UE_Offsets *off = Offsets();
            if (!off || off->UObject.NamePrivate == 0 || off->UStruct.SuperStruct == 0 || !Objects())
                return false;

            const UE_UClass worldClass = Objects()->FindObject("Class Engine.World").Cast<UE_UClass>();
            const uintptr_t worldAddr = worldClass ? reinterpret_cast<uintptr_t>(worldClass.GetAddress()) : 0;

            const uintptr_t before = off->UStruct.Children;
            if (before && ValidateChildrenChain(before))
            {
                LOGI("[AutoFix] UStruct.Children=0x%lx validated", (unsigned long)before);
                return true;
            }

            const uintptr_t derived = off->UStruct.SuperStruct + sizeof(void *);
            if (ValidateChildrenChain(derived))
            {
                LOGI("[AutoFix] Derived UStruct.Children=0x%lx (was 0x%lx) from SuperStruct+ptr",
                     (unsigned long)derived, (unsigned long)before);
                off->UStruct.Children = derived;
                return true;
            }

            if (off->Config.IsUsingFNamePool && off->UStruct.ChildProperties && worldAddr)
            {
                const uintptr_t maybeChild = ReadPtr(worldAddr + derived);
                if (!before && IsLikelyPtr(maybeChild))
                {
                    LOGI("[AutoFix] Derived UStruct.Children=0x%lx from SuperStruct+ptr under FField layout",
                         (unsigned long)derived);
                    off->UStruct.Children = derived;
                    return true;
                }

                if (before == derived)
                {
                    LOGI("[AutoFix] UStruct.Children kept at 0x%lx under FField layout", (unsigned long)before);
                    return true;
                }
            }

            for (uintptr_t candidate = off->UStruct.SuperStruct + sizeof(void *) * 2;
                 candidate <= off->UStruct.SuperStruct + 0x40;
                 candidate += sizeof(void *))
            {
                if (!ValidateChildrenChain(candidate))
                    continue;
                LOGI("[AutoFix] Rescanned UStruct.Children=0x%lx (was 0x%lx)",
                     (unsigned long)candidate, (unsigned long)before);
                off->UStruct.Children = candidate;
                return true;
            }

            LOGI("[AutoFix] UStruct.Children not confirmed by scan; keeping preset");
            return false;
        }

        // ----- UStruct.ChildProperties fixup --------------------------------------------
        // 仅 UE >= 4.25 启用。锚点：World 类的 ChildProperties 链表中应当出现 FField 形式的
        // 子属性，且 FField 的 NamePrivate 解码出合法标识符（比如 "OwningGameInstance"）。
        // 同步派生 PropertiesSize = ChildProperties + sizeof(void*)（dumper-7 的兜底逻辑）。
        bool ValidateChildPropertiesChain(uintptr_t candidate)
        {
            UE_Offsets *off = Offsets();
            if (!candidate || !off->FField.NamePrivate || !Objects())
                return false;
            if (!off->FField.Next)
                return false;

            const UE_UClass worldClass = Objects()->FindObject("Class Engine.World").Cast<UE_UClass>();
            if (!worldClass)
                return false;

            const uintptr_t worldAddr = reinterpret_cast<uintptr_t>(worldClass.GetAddress());
            uintptr_t prop = ReadPtr(worldAddr + candidate);
            int knownNameHits = 0;
            int propertyClassHits = 0;
            int identifierHits = 0;
            int depth = 0;
            while (IsLikelyPtr(prop) && depth < 256)
            {
                const std::string name = ReadObjectName(prop, off->FField.NamePrivate);
                if (IsIdentifierLike(name))
                    ++identifierHits;
                if (IsKnownWorldPropertyName(name))
                    ++knownNameHits;
                if (off->FField.ClassPrivate)
                {
                    const std::string className = ReadFFieldClassName(ReadPtr(prop + off->FField.ClassPrivate));
                    if (IsPropertyClassName(className))
                        ++propertyClassHits;
                }
                if (knownNameHits >= 1 && (propertyClassHits >= 2 || identifierHits >= 3))
                    return true;
                prop = ReadPtr(prop + off->FField.Next);
                ++depth;
            }
            return knownNameHits >= 1 && (propertyClassHits >= 1 || identifierHits >= 2);
        }

        bool FixupChildProperties()
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects())
                return false;
            if (!off->Config.IsUsingFNamePool && off->FField.NamePrivate == 0)
                return false;

            const uintptr_t before = off->UStruct.ChildProperties;
            if (before && ValidateChildPropertiesChain(before))
            {
                LOGI("[AutoFix] UStruct.ChildProperties=0x%lx validated", (unsigned long)before);
                return true;
            }

            const uintptr_t childrenOff = off->UStruct.Children;
            if (childrenOff)
            {
                const uintptr_t derived = childrenOff + sizeof(void *);
                if (ValidateChildPropertiesChain(derived))
                {
                    LOGI("[AutoFix] Derived UStruct.ChildProperties=0x%lx (was 0x%lx)",
                         (unsigned long)derived, (unsigned long)before);
                    off->UStruct.ChildProperties = derived;
                    if (off->UStruct.PropertiesSize == 0)
                        off->UStruct.PropertiesSize = derived + sizeof(void *);
                    return true;
                }
            }

            const uintptr_t scanFrom = childrenOff ? (childrenOff + sizeof(void *))
                                                  : (off->UStruct.SuperStruct + sizeof(void *) * 2);
            for (uintptr_t candidate = scanFrom;
                 candidate <= scanFrom + 0x40;
                 candidate += sizeof(void *))
            {
                if (!ValidateChildPropertiesChain(candidate))
                    continue;
                LOGI("[AutoFix] Rescanned UStruct.ChildProperties=0x%lx (was 0x%lx)",
                     (unsigned long)candidate, (unsigned long)before);
                off->UStruct.ChildProperties = candidate;
                if (off->UStruct.PropertiesSize == 0)
                    off->UStruct.PropertiesSize = candidate + sizeof(void *);
                return true;
            }

            LOGW("[AutoFix] UStruct.ChildProperties fixup skipped/failed; keeping preset");
            return false;
        }

        // ----- UEnum.Names fixup ---------------------------------------------------------
        // 验证：UEnum 对象在 Names 偏移处应当是一个 TArray<TPair<FName,int64>>，其 Num 在
        // 合理范围内（2..4096），Data 指针有效，第一项的 FName 能被解码成合法标识符。
        bool ValidateUEnumNames(uintptr_t candidate, int sampleCap)
        {
            if (!candidate || !Objects())
                return false;
            UE_Offsets *off = Offsets();
            const uintptr_t nameOff = off->UObject.NamePrivate;
            if (!nameOff)
                return false;

            int hits = 0;
            int probed = 0;
            Objects()->ForEachObject([&](UE_UObject object)
            {
                if (probed >= sampleCap)
                    return true;

                const uintptr_t addr = reinterpret_cast<uintptr_t>(object.GetAddress());
                const uintptr_t classPtr = ReadPtr(addr + off->UObject.ClassPrivate);
                const std::string className = ReadObjectName(classPtr, nameOff);
                if (className != "Enum" && className != "UserDefinedEnum")
                    return false;

                ++probed;
                const uintptr_t dataPtr = ReadPtr(addr + candidate);
                const int32_t num = ReadI32(addr + candidate + sizeof(void *));
                const int32_t maxN = ReadI32(addr + candidate + sizeof(void *) + sizeof(int32_t));
                if (!IsLikelyPtr(dataPtr) || num < 2 || num > 65536 || maxN < num)
                    return false;

                const std::string firstName = ReadNameById(ReadI32(dataPtr));
                const std::string secondName = ReadNameById(ReadI32(dataPtr + 0x10));
                if (IsIdentifierLike(firstName) && IsIdentifierLike(secondName))
                    ++hits;
                return false;
            });
            return hits >= 1;
        }

        uintptr_t FindUEnumNamesOffsetFromKnownCounts()
        {
            struct EnumCountAnchor
            {
                const char *Name;
                int32_t Count;
            };

            static const EnumCountAnchor kAnchors[] = {
                {"EInterpCurveMode", 5},
                {"ENetRole", 5},
                {"ETraceTypeQuery", 0x22},
            };

            for (const EnumCountAnchor &anchor : kAnchors)
            {
                const UE_UEnum obj = Objects()->FindObjectFast<UE_UEnum>(anchor.Name);
                if (!obj)
                    continue;

                const uintptr_t base = reinterpret_cast<uintptr_t>(obj.GetAddress());
                for (uintptr_t countOff = 0x10 + sizeof(void *); countOff <= 0x100; countOff += sizeof(int32_t))
                {
                    if (ReadI32(base + countOff) != anchor.Count)
                        continue;

                    const uintptr_t arrayOff = countOff - sizeof(void *);
                    const uintptr_t dataPtr = ReadPtr(base + arrayOff);
                    const int32_t maxCount = ReadI32(base + countOff + sizeof(int32_t));
                    if (IsLikelyPtr(dataPtr) && IsPlausibleArrayCount(anchor.Count, maxCount, 0x100))
                        return arrayOff;
                }
            }

            return 0;
        }

        bool FixupUEnumNames()
        {
            UE_Offsets *off = Offsets();
            if (!off || off->UObject.NamePrivate == 0 || !Objects())
                return false;

            const uintptr_t before = off->UEnum.Names;

            uintptr_t scannedOffset = 0;
            Objects()->ForEachObject([&](UE_UObject object)
            {
                if (scannedOffset != 0)
                    return true;

                const uintptr_t addr = reinterpret_cast<uintptr_t>(object.GetAddress());
                const std::string objName = object.GetName();
                if (objName != "EInterpCurveMode")
                    return false;

                for (int d = 0; d < 50; ++d)
                {
                    const uintptr_t memberPtr = ReadPtr(addr + d * sizeof(void *));
                    if (!IsLikelyPtr(memberPtr))
                        continue;

                    const std::string memberName = ReadObjectName(memberPtr, off->UObject.NamePrivate);
                    if (memberName == "CIM_Linear")
                    {
                        scannedOffset = d * sizeof(void *);
                        LOGI("[AutoFix] Rescanned UEnum.Names=0x%lx (was 0x%lx) from EInterpCurveMode.CIM_Linear anchor",
                             (unsigned long)scannedOffset, (unsigned long)before);
                        return true;
                    }
                }
                return false;
            });

            if (scannedOffset != 0)
            {
                off->UEnum.Names = scannedOffset;
                return true;
            }

            const uintptr_t countDerived = FindUEnumNamesOffsetFromKnownCounts();
            if (countDerived && ValidateUEnumNames(countDerived, kStrictSampleCap))
            {
                LOGI("[AutoFix] Rescanned UEnum.Names=0x%lx (was 0x%lx) from known Enum count anchors",
                     (unsigned long)countDerived, (unsigned long)before);
                off->UEnum.Names = countDerived;
                return true;
            }

            for (uintptr_t candidate = 0x28; candidate <= 0x80; candidate += sizeof(void *))
            {
                if (!ValidateUEnumNames(candidate, kStrictSampleCap))
                    continue;
                LOGI("[AutoFix] Rescanned UEnum.Names=0x%lx (was 0x%lx) from generic Enum scan",
                     (unsigned long)candidate, (unsigned long)before);
                off->UEnum.Names = candidate;
                return true;
            }

            LOGI("[AutoFix] UEnum.Names not confirmed by scan; keeping preset");
            return false;
        }

        struct FunctionLayoutAnchor
        {
            UE_UFunction Object;
            const char *Name = nullptr;
            uint32_t ExpectedFlags = 0;
            bool HasMeasuredLayout = false;
            int32_t NumParams = 0;
            int32_t ParamSize = 0;
        };

        bool TryMeasureFunctionLayout(const UE_UFunction &function, int32_t &outNumParams, int32_t &outParamSize)
        {
            UE_Offsets *off = Offsets();
            if (!function || !off)
                return false;

            int32_t numParams = 0;
            int32_t maxParamEnd = 0;
            bool traversed = false;

            if (off->Config.IsUsingFNamePool && off->UStruct.ChildProperties)
            {
                int guard = 0;
                for (UE_FField field = function.GetChildProperties(); field && guard < 256; field = field.GetNext(), ++guard)
                {
                    traversed = true;
                    const std::string className = field.GetClass().GetName();
                    if (className.find("Property") == std::string::npos)
                        continue;

                    const UE_FProperty prop = field.Cast<UE_FProperty>();
                    if ((prop.GetPropertyFlags() & CPF_Parm) == 0)
                        continue;

                    ++numParams;
                    maxParamEnd = std::max(maxParamEnd, prop.GetOffset() + prop.GetSize());
                }
            }
            else
            {
                int guard = 0;
                for (UE_UField field = function.GetChildren(); field && guard < 256; field = field.GetNext(), ++guard)
                {
                    traversed = true;
                    const UE_UObject object = field.Cast<UE_UObject>();
                    if (!object.IsA<UE_UProperty>())
                        continue;

                    const UE_UProperty prop = field.Cast<UE_UProperty>();
                    if ((prop.GetPropertyFlags() & CPF_Parm) == 0)
                        continue;

                    ++numParams;
                    maxParamEnd = std::max(maxParamEnd, prop.GetOffset() + prop.GetSize());
                }
            }

            if (!traversed)
                return false;

            outNumParams = numParams;
            outParamSize = maxParamEnd;
            return true;
        }

        std::vector<FunctionLayoutAnchor> BuildFunctionLayoutAnchors()
        {
            std::vector<FunctionLayoutAnchor> anchors;
            if (!Objects())
                return anchors;

            auto addAnchor = [&](const char *name, uint32_t expectedFlags)
            {
                const UE_UFunction function = Objects()->FindObjectFast<UE_UFunction>(name);
                if (!function)
                    return;

                FunctionLayoutAnchor anchor;
                anchor.Object = function;
                anchor.Name = name;
                anchor.ExpectedFlags = expectedFlags;
                anchor.HasMeasuredLayout = TryMeasureFunctionLayout(function, anchor.NumParams, anchor.ParamSize);
                anchors.push_back(anchor);
            };

            addAnchor("WasInputKeyJustPressed", FUNC_Final | FUNC_Native | FUNC_Public | FUNC_BlueprintCallable | FUNC_BlueprintPure | FUNC_Const);
            addAnchor("ToggleSpeaking", FUNC_Exec | FUNC_Native | FUNC_Public);
            addAnchor("SwitchLevel", FUNC_Exec | FUNC_Native | FUNC_Public);
            if (anchors.size() < 3)
                addAnchor("FOV", FUNC_Exec | FUNC_Native | FUNC_Public);

            return anchors;
        }

        bool MatchesExpectedFunctionFlags(uint32_t actual, uint32_t expected)
        {
            return actual == expected || actual == (expected | FUNC_RequiredAPI);
        }

        bool MatchesMeasuredParamSize(int32_t measuredSize, int32_t actualSize)
        {
            return actualSize == measuredSize || (actualSize >= measuredSize && actualSize <= (measuredSize + 0x8));
        }

        bool ValidateUFunctionLayoutOffsets(uintptr_t flagsOff,
                                            uintptr_t numParamsOff,
                                            uintptr_t paramSizeOff,
                                            const std::vector<FunctionLayoutAnchor> &anchors)
        {
            if (!flagsOff || !numParamsOff || !paramSizeOff || anchors.size() < 2)
                return false;

            int measuredMatches = 0;
            int measuredCount = 0;
            for (const FunctionLayoutAnchor &anchor : anchors)
            {
                const uintptr_t address = reinterpret_cast<uintptr_t>(anchor.Object.GetAddress());
                if (!IsLikelyPtr(address))
                    return false;

                const uint32_t flags = ReadU32(address + flagsOff);
                if (!MatchesExpectedFunctionFlags(flags, anchor.ExpectedFlags))
                    return false;

                const int32_t numParams = static_cast<int32_t>(ReadI8(address + numParamsOff));
                const int32_t paramSize = static_cast<int32_t>(ReadI16(address + paramSizeOff));
                if (numParams < 0 || numParams > 0x40 || paramSize < 0 || paramSize > 0x4000)
                    return false;

                if (anchor.HasMeasuredLayout)
                {
                    ++measuredCount;
                    if (numParams != anchor.NumParams || !MatchesMeasuredParamSize(anchor.ParamSize, paramSize))
                        return false;
                    ++measuredMatches;
                }
            }

            return measuredCount == 0 || measuredMatches >= std::min(measuredCount, 2);
        }

        bool FixupUFunctionLayout()
        {
            UE_Offsets *off = Offsets();
            if (!off || !Objects())
                return false;

            const auto anchors = BuildFunctionLayoutAnchors();
            if (anchors.size() < 2)
            {
                LOGI("[AutoFix] UFunction metadata scan skipped; not enough anchor functions");
                return false;
            }

            const uintptr_t beforeFlags = off->UFunction.EFunctionFlags;
            const uintptr_t beforeNumParams = off->UFunction.NumParams;
            const uintptr_t beforeParamSize = off->UFunction.ParamSize;
            if (ValidateUFunctionLayoutOffsets(beforeFlags, beforeNumParams, beforeParamSize, anchors))
            {
                LOGI("[AutoFix] UFunction layout validated: Flags=0x%lx NumParams=0x%lx ParamSize=0x%lx",
                     (unsigned long)beforeFlags,
                     (unsigned long)beforeNumParams,
                     (unsigned long)beforeParamSize);
                return true;
            }

            const uintptr_t searchStart = beforeFlags && beforeFlags > 0x20 ? (beforeFlags - 0x20) : 0x40;
            const uintptr_t searchEnd = std::max<uintptr_t>(beforeFlags ? (beforeFlags + 0x20) : 0xA0, 0xA0);
            for (uintptr_t flagsOff = searchStart; flagsOff <= searchEnd; flagsOff += sizeof(int32_t))
            {
                for (uintptr_t numParamsOff = flagsOff + sizeof(int32_t); numParamsOff <= flagsOff + 0x8; ++numParamsOff)
                {
                    for (uintptr_t paramSizeOff = numParamsOff + 1; paramSizeOff <= numParamsOff + 0x4; ++paramSizeOff)
                    {
                        if (!ValidateUFunctionLayoutOffsets(flagsOff, numParamsOff, paramSizeOff, anchors))
                            continue;

                        off->UFunction.EFunctionFlags = flagsOff;
                        off->UFunction.NumParams = numParamsOff;
                        off->UFunction.ParamSize = paramSizeOff;
                        LOGI("[AutoFix] Rescanned UFunction layout: Flags=0x%lx (was 0x%lx) NumParams=0x%lx (was 0x%lx) ParamSize=0x%lx (was 0x%lx)",
                             (unsigned long)flagsOff,
                             (unsigned long)beforeFlags,
                             (unsigned long)numParamsOff,
                             (unsigned long)beforeNumParams,
                             (unsigned long)paramSizeOff,
                             (unsigned long)beforeParamSize);
                        return true;
                    }
                }
            }

            LOGI("[AutoFix] UFunction metadata not confirmed by scan; keeping preset");
            return false;
        }

        // ----- FField 布局 (UE >= 4.25) ---------------------------------------------------
        // FField 是 UE 4.25 起把 UProperty 从 UField 派生改成独立的 FField/FFieldClass 类型。
        // 布局:
        //   FField {
        //     void* vtable;
        //     FFieldClass* ClassPrivate;     // 类似 UClass，有 Name/SuperClass
        //     void* Owner;                   // FFieldVariant（UStruct* 或 FField*）
        //     FField* Next;
        //     FName NamePrivate;
        //     EObjectFlags FlagsPrivate;
        //   }
        // 锚点：World 类 ChildProperties 链上的某个属性，其 FFieldClass.Name 应当是已知
        // 类名（ObjectProperty / StructProperty 等），其 NamePrivate 应当是已知字段名
        // （PersistentLevel / OwningGameInstance / NetDriver 等）。
        bool ValidateFFieldClass(uintptr_t classOff)
        {
            UE_Offsets *off = Offsets();
            if (!classOff || !off->UObject.NamePrivate || !off->UStruct.ChildProperties)
                return false;
            const UE_UClass worldClass = Objects()->FindObject("Class Engine.World").Cast<UE_UClass>();
            if (!worldClass)
                return false;

            const uintptr_t worldAddr = reinterpret_cast<uintptr_t>(worldClass.GetAddress());
            uintptr_t prop = ReadPtr(worldAddr + off->UStruct.ChildProperties);
            int hits = 0;
            int depth = 0;
            const uintptr_t nextOff = off->FField.Next ? off->FField.Next : 0x20;
            while (IsLikelyPtr(prop) && depth < 64)
            {
                const uintptr_t fclass = ReadPtr(prop + classOff);
                if (IsLikelyPtr(fclass))
                {
                    const std::string clsName = ReadFFieldClassName(fclass);
                    if (IsPropertyClassName(clsName))
                        ++hits;
                    if (hits >= 2)
                        return true;
                }
                prop = ReadPtr(prop + nextOff);
                ++depth;
            }
            return hits >= 1;
        }

        bool FixupFFieldClass()
        {
            UE_Offsets *off = Offsets();
            if (!off || !off->Config.IsUsingFNamePool || !off->UStruct.ChildProperties)
                return false;

            const uintptr_t before = off->FField.ClassPrivate;
            if (before && ValidateFFieldClass(before))
            {
                LOGI("[AutoFix] FField.ClassPrivate=0x%lx validated", (unsigned long)before);
                return true;
            }
            for (uintptr_t candidate = 0x8; candidate <= 0x18; candidate += sizeof(void *))
            {
                if (!ValidateFFieldClass(candidate))
                    continue;
                LOGI("[AutoFix] Rescanned FField.ClassPrivate=0x%lx (was 0x%lx)",
                     (unsigned long)candidate, (unsigned long)before);
                off->FField.ClassPrivate = candidate;
                return true;
            }
            LOGI("[AutoFix] FField.ClassPrivate not confirmed by scan; keeping preset");
            return false;
        }

        bool ValidateFFieldOwner(uintptr_t ownerOff)
        {
            UE_Offsets *off = Offsets();
            if (!off || !ownerOff || !off->Config.IsUsingFNamePool || !off->UStruct.ChildProperties || !off->FField.Next || !off->FField.NamePrivate)
                return false;

            int valid = 0;
            int hits = 0;
            auto checkAnchor = [&](const char *ownerFullName, std::initializer_list<const char *> fieldNames)
            {
                const UE_UClass owner = FindClassByFullName(ownerFullName);
                const uintptr_t field = FindNamedFFieldInOwner(ownerFullName, fieldNames);
                if (!owner || !field)
                    return;

                ++valid;
                if (ReadPtr(field + ownerOff) == reinterpret_cast<uintptr_t>(owner.GetAddress()))
                    ++hits;
            };

            checkAnchor("Class Engine.World", {"PersistentLevel", "OwningGameInstance", "NetDriver", "DemoNetDriver"});
            checkAnchor("Class Engine.Engine", {"TinyFont", "SmallFont", "MediumFont", "LargeFont"});
            return valid >= 2 && hits == valid;
        }

        bool FixupFFieldOwner()
        {
            UE_Offsets *off = Offsets();
            if (!off || !off->Config.IsUsingFNamePool)
                return false;

            const uintptr_t before = off->FField.Owner;
            if (before && ValidateFFieldOwner(before))
            {
                LOGI("[AutoFix] FField.Owner=0x%lx validated", (unsigned long)before);
                return true;
            }

            const uintptr_t derived = off->FField.ClassPrivate ? (off->FField.ClassPrivate + sizeof(void *)) : 0;
            if (derived && ValidateFFieldOwner(derived))
            {
                LOGI("[AutoFix] Derived FField.Owner=0x%lx (was 0x%lx)",
                     (unsigned long)derived,
                     (unsigned long)before);
                off->FField.Owner = derived;
                return true;
            }

            const uintptr_t end = off->FField.Next > sizeof(void *) ? off->FField.Next - sizeof(void *) : 0x28;
            for (uintptr_t candidate = off->FField.ClassPrivate ? off->FField.ClassPrivate + sizeof(void *) : 0x10;
                 candidate <= end;
                 candidate += sizeof(void *))
            {
                if (!ValidateFFieldOwner(candidate))
                    continue;

                LOGI("[AutoFix] Rescanned FField.Owner=0x%lx (was 0x%lx)",
                     (unsigned long)candidate,
                     (unsigned long)before);
                off->FField.Owner = candidate;
                return true;
            }

            LOGI("[AutoFix] FField.Owner not confirmed by scan; keeping preset");
            return false;
        }

        bool ValidateFFieldClassSuperClass(uintptr_t candidate)
        {
            struct FieldClassAnchor
            {
                const char *Derived;
                const char *Base;
            };

            static const FieldClassAnchor kAnchors[] = {
                {"ObjectProperty", "ObjectPropertyBase"},
                {"ClassProperty", "ObjectProperty"},
                {"StructProperty", "Property"},
                {"EnumProperty", "Property"},
                {"ArrayProperty", "Property"},
            };

            int valid = 0;
            int hits = 0;
            for (const FieldClassAnchor &anchor : kAnchors)
            {
                const uintptr_t derived = FindAnyFFieldClassByName(anchor.Derived);
                const uintptr_t base = FindAnyFFieldClassByName(anchor.Base);
                if (!IsLikelyPtr(derived) || !IsLikelyPtr(base))
                    continue;

                ++valid;
                if (ReadPtr(derived + candidate) == base)
                    ++hits;
            }

            return valid >= 2 && hits >= std::min(valid, 2);
        }

        bool FixupFFieldClassSuperClass()
        {
            UE_Offsets *off = Offsets();
            if (!off || !off->Config.IsUsingFNamePool || !off->FField.ClassPrivate)
                return false;

            const uintptr_t before = off->FFieldClass.SuperClass;
            if (before && ValidateFFieldClassSuperClass(before))
            {
                LOGI("[AutoFix] FFieldClass.SuperClass=0x%lx validated", (unsigned long)before);
                return true;
            }

            for (uintptr_t candidate = 0x8; candidate <= 0x40; candidate += sizeof(void *))
            {
                if (!ValidateFFieldClassSuperClass(candidate))
                    continue;

                LOGI("[AutoFix] Rescanned FFieldClass.SuperClass=0x%lx (was 0x%lx)",
                     (unsigned long)candidate,
                     (unsigned long)before);
                off->FFieldClass.SuperClass = candidate;
                return true;
            }

            LOGI("[AutoFix] FFieldClass.SuperClass not confirmed by scan; keeping preset");
            return false;
        }

        bool ValidateFFieldClassCastFlags(uintptr_t candidate)
        {
            UE_Offsets *off = Offsets();
            if (!off || !candidate || !off->FFieldClass.SuperClass)
                return false;

            struct FieldClassAnchor
            {
                const char *Derived;
                const char *Base;
            };

            static const FieldClassAnchor kAnchors[] = {
                {"ObjectProperty", "ObjectPropertyBase"},
                {"ClassProperty", "ObjectProperty"},
                {"StructProperty", "Property"},
                {"EnumProperty", "Property"},
            };

            int valid = 0;
            int hits = 0;
            for (const FieldClassAnchor &anchor : kAnchors)
            {
                const uintptr_t derived = FindAnyFFieldClassByName(anchor.Derived);
                const uintptr_t base = FindAnyFFieldClassByName(anchor.Base);
                if (!IsLikelyPtr(derived) || !IsLikelyPtr(base))
                    continue;

                ++valid;
                const uint64_t derivedFlags = ReadU64(derived + candidate);
                const uint64_t baseFlags = ReadU64(base + candidate);
                if (IsLikelyBitmaskU64(derivedFlags) && IsLikelyBitmaskU64(baseFlags) && derivedFlags != baseFlags &&
                    (derivedFlags & baseFlags) == baseFlags)
                {
                    ++hits;
                }
            }

            return valid >= 2 && hits >= std::min(valid, 2);
        }

        bool FixupFFieldClassCastFlags()
        {
            UE_Offsets *off = Offsets();
            if (!off || !off->Config.IsUsingFNamePool)
                return false;

            const uintptr_t before = off->FFieldClass.CastFlags;
            if (before && ValidateFFieldClassCastFlags(before))
            {
                LOGI("[AutoFix] FFieldClass.CastFlags=0x%lx validated", (unsigned long)before);
                return true;
            }

            for (uintptr_t candidate = 0x8; candidate <= 0x40; candidate += sizeof(uint64_t))
            {
                if (candidate == off->FFieldClass.SuperClass)
                    continue;
                if (!ValidateFFieldClassCastFlags(candidate))
                    continue;

                LOGI("[AutoFix] Rescanned FFieldClass.CastFlags=0x%lx (was 0x%lx)",
                     (unsigned long)candidate,
                     (unsigned long)before);
                off->FFieldClass.CastFlags = candidate;
                return true;
            }

            LOGI("[AutoFix] FFieldClass.CastFlags not confirmed by scan; keeping preset");
            return false;
        }

        bool ValidateFFieldNext(uintptr_t nextOff)
        {
            UE_Offsets *off = Offsets();
            if (!nextOff || !off->FField.NamePrivate || !off->UStruct.ChildProperties)
                return false;
            if (nextOff == off->FField.NamePrivate || nextOff == off->FField.ClassPrivate)
                return false;
            const UE_UClass worldClass = Objects()->FindObject("Class Engine.World").Cast<UE_UClass>();
            if (!worldClass)
                return false;

            const uintptr_t worldAddr = reinterpret_cast<uintptr_t>(worldClass.GetAddress());
            uintptr_t prop = ReadPtr(worldAddr + off->UStruct.ChildProperties);
            int validHits = 0;
            int depth = 0;
            while (IsLikelyPtr(prop) && depth < 256)
            {
                const std::string currentName = ReadObjectName(prop, off->FField.NamePrivate);
                const uintptr_t nextPtr = ReadPtr(prop + nextOff);
                if (IsLikelyPtr(nextPtr) && nextPtr != prop)
                {
                    const std::string nextName = ReadObjectName(nextPtr, off->FField.NamePrivate);
                    if (IsIdentifierLike(currentName) && IsIdentifierLike(nextName) && nextName != currentName &&
                        (IsKnownWorldPropertyName(currentName) || IsKnownWorldPropertyName(nextName)))
                    {
                        if (off->FField.ClassPrivate)
                        {
                            const std::string nextClassName = ReadFFieldClassName(ReadPtr(nextPtr + off->FField.ClassPrivate));
                            if (!IsPropertyClassName(nextClassName))
                            {
                                prop = nextPtr;
                                ++depth;
                                continue;
                            }
                        }
                        ++validHits;
                    }
                }
                if (validHits >= 3)
                    return true;
                prop = nextPtr;
                ++depth;
            }
            return validHits >= 2;
        }

        bool FixupFFieldNext()
        {
            UE_Offsets *off = Offsets();
            if (!off || !off->Config.IsUsingFNamePool || !off->UStruct.ChildProperties)
                return false;

            const uintptr_t before = off->FField.Next;
            if (before && ValidateFFieldNext(before))
            {
                LOGI("[AutoFix] FField.Next=0x%lx validated", (unsigned long)before);
                return true;
            }
            for (uintptr_t candidate = 0x18; candidate <= 0x30; candidate += sizeof(void *))
            {
                if (!ValidateFFieldNext(candidate))
                    continue;
                LOGI("[AutoFix] Rescanned FField.Next=0x%lx (was 0x%lx)",
                     (unsigned long)candidate, (unsigned long)before);
                off->FField.Next = candidate;
                return true;
            }
            LOGW("[AutoFix] FField.Next fixup failed; keeping preset");
            return false;
        }

        bool ValidateFFieldName(uintptr_t nameOff)
        {
            UE_Offsets *off = Offsets();
            if (!nameOff || !off->UStruct.ChildProperties)
                return false;
            const UE_UClass worldClass = Objects()->FindObject("Class Engine.World").Cast<UE_UClass>();
            if (!worldClass)
                return false;

            const uintptr_t worldAddr = reinterpret_cast<uintptr_t>(worldClass.GetAddress());
            const uintptr_t nextOff = off->FField.Next ? off->FField.Next : 0x20;
            uintptr_t prop = ReadPtr(worldAddr + off->UStruct.ChildProperties);
            int hits = 0;
            int depth = 0;
            while (IsLikelyPtr(prop) && depth < 64)
            {
                const std::string nm = ReadObjectName(prop, nameOff);
                if (IsKnownWorldPropertyName(nm))
                    ++hits;
                if (hits >= 1)
                    return true;
                prop = ReadPtr(prop + nextOff);
                ++depth;
            }
            return false;
        }

        bool FixupFFieldName()
        {
            UE_Offsets *off = Offsets();
            if (!off || !off->Config.IsUsingFNamePool || !off->UStruct.ChildProperties)
                return false;

            const uintptr_t before = off->FField.NamePrivate;
            if (before && ValidateFFieldName(before))
            {
                LOGI("[AutoFix] FField.NamePrivate=0x%lx validated", (unsigned long)before);
                return true;
            }
            for (uintptr_t candidate = 0x18; candidate <= 0x40; candidate += 0x4)
            {
                if (!ValidateFFieldName(candidate))
                    continue;
                LOGI("[AutoFix] Rescanned FField.NamePrivate=0x%lx (was 0x%lx)",
                     (unsigned long)candidate, (unsigned long)before);
                off->FField.NamePrivate = candidate;
                return true;
            }
            LOGW("[AutoFix] FField.NamePrivate fixup failed; keeping preset");
            return false;
        }

        // ----- FProperty 布局 (UE >= 4.25) -----------------------------------------------
        // FProperty 继承 FField，紧接其后是 5 个核心字段（ArrayDim/ElementSize/Flags/Offset/Size）。
        // dumper-7 通过寻找 ArrayDim==1 + ElementSize==8 (大多数指针属性) 的双 int32 模式定位。
        // 锚点：World.PersistentLevel（ObjectProperty，ElementSize=8，ArrayDim=1）。
        bool FindPersistentLevelProp(uintptr_t &outAddr)
        {
            UE_Offsets *off = Offsets();
            if (!off->UStruct.ChildProperties || !off->FField.NamePrivate || !off->FField.Next)
                return false;
            outAddr = FindPropertyLayoutAnchor();
            return outAddr != 0;
        }

        bool ValidateFPropertyLayout(const FPropertyLayoutScanResult &layout)
        {
            if (!layout.ArrayDim || !layout.ElementSize || !layout.PropertyFlags || !layout.OffsetInternal)
                return false;

            const uintptr_t layoutAnchor = FindPropertyLayoutAnchor();
            if (!layoutAnchor)
                return false;

            FPropertyLayoutScanResult probed;
            if (!TryResolveFPropertyLayout(layoutAnchor, layout.ArrayDim, 0, probed))
                return false;
            if (probed.ElementSize != layout.ElementSize ||
                probed.PropertyFlags != layout.PropertyFlags ||
                probed.OffsetInternal != layout.OffsetInternal)
            {
                return false;
            }

            if (layout.Size)
            {
                const uintptr_t sizeAnchor = FindPropertySizeAnchor();
                uintptr_t sizeOff = 0;
                if (!sizeAnchor || !ProbeFPropertySizeFromAnchor(sizeAnchor, sizeOff) || sizeOff != layout.Size)
                    return false;
            }

            return true;
        }

        bool FixupFPropertyLayout()
        {
            UE_Offsets *off = Offsets();
            if (!off || !off->Config.IsUsingFNamePool || !off->FField.NamePrivate)
                return false;

            const FPropertyLayoutScanResult before{
                off->FProperty.ArrayDim,
                off->FProperty.ElementSize,
                off->FProperty.PropertyFlags,
                off->FProperty.Offset_Internal,
                off->FProperty.Size,
            };
            if (ValidateFPropertyLayout(before))
            {
                LOGI("[AutoFix] FProperty layout validated: ArrayDim=0x%lx ElementSize=0x%lx PropFlags=0x%lx Offset=0x%lx Size=0x%lx",
                     (unsigned long)before.ArrayDim,
                     (unsigned long)before.ElementSize,
                     (unsigned long)before.PropertyFlags,
                     (unsigned long)before.OffsetInternal,
                     (unsigned long)before.Size);
                return true;
            }

            FPropertyLayoutScanResult found;
            const uintptr_t sizeAnchor = FindPropertySizeAnchor();
            if (sizeAnchor)
            {
                ProbeFPropertySizeFromAnchor(sizeAnchor, found.Size);
            }

            const uintptr_t layoutAnchor = FindPropertyLayoutAnchor();
            if (layoutAnchor)
            {
                ProbeFPropertyLayoutFromAnchor(layoutAnchor, found.Size, found);
                if (!found.ArrayDim && off->FProperty.Size)
                    ProbeFPropertyLayoutFromAnchor(layoutAnchor, off->FProperty.Size, found);
            }

            if (!found.ArrayDim && layoutAnchor)
            {
                for (uintptr_t candidate = 0x28; candidate <= 0x60; candidate += sizeof(int32_t))
                {
                    if (TryResolveFPropertyLayout(layoutAnchor, candidate, found.Size, found))
                    {
                        break;
                    }
                }
            }

            if (!found.ArrayDim || !found.OffsetInternal)
            {
                LOGI("[AutoFix] FProperty layout not confirmed by scan; keeping preset");
                return false;
            }

            off->FProperty.ArrayDim = found.ArrayDim;
            off->FProperty.ElementSize = found.ElementSize;
            off->FProperty.PropertyFlags = found.PropertyFlags;
            off->FProperty.Offset_Internal = found.OffsetInternal;
            if (found.Size)
                off->FProperty.Size = found.Size;

            LOGI("[AutoFix] FProperty layout: ArrayDim=0x%lx ElementSize=0x%lx PropFlags=0x%lx Offset=0x%lx Size=0x%lx",
                 (unsigned long)off->FProperty.ArrayDim,
                 (unsigned long)off->FProperty.ElementSize,
                 (unsigned long)off->FProperty.PropertyFlags,
                 (unsigned long)off->FProperty.Offset_Internal,
                 (unsigned long)off->FProperty.Size);
            return true;
        }

        // ----- UField.Next (UE < 4.25) ---------------------------------------------------
        // UE4.24 之前，UProperty 派生自 UField，Children 链节点是 UField*，靠 UField.Next 串联。
        // 锚点：World 类的 Children 链中应当能找到 PersistentLevel/NetDriver。
        bool ValidateUFieldNext(uintptr_t nextOff)
        {
            UE_Offsets *off = Offsets();
            if (!nextOff || !off->UObject.NamePrivate || !off->UStruct.Children)
                return false;
            const UE_UClass worldClass = Objects()->FindObject("Class Engine.World").Cast<UE_UClass>();
            if (!worldClass)
                return false;

            const uintptr_t worldAddr = reinterpret_cast<uintptr_t>(worldClass.GetAddress());
            uintptr_t child = ReadPtr(worldAddr + off->UStruct.Children);
            int depth = 0;
            int hits = 0;
            while (IsLikelyPtr(child) && depth < 128)
            {
                const std::string nm = ReadObjectName(child, off->UObject.NamePrivate);
                if (IsIdentifierLike(nm))
                    ++hits;
                if (hits >= 3)
                    return true;
                child = ReadPtr(child + nextOff);
                ++depth;
            }
            return hits >= 2;
        }

        bool FixupUFieldNext()
        {
            UE_Offsets *off = Offsets();
            if (!off || !off->UStruct.Children)
                return false;

            const uintptr_t before = off->UField.Next;
            if (before && ValidateUFieldNext(before))
            {
                LOGI("[AutoFix] UField.Next=0x%lx validated", (unsigned long)before);
                return true;
            }
            for (uintptr_t candidate = 0x20; candidate <= 0x40; candidate += sizeof(void *))
            {
                if (!ValidateUFieldNext(candidate))
                    continue;
                LOGI("[AutoFix] Rescanned UField.Next=0x%lx (was 0x%lx)",
                     (unsigned long)candidate, (unsigned long)before);
                off->UField.Next = candidate;
                return true;
            }
            LOGW("[AutoFix] UField.Next fixup failed; keeping preset");
            return false;
        }
    }  // namespace

    bool RunFixup(IGameProfile *profile)
    {
        (void)profile;
        if (!Offsets() || !Objects() || !Vars())
        {
            LOGE("[AutoFix] RunFixup: UEWrappers not initialized");
            return false;
        }

        LOGI("[AutoFix] === RunFixup begin ===");
        ApplyDefaultsOnly();

        UE_Offsets *off = Offsets();
        UE_Offsets before = *off;

        const bool nameOk = FixupNamePrivate();
        // 后续修复全部依赖 NamePrivate 解码 FName，因此 nameOk 失败时保留 preset。
        // 这些修补**只写**会失败兜底的字段，不影响 SDK 浏览器其它路径。
        const bool classOk = nameOk ? FixupClassPrivate() : false;
        if (nameOk) FixupFNameSettings();
        if (nameOk) ApplyFNameDependentFieldFixups();
        if (nameOk) FixupObjectFlags();
        if (nameOk) FixupInternalIndex();
        if (nameOk) FixupOuterPrivate();
        const bool superOk = nameOk ? FixupSuperStruct() : false;
        if (nameOk) FixupStructSize();
        if (nameOk) FixupMinAlignment();
        if (nameOk) FixupClassDefaultObject();
        if (nameOk) FixupImplementedInterfaces();
        if (nameOk) Fixup_UClassCastFlags();
        if (nameOk) Fixup_UClassClassFlags();
        if (nameOk) FixupLevelActors();
        if (nameOk) FixupDataTableRowMap();
        if (nameOk) FixupChildren();
        // UField.Next 仅 UE<4.25 真正用，但即使 UE>=4.25 也可能需要它来遍历 UFunction 链；
        // 内部已根据 ChildProperties/Children 等情况自适应。
        if (nameOk) FixupUFieldNext();
        if (nameOk) FixupChildProperties();
        // FField 三件套：仅 IsUsingFNamePool && ChildProperties 有效时生效，
        // 顺序为 Name -> Class -> Next（Class 验证依赖 ChildProperties 链遍历，
        // Next 验证依赖 NamePrivate 已知，故先 Name）。
        if (nameOk) FixupFFieldName();
        if (nameOk) FixupFFieldClassName();
        if (nameOk) FixupFFieldNext();
        if (nameOk) FixupFFieldClass();
        if (nameOk) FixupFFieldOwner();
        if (nameOk) FixupFFieldClassSuperClass();
        if (nameOk) FixupFFieldClassCastFlags();
        if (nameOk) FixupFPropertyLayout();
        if (nameOk) FixupUEnumNames();
        if (nameOk) FixupUFunctionLayout();
        const bool funcOk = nameOk ? FixupUFunctionFunc() : false;

        if (nameOk)
        {
            AutoFixPropertyOffsets::Invalidate();
            AutoFixStructLayout::Invalidate();

            AutoFixPropertyOffsets::EnsureResolved();
            AutoFixStructLayout::Warmup();
            LOGI("[AutoFix] Property-specific offsets and struct layout cache warmed");
        }

        DumpAutoFoundOffsets(before, *off);

        const bool success = nameOk && classOk && superOk && funcOk;
        LOGI("[AutoFix] === RunFixup done (success=%d) ===", success ? 1 : 0);
        return success;
    }
}  // namespace AutoFix
