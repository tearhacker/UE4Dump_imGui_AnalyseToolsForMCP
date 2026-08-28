#pragma once

#include <cstdint>
#include <vector>

#include "../UE/UEWrappers.hpp"

namespace AutoFixStructLayout
{
struct StructLayoutInfo
{
    int32_t LastMemberEnd = 0;
    int32_t Size = 0;
    int32_t AlignedSize = 0;
    int32_t Alignment = 1;
    bool UseExplicitAlignment = false;
    bool HasReusedTrailingPadding = false;
    bool IsFinal = true;
};

struct FieldLayoutInfo
{
    UE_FProperty Field;
    int32_t Offset = 0;
    int32_t ElementSize = 0;
    int32_t ArrayDim = 1;
    int32_t TotalSize = 0;
    bool IsBitfield = false;
    uint8_t BitIndex = 0xFF;
    uint8_t FieldMask = 0xFF;
};

const StructLayoutInfo &GetStructLayoutInfo(const UE_UStruct &strct);
std::vector<FieldLayoutInfo> GetSortedFields(const UE_UStruct &strct);
int32_t GetOwnMemberStart(const UE_UStruct &strct);
int32_t GetDisplayStructSize(const UE_UStruct &strct);
void Warmup();
void Invalidate();
}
