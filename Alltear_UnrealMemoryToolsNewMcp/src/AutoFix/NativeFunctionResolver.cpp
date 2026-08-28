#include "NativeFunctionResolver.hpp"

#include <unordered_map>
#include <vector>

#include "../UE/UEMemory.hpp"
#include "../UE/UEWrappers.hpp"
#include "VirtualFunctionResolver.hpp"

using namespace UEMemory;

namespace AutoFixNativeFunctions
{
    namespace
    {
        struct ClassObjectCandidate
        {
            uintptr_t Object = 0;
            bool IsDefaultObject = false;
        };

        uint32_t gCachedGeneration = 0;
        uintptr_t gDiscoveredDefaultObjectOffset = 0;
        std::unordered_map<uintptr_t, uintptr_t> gScannedDefaultObjectCache;
        std::unordered_map<uintptr_t, ClassObjectCandidate> gClassObjectCache;
        std::unordered_map<uintptr_t, std::unordered_map<uintptr_t, bool>> gInterfaceCheckCache;
        std::unordered_map<uintptr_t, std::vector<uintptr_t>> gInterfaceImplementerCache;
        std::unordered_map<uintptr_t, std::unordered_map<uintptr_t, uintptr_t>> gResolvedInterfaceVTableCache;

        void ResetCachesIfNeeded()
        {
            const uint32_t generation = UEWrappers::GetInitGeneration();
            if (gCachedGeneration == generation)
                return;

            gCachedGeneration = generation;
            gDiscoveredDefaultObjectOffset = 0;
            gScannedDefaultObjectCache.clear();
            gClassObjectCache.clear();
            gInterfaceCheckCache.clear();
            gInterfaceImplementerCache.clear();
            gResolvedInterfaceVTableCache.clear();
        }

        bool IsLikelyPointer(uintptr_t address)
        {
            return address >= 0x10000;
        }

        bool IsModuleAddress(uintptr_t address)
        {
            const uintptr_t baseAddr = UEWrappers::GetBaseAddress();
            return baseAddr && address >= baseAddr;
        }

        bool IsObjectOfClass(uintptr_t objectAddr, uintptr_t classAddr)
        {
            UE_Offsets *off = UEWrappers::GetOffsets();
            if (!off || !off->UObject.ClassPrivate)
                return false;

            return IsLikelyPointer(objectAddr) &&
                   IsLikelyPointer(classAddr) &&
                   vm_rpm_ptr<uintptr_t>((const void *)(objectAddr + off->UObject.ClassPrivate)) == classAddr;
        }

        bool HasNativeVTable(uintptr_t objectAddr)
        {
            if (!IsLikelyPointer(objectAddr))
                return false;

            const uintptr_t vtable = vm_rpm_ptr<uintptr_t>((const void *)objectAddr);
            return IsLikelyPointer(vtable) && IsModuleAddress(vtable);
        }

        bool IsLikelyDefaultObject(uintptr_t objectAddr, uintptr_t classAddr)
        {
            UE_Offsets *off = UEWrappers::GetOffsets();
            if (!off || !off->UObject.OuterPrivate)
                return false;
            if (!IsObjectOfClass(objectAddr, classAddr))
                return false;

            if (vm_rpm_ptr<uintptr_t>((const void *)(objectAddr + off->UObject.OuterPrivate)) == classAddr)
                return true;

            const UE_UObject object(reinterpret_cast<uint8_t *>(objectAddr));
            const std::string name = object.GetName();
            return name.rfind("Default__", 0) == 0;
        }

        uintptr_t FindDefaultObjectByScanningClass(uintptr_t classAddr)
        {
            ResetCachesIfNeeded();

            auto it = gScannedDefaultObjectCache.find(classAddr);
            if (it != gScannedDefaultObjectCache.end())
                return it->second;

            if (!IsLikelyPointer(classAddr))
            {
                gScannedDefaultObjectCache[classAddr] = 0;
                return 0;
            }

            auto readCandidate = [&](uintptr_t offset) -> uintptr_t
            {
                const uintptr_t objectAddr = vm_rpm_ptr<uintptr_t>((const void *)(classAddr + offset));
                if (!IsObjectOfClass(objectAddr, classAddr) || !HasNativeVTable(objectAddr))
                    return 0;
                return objectAddr;
            };

            if (gDiscoveredDefaultObjectOffset)
            {
                const uintptr_t objectAddr = readCandidate(gDiscoveredDefaultObjectOffset);
                if (objectAddr && IsLikelyDefaultObject(objectAddr, classAddr))
                {
                    gScannedDefaultObjectCache[classAddr] = objectAddr;
                    return objectAddr;
                }
            }

            uintptr_t firstObject = 0;
            for (uintptr_t offset = 0x40; offset < 0x400; offset += sizeof(uintptr_t))
            {
                const uintptr_t objectAddr = readCandidate(offset);
                if (!objectAddr)
                    continue;

                if (!firstObject)
                    firstObject = objectAddr;

                if (IsLikelyDefaultObject(objectAddr, classAddr))
                {
                    gDiscoveredDefaultObjectOffset = offset;
                    gScannedDefaultObjectCache[classAddr] = objectAddr;
                    return objectAddr;
                }
            }

            gScannedDefaultObjectCache[classAddr] = firstObject;
            return firstObject;
        }

        void BuildClassObjectCache()
        {
            ResetCachesIfNeeded();
            if (!gClassObjectCache.empty() || !UEWrappers::GetObjects())
                return;

            UEWrappers::GetObjects()->ForEachObject([&](UE_UObject object)
            {
                const uintptr_t objectAddr = reinterpret_cast<uintptr_t>(object.GetAddress());
                if (!IsLikelyPointer(objectAddr))
                    return false;

                const uintptr_t classAddr = reinterpret_cast<uintptr_t>(object.GetClass().GetAddress());
                if (!IsLikelyPointer(classAddr))
                    return false;

                const bool isDefaultObject = object.HasFlags(EObjectFlags::ClassDefaultObject);
                auto it = gClassObjectCache.find(classAddr);
                if (it == gClassObjectCache.end() || (isDefaultObject && !it->second.IsDefaultObject))
                    gClassObjectCache[classAddr] = {objectAddr, isDefaultObject};

                return false;
            });
        }

        uintptr_t FindContextObjectForClass(uintptr_t classAddr)
        {
            ResetCachesIfNeeded();

            if (!IsLikelyPointer(classAddr))
                return 0;

            const uintptr_t scannedCdo = FindDefaultObjectByScanningClass(classAddr);
            if (scannedCdo)
                return scannedCdo;

            BuildClassObjectCache();
            auto it = gClassObjectCache.find(classAddr);
            if (it != gClassObjectCache.end() &&
                IsObjectOfClass(it->second.Object, classAddr) &&
                HasNativeVTable(it->second.Object))
            {
                return it->second.Object;
            }

            return 0;
        }

        bool IsInterfaceClass(uintptr_t classAddr)
        {
            ResetCachesIfNeeded();

            const uintptr_t interfaceClassAddr = reinterpret_cast<uintptr_t>(UE_UInterface::StaticClass().GetAddress());
            if (!IsLikelyPointer(classAddr) || !IsLikelyPointer(interfaceClassAddr))
                return false;

            std::unordered_map<uintptr_t, bool> seen;
            for (UE_UStruct current(reinterpret_cast<uint8_t *>(classAddr));
                 current && seen.emplace(reinterpret_cast<uintptr_t>(current.GetAddress()), true).second;
                 current = current.GetSuper())
            {
                if (reinterpret_cast<uintptr_t>(current.GetAddress()) == interfaceClassAddr)
                    return true;
            }

            return false;
        }

        bool IsPlausibleArrayCount(int32_t count, int32_t max)
        {
            return count > 0 && count <= max && max <= 256;
        }

        bool IsReasonableInterfacePointerOffset(int32_t offset)
        {
            return offset > -0x100000 && offset < 0x100000;
        }

        bool ArrayContainsInterfaceClass(uintptr_t data, int32_t count, uintptr_t interfaceClassAddr)
        {
            if (!IsLikelyPointer(data) || count <= 0 || count > 256)
                return false;

            const uintptr_t candidateEntrySizes[] = {0x10, 0x18, 0x20};
            for (uintptr_t entrySize : candidateEntrySizes)
            {
                for (int32_t i = 0; i < count; ++i)
                {
                    const uintptr_t entry = data + entrySize * i;
                    const int32_t pointerOffset = vm_rpm_ptr<int32_t>((const void *)(entry + sizeof(uintptr_t)));
                    const uint8_t implementedByK2 = vm_rpm_ptr<uint8_t>((const void *)(entry + sizeof(uintptr_t) + sizeof(int32_t)));
                    if (vm_rpm_ptr<uintptr_t>((const void *)entry) == interfaceClassAddr &&
                        IsReasonableInterfacePointerOffset(pointerOffset) &&
                        implementedByK2 <= 1)
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        bool ClassMayImplementInterface(uintptr_t classAddr, uintptr_t interfaceClassAddr)
        {
            ResetCachesIfNeeded();

            if (!IsLikelyPointer(classAddr) || !IsLikelyPointer(interfaceClassAddr) || classAddr == interfaceClassAddr)
                return false;

            auto &perInterfaceCache = gInterfaceCheckCache[interfaceClassAddr];
            auto cached = perInterfaceCache.find(classAddr);
            if (cached != perInterfaceCache.end())
                return cached->second;

            bool found = false;
            for (uintptr_t offset = 0x40; offset < 0x500; offset += sizeof(uintptr_t))
            {
                const uintptr_t value = vm_rpm_ptr<uintptr_t>((const void *)(classAddr + offset));
                const int32_t count = vm_rpm_ptr<int32_t>((const void *)(classAddr + offset + sizeof(uintptr_t)));
                const int32_t max = vm_rpm_ptr<int32_t>((const void *)(classAddr + offset + sizeof(uintptr_t) + sizeof(int32_t)));
                if (IsPlausibleArrayCount(count, max) && ArrayContainsInterfaceClass(value, count, interfaceClassAddr))
                {
                    found = true;
                    break;
                }
            }

            perInterfaceCache[classAddr] = found;
            return found;
        }

        const std::vector<uintptr_t> &GetInterfaceImplementerClasses(uintptr_t interfaceClassAddr)
        {
            ResetCachesIfNeeded();

            auto cached = gInterfaceImplementerCache.find(interfaceClassAddr);
            if (cached != gInterfaceImplementerCache.end())
                return cached->second;

            std::vector<uintptr_t> classes;
            if (!IsInterfaceClass(interfaceClassAddr) || !UEWrappers::GetObjects())
            {
                auto inserted = gInterfaceImplementerCache.emplace(interfaceClassAddr, std::move(classes));
                return inserted.first->second;
            }

            const UE_UClass classClass = UE_UClass::StaticClass();
            UEWrappers::GetObjects()->ForEachObject([&](UE_UObject object)
            {
                if (!object || !object.IsA(classClass))
                    return false;

                const uintptr_t classAddr = reinterpret_cast<uintptr_t>(object.GetAddress());
                if (IsLikelyPointer(classAddr) && ClassMayImplementInterface(classAddr, interfaceClassAddr))
                    classes.push_back(classAddr);

                return false;
            });

            auto inserted = gInterfaceImplementerCache.emplace(interfaceClassAddr, std::move(classes));
            return inserted.first->second;
        }

        uintptr_t ResolveInterfaceVTableFunction(uintptr_t interfaceClassAddr, uintptr_t vtableOffset)
        {
            ResetCachesIfNeeded();

            auto &perInterfaceCache = gResolvedInterfaceVTableCache[interfaceClassAddr];
            auto cached = perInterfaceCache.find(vtableOffset);
            if (cached != perInterfaceCache.end())
                return cached->second;

            uintptr_t realAddress = 0;
            for (uintptr_t implementerClassAddr : GetInterfaceImplementerClasses(interfaceClassAddr))
            {
                const uintptr_t contextObject = FindContextObjectForClass(implementerClassAddr);
                if (!contextObject)
                    continue;

                const uintptr_t candidate = AutoFixVTable::ResolveVTableFunction(contextObject, vtableOffset);
                if (IsModuleAddress(candidate))
                {
                    realAddress = candidate;
                    break;
                }
            }

            perInterfaceCache[vtableOffset] = realAddress;
            return realAddress;
        }
    }  // namespace

    NativeFunctionExportInfo ResolveNativeFunctionInfo(uintptr_t ownerClassAddr, uintptr_t funcPtr, uint32_t funcFlags)
    {
        (void)funcFlags;
        ResetCachesIfNeeded();

        NativeFunctionExportInfo info{};
        const uintptr_t baseAddr = UEWrappers::GetBaseAddress();
        if (!funcPtr || !baseAddr)
            return info;

        if (funcPtr > baseAddr)
            info.FuncOffset = funcPtr - baseAddr;

        if (funcPtr <= baseAddr || (funcPtr % sizeof(uint32_t)) != 0)
            return info;

        info.VTableOffset = AutoFixVTable::FindVTableCallOffset(funcPtr);
        if (!info.VTableOffset)
        {
            const uintptr_t directTarget = AutoFixVTable::FindDirectBranchCallTarget(funcPtr);
            if (directTarget && directTarget != funcPtr && directTarget > baseAddr)
                info.RealOffset = directTarget - baseAddr;
            else
                info.RealOffset = info.FuncOffset;
            return info;
        }

        const uintptr_t contextObject = FindContextObjectForClass(ownerClassAddr);
        uintptr_t realAddress = contextObject ? AutoFixVTable::ResolveVTableFunction(contextObject, info.VTableOffset) : 0;

        if (!IsModuleAddress(realAddress))
            realAddress = ResolveInterfaceVTableFunction(ownerClassAddr, info.VTableOffset);

        if (realAddress > baseAddr)
            info.RealOffset = realAddress - baseAddr;

        return info;
    }
}  // namespace AutoFixNativeFunctions
