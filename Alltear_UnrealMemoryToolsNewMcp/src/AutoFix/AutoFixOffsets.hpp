#pragma once

class IGameProfile;

namespace AutoFix
{
    // Validate and patch UE_Offsets on the given profile based on real
    // memory layout of the running game. Should be called AFTER
    // IGameProfile::InitUEVars() succeeded (so GUObject + GNames work).
    //
    // Safe fallback: if any anchor cannot be located, leaves the field
    // at its preset (version-derived) value and continues.
    bool RunFixup(IGameProfile *profile);
}
