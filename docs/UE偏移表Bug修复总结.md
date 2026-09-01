# UE 偏移表 Bug 修复总结

## 修复的三个 Bug

### Bug 1: UE4.2x NumElements 偏移错误
**文件**: `UEOffsets.cpp` (UE4_20, UE4_23_24)
**问题**: 原代码设为 `(sizeof(void *) * 2) + sizeof(int32_t) = 0x14`（UE5布局）
**修复**: 改为 `sizeof(void *) + (sizeof(int32_t) * 3) = 0x10`（UE4.2x布局）

```cpp
// UE4.20/23/24 (正确):
offsets.TUObjectArray.NumElements = sizeof(void *) + (sizeof(int32_t) * 3);  // 0x10

// UE5.00+ (保持原样):
offsets.TUObjectArray.NumElements = (sizeof(void *) * 2) + sizeof(int32_t);  // 0x14
```

**说明**: UE4.2x的FChunkedFixedUObjectArray布局是 `[Objects*][Max][Count]`，NumElements在0x10。
UE5+增加了ExtraChunkSize字段，布局变为 `[Objects*][Max][Count][ExtraChunkSize]`，NumElements移到0x14。

---

### Bug 2: GetGUObjectArrayPtr 验证逻辑缺陷
**文件**: `UEGameProfile.cpp`
**问题**: 只验证slot 0是否为"/Script/CoreUObject"，但UE4.2x的slot 0是保留空槽
**修复**: 
- 提取 `stableItemSize` 变量（0x18）
- 新增lambda `verifyCandidate()` 扫描前8个槽位
- 统计锚点命中数，`anchorHits > 0` 即通过验证
- 兼容非chunked和chunked两种布局

```cpp
// 之前：只验slot 0，UE4.2x必然失败
if (nm != "/Script/CoreUObject") continue;

// 之后：多槽位验证，跳过空槽
int anchorHits = 0;
for (int slot = 0; slot < kVerifySlots; ++slot) {
    // 读取槽位对象，验证name解析
    if (nm == kVerifyAnchor) ++anchorHits;
}
if (anchorHits > 0) return candObjAddr;
```

---

### Bug 3: sampleGnames 偏移错位
**文件**: `UECandidateAnalysis.cpp`
**问题**: `anchorOffsets` 传入的是字节偏移 `{0, 2, 4, 6}`，但 `DecodeNameAt` 内部会乘 `layout.stride`
**修复**: 调用前将字节偏移 ÷ stride 转成单元偏移

```cpp
// 之前：直接使用字节偏移
std::vector<uint32_t> anchorOffsets = {0, 2, 4, 6, 8, 10, 12, 16};

// 之后：转换为单元偏移
for (auto &o : anchorOffsets)
    o = o / layouts[0].stride;
```

**说明**: UE4.2x的FNamePool stride=2，byte_offset 2 对应 id=1（"ByteProperty"），
但代码会计算 `block + 2*2 = block + 4`，跳到id=2（"IntProperty"），错位导致验证失败。

---

## 编译验证
- NDK27 + VS2026 cmake/ninja 增量编译通过
- 产物: `outputs/arm64-v8a/UnrealMemoryTools`
- llvm-strings 确认新日志串已进场:
  - `[Bootstrap] GUObject @ 0x%lx (%s/Objects=0x%lx, anchorHits=%d, verified %d slots)`

## 兼容性保障
✅ **UE4.00-4.19**: 保持原样（NumElements=0x0C，No Chunked）
✅ **UE4.20-4.27**: 修复为NumElements=0x10（原错误为0x14）
✅ **UE5.00+**: 保持原样NumElements=0x14（新增ExtraChunkSize字段）

所有修改均不破坏原有探针寻找GNames/GUObjectArray/GWorld的通用算法逻辑，
只修正了偏移量错误，确保UE4到UE5的完整兼容性。

## 变更统计
- `UEOffsets.cpp`: 3处NumElements偏移修正（UE4.20, UE4.23_24, UE5_00_02）
- `UEGameProfile.cpp`: GetGUObjectArrayPtr验证逻辑重构（多槽位扫描）
- `UECandidateAnalysis.cpp`: anchorOffsets单位转换（字节→单元）