# L4 分析层 API · AutoFixArm64（Arm64 解码）

> **源码**：`src/AutoFix/Arm64Decode.hpp`（35 行）/ `Arm64Decode.cpp`（117 行）
> **定位**：AutoFix 内部用的**手写位操作解码器**，用于反推函数地址/虚表偏移。
> **已精读范围**：hpp 全文 + cpp 全文（1–117）。
> ⚠️ **与 `UEMemory::Arm64` 是两套不同解码器**（见 §2），MCP 封装时勿混淆。

---

## 0. 全局约定

1. **纯函数、无状态、无内存读写**：全部是 `uint32_t` 指令字的位运算，不依赖 `kMgr`/`kPtrValidator`。可安全并发调用。
2. **失败返回**：解码/识别失败返回 `false`（bool 类）或 `-1`（`GetBLR_Reg`/`GetBR_Reg`），不抛异常。
3. **`uint32_t insn` 是小端指令字**：调用方负责先从内存读出 `uint32_t`（通常 `vm_rpm_ptr<uint32_t>`）。
4. **指令集为 ARM64 AArch64 标准编码**，与 PC 侧 capstone 反汇编是互补关系，不是替代。

---

## 1. 接口清单

### 1.1 解码类

#### `uint64_t DecodeADRP(uint64_t pc, uint32_t insn)`　`:5-15`

ADRP 解码为绝对页地址：
```
immhi = (insn >> 5) & 0x7FFFF;  immlo = (insn >> 29) & 0x3;
imm = (immhi << 2) | immlo;     // 21 位立即数
if (imm & (1<<20)) imm |= ~((1<<21)-1);   // 符号扩展
base = pc & ~0xFFF;             // pc 页对齐
return base + (sign_extended_imm << 12);
```

⚠️ **`pc` 必须是 ADRP 指令自身的地址**，内部才做页对齐。传错 pc 结果直接错一个页。

#### `uint64_t DecodeADD(uint32_t insn)`　`:17-22`

```
imm12 = (insn >> 10) & 0xFFF;  shift = (insn >> 22) & 0x1;
return imm12 << (shift ? 12 : 0);
```

**只返回立即数部分**（已按 LSL #12 处理），**不含寄存器**。这是「ADD 的立即数解码」，不是「ADD 目标地址解码」——目标地址要调用方自己 `Xn + imm`。

#### `int64_t DecodeBranchOffset(uint32_t insn)`　`:110-116`

B/BL 的 26 位立即数，符号扩展后 `<< 2`，返回**字对齐的字节偏移**。

### 1.2 指令识别类（`Is*`，均返回 bool）

| 函数 | 掩码判定 | 识别 | 行号 |
|---|---|---|---|
| `IsADRP` | `(insn & 0x9F000000) == 0x90000000` | ADRP | `:24` |
| `IsADD` | `(insn & 0x7F800000) == 0x11000000` | ADD (immediate) | `:26` |
| `IsAddImm` | **同上（与 IsADD 完全相同）** | ADD (immediate) | `:28-31` |
| `IsLDR_Imm` | `(insn & 0xFFC00000) == 0xB9400000` 或 `== 0xF9400000` | LDR 32/64-bit unsigned imm | `:33-38` |
| `IsBL` | `(insn & 0xFC000000) == 0x94000000` | BL | `:40` |
| `IsB` | `(insn & 0xFC000000) == 0x14000000` | B | `:41` |
| `IsBLR` | `(insn & 0xFFFFFC1F) == 0xD63F0000` | BLR | `:42` |
| `IsBR` | `(insn & 0xFFFFFC1F) == 0xD61F0000` | BR | `:43` |
| `IsRET` | `(insn & 0xFFFFFC1F) == 0xD65F0000` | RET | `:44` |
| `IsMov_Reg` | `(insn & 0x7F2003E0) == 0x2A0003E0` | ORR(shifted) Rn=XZR = MOV reg | `:46-51` |
| `IsMovz` | `(insn & 0x7F800000) == 0x52800000` | MOVZ | `:53-57` |

⚠️ **`IsADD` 与 `IsAddImm` 是逐字节相同的重复实现**（`:26` 与 `:28-31`）。这是冗余，不是语义差异——MCP 侧认一个即可，但要知道两者都存在（调用方可能混用）。

### 1.3 解码带输出参数类（`Decode*`，bool 返回 + 出参）

| 函数 | 出参 | 失败返回 | 行号 |
|---|---|---|---|
| `DecodeLDR_Imm(insn, *Rn, *Rt, *Imm12, *Scale)` | 基址寄存器 / 目标寄存器 / 立即数 / 缩放 | `!IsLDR_Imm` → false | `:59-75` |
| `DecodeMovReg(insn, *Rd, *Rm)` | 目标 / 源寄存器 | `!IsMov_Reg` → false | `:77-84` |
| `DecodeAddImm(insn, *Rd, *Rn, *Imm)` | 目标 / 源 / 立即数 | `!IsAddImm` → false | `:86-96` |

**出参语义**：
- `DecodeLDR_Imm` 的 `Scale`：`size=10 → 4`（32-bit），`size=11 → 8`（64-bit），即 `(1u << size)`。**实际内存偏移 = Imm12 × Scale**。
- 三个函数**出参可为 `nullptr`**（内部判空跳过），MCP 侧调用可只取需要的字段。

### 1.4 寄存器号提取类

| 函数 | 失败 | 行号 |
|---|---|---|
| `GetBLR_Reg(insn)` | 非 BLR 返回 `-1` | `:98-102` |
| `GetBR_Reg(insn)` | 非 BR 返回 `-1` | `:104-108` |

---

## 2. ⚠️ 与 `UEMemory::Arm64` 的两套解码器（MCP 封装必读）

| | `AutoFixArm64`（本层） | `UEMemory::Arm64` |
|---|---|---|
| 位置 | `AutoFix/Arm64Decode.cpp` | `UE/UEMemory.cpp:83-216` |
| 底层 | **手写位操作** | 调用 `KittyArm64::decode_adr_imm` 等 |
| 输入 | 指令字 `uint32_t` | **指令地址 `uintptr_t`**（内部读内存） |
| 输出 | 立即数/页地址 | **绝对目标地址** |
| MCP 工具 | 间接（AutoFix 内部用） | `decodeAdrl`（E 组，✅ 已有） |

**`decodeAdrl` 工具对应的是 `UEMemory::Arm64::DecodeADRL`（带内存读取），不是本层。** 本层是 AutoFix 虚表/函数解析的内部依赖，MCP 若要暴露 arm64 解码，优先用 `UEMemory::Arm64`（已接好内存读取），本层只有"已拿到指令字"时才有价值。

---

## 3. MCP 封装陷阱清单

| # | 陷阱 | 后果 | 应对 |
|---|---|---|---|
| 1 | `DecodeADRP` 的 `pc` 必须是 ADRP 指令自身地址 | 传错页对齐基准，目标错一个页 | 调用方明确 pc 语义 |
| 2 | `DecodeADD` 只返回立即数，不含目标地址 | 误当绝对地址用 | 目标 = `Xn + DecodeADD(insn)` |
| 3 | `IsADD` ≡ `IsAddImm` 重复实现 | 两个名字一个行为，混用无碍但冗余 | 认一个即可 |
| 4 | `DecodeLDR_Imm` 的偏移要乘 `Scale` | 直接拿 Imm12 当字节偏移会错 4/8 倍 | 用 `Imm12 × Scale` |
| 5 | 本层无内存读取，指令字要调用方自己读 | 拿不到指令字就没法用 | 配套 `vm_rpm_ptr<uint32_t>` |
| 6 | 本层 ≠ `decodeAdrl` 工具底座 | 封装错工具 | `decodeAdrl` 用 `UEMemory::Arm64` |

---

## 4. 本次未精读范围

无。hpp + cpp 已全文精读。
