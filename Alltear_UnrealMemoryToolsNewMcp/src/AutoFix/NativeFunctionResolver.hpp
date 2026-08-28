#pragma once

#include <cstdint>

namespace AutoFixNativeFunctions
{
    struct NativeFunctionExportInfo
    {
        uintptr_t FuncOffset = 0;
        uintptr_t VTableOffset = 0;
        uintptr_t RealOffset = 0;
    };

    NativeFunctionExportInfo ResolveNativeFunctionInfo(uintptr_t ownerClassAddr, uintptr_t funcPtr, uint32_t funcFlags);
}
