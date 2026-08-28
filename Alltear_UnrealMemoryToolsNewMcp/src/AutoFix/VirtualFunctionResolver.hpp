#pragma once

#include <cstdint>

namespace AutoFixVTable
{
    // Returns the slot offset (in bytes from vtable base) of the indirect call
    // inside an exec wrapper at functionAddress. 0 = not found.
    uintptr_t FindVTableCallOffset(uintptr_t functionAddress);

    // Returns the absolute target of the first direct B/BL inside the wrapper
    // that does not branch back to itself. 0 = not found.
    uintptr_t FindDirectBranchCallTarget(uintptr_t functionAddress);

    // Convert a slot offset to vtable index (slot / 8).
    inline int OffsetToIndex(uintptr_t off)
    {
        return (off && (off % 8) == 0) ? (int)(off / 8) : -1;
    }

    // Reads the vtable function pointer from objectAddress at the given slot.
    uintptr_t ResolveVTableFunction(uintptr_t objectAddress, uintptr_t vtableOffset);
}
