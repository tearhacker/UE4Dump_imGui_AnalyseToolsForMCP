# L1 原语层 API · UEMemory（内存读写 / 指针搜索 / 解码）

> **源码**：`src/UE/UEMemory.hpp`（154 行）/ `UEMemory.cpp`（352 行）
> **定位**：最底层内存原语。D 组工具（`readMemory`/`writeMemory`/`readMemoryValue`/`readString`/`scanPattern`）与 E 组 `decodeAdrl` 的直接底座。
> **已精读范围**：hpp 全文 + cpp 全文（1–352）。

---

## 0. 全局约定

### 0.1 两个进程级单例（`UEMemory.cpp:8-9`）

```cpp
KittyMemoryMgr kMgr{};           // 内存读写/ELF 扫描/转储的入口
KittyPtrValidator kPtrValidator; // 指针可读性校验（读前先查）
```

`vm_rpm_*` 全部依赖这两个单例。→ MCP 命令服务必须**串行**调用（与 L2/L3 同约束）。

### 0.2 空值语义

| 接口 | 失败返回 |
|---|---|
| `vm_rpm_ptr`（bool 版） | `false` |
| `vm_rpm_ptr<T>`（模板版） | `T{}`（值初始化零值，**不抛异常**） |
| `vm_rpm_str` / `vm_rpm_strw` | `""` / `L""`（空串，读失败与读到空串**无法区分**） |
| `FindAlignedPointerRefrence` | `0` |
| `Arm64::DecodeADRL` 等三个 | `0` |

### 0.3 模板 `vm_rpm_ptr<T>` 是读值主路径

```cpp
template <typename T> T vm_rpm_ptr(const void *address) {
    T buffer{};                              // 值初始化
    vm_rpm_ptr(address, &buffer, sizeof(T)); // 忽略 bool 返回
    return buffer;
}
```

⚠️ **读失败时返回 `T{}` 零值，调用方无法区分"读到 0"与"读失败"**。需要失败感知的场景（如读对象数组）必须改用 bool 版 `vm_rpm_ptr(address, &out, len)`。

---

## 1. 内存读写

### `bool vm_rpm_ptr(const void *address, void *result, size_t len)`　`:11-17`

```cpp
if (!kPtrValidator.isPtrReadable(address)) return false;
return kMgr.readMem(uintptr_t(address), result, len) == len;
```

**双重校验**：先 `isPtrReadable`（映射区间 + 可读性），再 `readMem` 且要求**读满 len 字节**（部分读取也算失败）。

### `std::string vm_rpm_str(const void *address, size_t max_len = 1024)`　`:19-41`

- `vm_rpm_ptr` 读 `max_len` 字节失败 → `""`
- 逐字节复制直到遇到 `'\0'` 停止
- 首字节就是 `'\0'` → 循环没 push 任何字符，返回空串 `""`
- ⚠️ **`:37-38` 的 `if ((int)str[0] == 0 && str.size() == 1) return "";` 是死代码**：因为循环在 `'\0'` 处 break，`str` 内不可能含 `'\0'`，而 `str` 为空时 `size()==0` 不满足 `==1`。该判断恒 false（上游复制来的冗余防御）。

### `std::wstring vm_rpm_strw(const void *address, size_t max_len = 1024)`　`:43-65`

- 读 `max_len * 2` 字节（wchar_t 2 字节，**正确**）
- 返回 `std::wstring`（**不是 string**，FName 可能是宽字符）
- 同样的死代码判断在 `:61-62`

---

## 2. 指针搜索与解码

### `uintptr_t FindAlignedPointerRefrence(uintptr_t start, size_t range, uintptr_t ptr)`　`:67-81`

在 `[start, start+range)` 内**逐指针步进**找「值 == ptr」的地址。

**严格前置校验**（任一不满足返回 0）：
- `start != 0` 且 `start` 指针对齐
- `range >= sizeof(void*)` 且 `range` 指针对齐

用途：反查"谁指向某个全局指针"（定位引擎全局用）。⚠️ 是线性扫描，range 大时慢。

### `Arm64` 命名空间（`:83-216`）—— 三个绝对地址解码

> 三者都**内部读内存**（`vm_rpm_ptr<uint32_t>`），输入是**指令地址**不是指令字。

#### `uintptr_t DecodeADRL(uintptr_t adrp_address, uint32_t imm_insn_offset = 0)`　`:132-168`

ADRP/ADR + 后续立即数指令，解出绝对地址：
- `adrp_address == 0` → `0`
- 读 adrp 指令，`decode_adrl_target` 解页地址（ADRP 页对齐 / ADR 不页对齐）
- `imm_insn_offset == 0`：**扫描后续 8 条指令**（`:148-154`），找第一条「基址寄存器 == adrp 的目标寄存器」的立即数指令，返回 `adrp_target + imm`
- `imm_insn_offset != 0`：直接读 `adrp_address + offset` 处的指令，`decode_adrl_imm` 解立即数

#### `uintptr_t Decode_ADRP_ADD(uintptr_t adrp_address, uint32_t add_offset = 4)`　`:170-190`

```
page_off + adrp_pc_rel + add_imm12
```
- ⚠️ **不校验 `add_offset` 处是否真的是 ADD**：直接 `decode_addsub_imm` 解立即数。若偏移处不是 ADD，解出的是垃圾值。调用方必须保证 add_offset 正确。

#### `uintptr_t Decode_ADRP_LDR(uintptr_t adrp_address, uint32_t ldr_offset = 4)`　`:192-213`

```
page_off + adrp_pc_rel + ldr_imm12
```
- 这个**有校验**：`decode_ldrstr_uimm` 失败返回 0（`:209-210`），比 ADD 版严谨。

---

## 3. 工具函数与宏

| 项 | 签名/定义 | 作用 | 位置 |
|---|---|---|---|
| 页对齐宏 | `kINSN_PAGE_OFFSET(x)` = `(x) & ~0xFFF` | 指令地址取页基址 | hpp `:11` |
| 类型最大值 | `GetMaxOfType<T>()` = `(1ull << sizeof(T)*8) - 1` | 整型上限 | hpp `:31-35` |
| 指针对齐 | `GetPtrAlignedOf(p)` | 向上取指针对齐 | hpp `:37-40` |
| 枚举位掩码 | `kDEFINE_ENUM_BITMASK_OPERATORS(ENUM)` | 为枚举生成 `& \| ^ ~` 位运算 | hpp `:114-154` |

`EnumBitmask` 宏（hpp `:84-154`）是第三方 CC0 库，给 `EObjectFlags` 等枚举加位运算。MCP 侧若在 Python 复刻 flags 判断，注意是**位掩码语义**（`HasFlags` 用 `&`）。

---

## 4. `IOUtils` 文件工具（`UEMemory.cpp:220-352`）

全部是**设备端本地文件系统**操作，MCP 的 `listOutputFiles`/`readOutputFile` 间接依赖（读 `/sdcard/UnrealMemoryTools/` 产物）。

| 接口 | 签名 | 行为 | 行号 |
|---|---|---|---|
| 取文件名 | `std::string get_filename(const std::string &filePath)` | 无 `/` 或 `\` 时返回**空串** | `:222-231` |
| 取目录 | `std::string get_file_directory(const std::string &filePath)` | 无分隔符返回空串 | `:233-242` |
| 取扩展名 | `std::string get_file_extension(const std::string &filePath)` | 用 `find_last_of(".")`；**路径含点目录会取错** | `:244-253` |
| 路径包含 | `bool file_path_contains(const std::string &filePath, const std::string &subPath)` | `find != npos` | `:255-258` |
| 去特殊字符 | `std::string remove_specials(std::string s)` | 删除非字母数字下划线（就地 erase） | `:260-277` |
| 替换特殊字符 | `std::string replace_specials(std::string s, char c)` | 非字母数字下划线替换为 c | `:279-295` |
| 删目录 | `void delete_directory(const std::string &directory)` | 递归删除，目录不存在静默返回 | `:297-320` |
| 判目录 | `int path_is_directory(const std::string &path)` | `stat` 失败返回 0 | `:322-330` |
| 建目录 | `int mkdir_recursive(const std::string &dirPath, mode_t mode)` | 逐级 mkdir，`EEXIST` 跳过；失败返回 -1 | `:332-351` |

### 陷阱

1. **`delete_directory` 是递归 `rm -rf`**：`UEDumper::Dump` 转储前会先删输出目录（`/sdcard/UnrealMemoryTools/<pkg>/`）。MCP 侧 `dumpSDK` 若触发，会**清空上次产物**，必须在工具描述里提示。
2. **`get_file_extension` 用 `find_last_of(".")` 而非 `rfind(".")` 语义**：`"/sdcard/a.b/file"` 会返回 `"b/file"` 而非空。文件名里含点（如 `libUE4.so`）正确，但目录含点会误判。
3. **`remove_specials`/`replace_specials` 就地修改**：用于把包名/类名转成合法文件名，MCP 侧生成文件名时留意相同规则。

---

## 5. MCP 封装陷阱清单

| # | 陷阱 | 后果 | 应对 |
|---|---|---|---|
| 1 | `vm_rpm_ptr<T>` 读失败返回 `T{}` 零值 | 无法区分"读到 0"与"读失败" | 需失败感知用 bool 版 |
| 2 | `vm_rpm_str` 读失败与空串都返回 `""` | 语义混淆 | 结合 `isPtrReadable` 前置判断 |
| 3 | `vm_rpm_strw` 返回 `std::wstring` | 当 string 用编译错 | FName 宽字符场景显式转 UTF-8 |
| 4 | `Decode_ADRP_ADD` 不校验目标指令 | 偏移错解出垃圾地址 | 调用方保证 add_offset 正确 |
| 5 | `FindAlignedPointerRefrence` 线性扫描 | range 大时慢 | 标注慢操作，限制 range |
| 6 | `delete_directory` 递归删除 | dumpSDK 清空上次产物 | 工具描述明示 |
| 7 | `get_file_extension` 目录含点误判 | 文件名解析错 | 用 `get_filename` 后再取扩展名 |
| 8 | 全层依赖 `kMgr`/`kPtrValidator` 单例 | 并发数据竞争 | 命令服务串行化 |

---

## 6. 本次未精读范围

无。hpp + cpp 已全文精读。
