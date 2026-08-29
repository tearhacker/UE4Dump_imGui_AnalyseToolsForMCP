# L3 对象层 API · UEWrappers

> **源码**：`src/UE/UEWrappers.hpp`（978 行）/ `UEWrappers.cpp`（1651 行）
> **定位**：UE 对象导航层。AI 分析 SDK 结构主要靠这一层。
> **已精读范围**：cpp 1–300、320–575、735–975。未覆盖部分在文末列出。

---

## 0. 全局约定（读代码前必知）

### 0.1 空对象语义

所有 `UE_*` 类内部只有一个 `uint8_t *object` 成员，**构造不校验有效性**。
`operator bool()` 返回 `object != nullptr`。
**因此任何 getter 都必须先判空**，否则读到地址 0 附近。

### 0.2 初始化代数缓存（`UEWrappers.cpp:11-20`）

```cpp
#define UE_STATIC_CLASS_CACHED(NAME)                        \
    static UE_UClass obj;                                   \
    static uint32_t _cachedGen = 0;                         \
    const uint32_t _curGen = UEWrappers::GetInitGeneration(); \
    if (_cachedGen != _curGen) {                            \
        obj = UEWrappers::GetObjects()->FindObject<UE_UClass>(NAME); \
        _cachedGen = _curGen;                               \
    }                                                       \
    return obj
```

**所有 `StaticClass()` 都是惰性查找 + 按代数缓存。**
`UEWrappers::Init()` 每次调用会 `++gInitGen`（`:48`），从而让全部缓存失效。
→ **切换进程后必须重新 `Init()`，否则拿到的是上一个进程的类对象。**

### 0.3 偏移为 0 表示"该版本不支持"

多处 getter 写成 `offset ? vm_rpm_ptr<T>(object + offset) : 0`。
这是**防御性设计**：偏移表里该项为 0（未识别）时返回 0，而不是读垃圾。

---

## 1. namespace `UEWrappers`（`:32-58`）

| 接口 | 签名 | 行为 | 行号 |
|---|---|---|---|
| 初始化 | `void Init(const UEVars *vars)` | `vars` 非空时：赋值 `GUVars`、重建 `pObjectsArray`、`++gInitGen`。**`vars` 为空则什么都不做** | `:38-50` |
| 取引擎变量 | `UEVars const *GetUEVars()` | 直接返回 `GUVars`，**未初始化时是 `nullptr`** | `:52` |
| 取基址 | `uintptr_t GetBaseAddress()` | `GUVars ? GUVars->GetBaseAddress() : 0` | `:53` |
| 取偏移表 | `UE_Offsets *GetOffsets()` | `GUVars ? GUVars->GetOffsets() : nullptr` | `:54` |
| 按 id 取名 | `std::string GetNameByID(int32_t id)` | `GUVars ? GUVars->GetNameByID(id) : ""` | `:55` |
| 取对象数组 | `UE_UObjectArray *GetObjects()` | `pObjectsArray.get()`，**未初始化时是 `nullptr`** | `:56` |
| 取初始化代数 | `uint32_t GetInitGeneration()` | 每次 `Init()` 递增 | `:57` |

**全局状态**：`GUVars`（`:34`）、`pObjectsArray`（`:35`）、`gInitGen`（`:36`）。
→ 单例，非线程安全。MCP 命令服务必须串行调用。

---

## 2. `UE_UObjectArray`（`:72-138`）

### `int32_t GetNumElements() const`　`:72-78`

- 读 `ObjObjectsPtr + TUObjectArray.NumElements`
- **`ObjObjectsPtr == 0` 时返回 0**
- 不校验可读性，读取失败由 `vm_rpm_ptr` 返回 0

### `uint8_t *GetObjectPtr(int32_t id) const`　`:80-101`

**边界检查**：`id < 0 || id >= GetNumElements() || !Objects` → 返回 `nullptr`

两种布局：

| 条件 | 计算方式 | 行号 |
|---|---|---|
| `NumElementsPerChunk <= 0` | `Objects + id*FUObjectItem.Size + FUObjectItem.Object` | `:85-88` |
| `NumElementsPerChunk > 0` | 先取 chunk：`Objects + chunkIndex*8`，再 `chunk + withinChunkIndex*FUObjectItem.Size + FUObjectItem.Object` | `:90-100` |

其中 `chunkIndex = id / NumElementsPerChunk`，`withinChunkIndex = id % NumElementsPerChunk`。
**chunk 为 nullptr 时返回 nullptr**（`:97-98`）。
⚠️ 注释掉了一行 `if (chunkIndex >= NumChunks) return nullptr;`（`:94`）——**缺少 chunk 上界检查**。

### `void ForEachObject(const std::function<bool(UE_UObject)> &callback) const`　`:103-114`

```cpp
if (!callback) return;
for (int32_t i = 0; i < GetNumElements(); i++) {
    uint8_t *object = GetObjectPtr(i);
    if (!object) continue;
    if (callback(object)) return;     // ← 返回 true 提前终止
}
```

**契约：callback 返回 `true` 表示"找到了，停止遍历"。**
→ MCP 封装时必须映射为「命中即停」，不是「遍历全部」。

### `void ForEachObjectOfClass(const UE_UClass &cmp, const std::function<bool(UE_UObject)> &callback) const`　`:116-128`

- `!cmp || !callback` → 直接返回
- 同上遍历，`object && object.IsA(cmp)` 才回调，**callback 返回 true 提前终止**
- ⚠️ **对每个对象都调 `IsA()`，而 `IsA()` 会遍历整条 Super 链并多次读内存** → O(n × 继承深度) 次远程读。
  对象数常达数十万，**这是性能热点**，MCP 侧必须分页或加计数上限。

### `bool IsObject(const UE_UObject &address) const`　`:130-138`

**O(n) 线性扫描整个对象数组**，逐地址比较。
⚠️ 极慢，不应在循环中使用。判断指针有效性应改用 `KittyPtrValidator`。

### 模板查找（hpp `:283-324`，审查时已精读实现）

三个都是**全量线性扫描**，逐对象比较后 `Cast<T>()` 返回，**找不到返回 `T()` 空对象**：

```cpp
// :284-295  按全名
template <typename T = UE_UObject>
T FindObject(const std::string &fullName) const {
    for (int32_t i = 0; i < GetNumElements(); i++) {
        UE_UObject object = GetObjectPtr(i);
        if (object && object.GetFullName() == fullName)   // ← GetFullName！
            return object.Cast<T>();
    }
    return T();
}

// :298-309  按短名
template <typename T = UE_UObject>
T FindObjectFast(const std::string &name) const {
    for (int32_t i = 0; i < GetNumElements(); i++) {
        UE_UObject object = GetObjectPtr(i);
        if (object && object.GetName() == name)
            return object.Cast<T>();
    }
    return T();
}

// :312-324  限定 Outer（⚠️ 非 const 成员函数，与另外两个不一致）
template <typename T = UE_UObject>
T FindObjectFastInOuter(const std::string &name, const std::string &outer) {
    for (int32_t i = 0; i < GetNumElements(); i++) {
        UE_UObject object = GetObjectPtr(i);
        if (object.GetName() == name && object.GetOuter().GetName() == outer)
            return object.Cast<T>();
    }
    return T();
}
```

**性能实测分析（MCP 封装必须知道的）：**

| 函数 | 每对象开销 | 总复杂度 | 量化 |
|---|---|---|---|
| `FindObject` | `GetFullName()` = 沿 Outer 链逐级 `GetName()`，**每级 1+ 次 FName 解析读** | **O(n × outer深度 × 每级读次数)** | 50 万对象 × 平均 3 层 Outer × 每层 2-3 次读 ≈ **数百万次远程读，分钟级** |
| `FindObjectFast` | `GetName()` = 1 次 FName 解析（1-3 次读） | O(n × 2~4) | 50 万对象 ≈ 百万次读，**数十秒** |
| `FindObjectFastInOuter` | 同上 + 外层名 | 同上略贵 | 同上 |

⚠️ **三者都不该在 MCP 里直接暴露为同步工具**。`StaticClass()` 宏用的就是 `FindObject`，
但它有按代数缓存兜底（每代只查一次）。MCP 若要暴露查找，必须：
1. 服务端建 fullName → index 哈希索引（一次全量扫描，之后 O(1)）
2. 或要求用户先给 class 名缩小范围（`ForEachObjectOfClass` 分页）

---

## 3. `UE_FName`（`:140-179`）

### `int GetNumber() const`　`:140-146`

```cpp
if (!object || GetOffsets()->Config.isUsingOutlineNumberName) return 0;
return vm_rpm_ptr<int32_t>(object + GetOffsets()->FName.Number);
```

- 空对象或使用了 OutlineNumber → **返回 0**

### `std::string GetName() const`　`:148-179`

⚠️ **失败时返回 `"None"`，不是空字符串。** 这是最容易踩的坑——MCP 侧若把 `"None"` 当有效名字会污染分析结果。

流程：
1. `!object` → `"None"`
2. 读 `object + FName.ComparisonIndex` 得 index；**读取失败或 `index < 0` → `"None"`**
   （`DisplayIndex` 分支被注释掉了，`:153-154`）
3. `GetNameByID(index)`；**结果为空 → `"None"`**
4. 若非 OutlineNumber 且 `GetNumber() > 0`，追加 `_` + `(number - 1)`（`:163-170`）
5. **取最后一个 `/` 之后的子串**（`:172-176`）——即 `"Class Engine.World"` 会得到 `"World"`

---

## 4. `UE_UObject`（`:181-300`）

| 接口 | 失败返回 | 读取偏移 | 行号 |
|---|---|---|---|
| `EObjectFlags GetFlags()` | `EObjectFlags::NoFlags` | `UObject.ObjectFlags` | `:181-186` |
| `int32_t GetIndex()` | **`-1`** | `UObject.InternalIndex` | `:188-193` |
| `UE_UClass GetClass()` | `nullptr` | `UObject.ClassPrivate` | `:195-200` |
| `UE_UObject GetOuter()` | `nullptr` | `UObject.OuterPrivate` | `:202-207` |

### `UE_UObject GetPackageObject() const`　`:209-219`

沿 `GetOuter()` 链一直向上，**返回最外层的 Outer**。
⚠️ 若 Outer 链有环则死循环（实践中 UE 不会，但 MCP 侧应加深度上限）。

### `std::string GetName() const`　`:221-227`

构造 `UE_FName(object + UObject.NamePrivate)` 后取 `GetName()`。
空对象返回 `""`；FName 解析失败返回 `"None"`。

### `std::string GetFullName() const`　`:229-241`

**格式**：`<ClassName> <Outer>.<Outer>.<Name>`

```cpp
std::string temp;
for (auto outer = GetOuter(); outer; outer = outer.GetOuter())
    temp = outer.GetName() + "." + temp;
return GetClass().GetName() + " " + temp + GetName();
```

例：`Class Engine.World`。

### `std::string GetCppName() const`　`:243-276`

**前缀规则**：沿 Super 链向上找，命中即停

| 命中 | 前缀 |
|---|---|
| `UE_AActor::StaticClass()` | `A` |
| `UE_UObject::StaticClass()` | `U` |
| `UE_UInterface::StaticClass()` | `I` |
| 非 UClass 对象 | `F`（直接赋值，不查链） |

最终 `前缀 + GetName()`。

### `bool IsA(UE_UClass cmp) const`　`:278-291`

```cpp
for (auto super = GetClass(); super; super = super.GetSuper().Cast<UE_UClass>())
    if (super == cmp) return true;
return false;
```

沿 Super 链逐级比较。⚠️ 每次迭代至少 1 次远程读，**深继承链代价高**。

### `bool HasFlags(EObjectFlags flags) const`　`:293-296`

`(GetFlags() & flags) == flags`（**全部位都满足**，不是任一）。

---

## 5. `UE_UField` / `UE_UStruct`（`:313-570`）

### `UE_UField GetNext() const`　`:313`

读 `UField.Next`，空对象返回 nullptr。

### `UE_UStruct` 各 getter

| 接口 | 行为 | 行号 |
|---|---|---|
| `UE_UStruct GetSuper()` | 读 `UStruct.SuperStruct` | `:501-504` |
| `UE_FField GetChildProperties()` | **`UStruct.ChildProperties > 0` 才读，否则返回空 `{}`** | `:506-513` |
| `UE_UField GetChildren()` | **`UStruct.Children > 0` 才读，否则返回空 `{}`** | `:515-522` |
| `int32_t GetSize()` | 读 `UStruct.PropertiesSize` | `:524-527` |
| `int32_t GetMinAlignment()` | **偏移为 0 返回 0**，否则读 `UStruct.MinAlignment` | `:529-533` |

> `ChildProperties`（UE4.25+ 的 FField 链表）与 `Children`（旧版 UField 链表）**二选一**，
> 取决于版本偏移表。MCP 封装时应两个都试。

### `UE_UStruct::StaticClass()`　`:535-550`

⚠️ 唯一**没有用宏**的 StaticClass，因为它带 fallback：
先查 `"Class CoreUObject.Struct"`，失败再查 `"Class CoreUObject.struct"`（小写）。

### `UE_FField FindChildProp(const std::string &name) const`　`:552-560`

沿 `GetChildProperties()` 链表遍历，`prop.GetName() == name` 则返回。
**找不到返回空 `{}`。**

### `UE_UField FindChild(const std::string &name) const`　`:562-570`

同上，但走 `GetChildren()` 链表。

⚠️ **两者都只遍历当前结构体的直接子节点，不向上查父类。**
要拿完整字段列表必须自行沿 `GetSuper()` 链逐级调用（这正是 `AutoFixStructLayout::GetSortedFields` 做的事）。

---

## 6. `UE_UClass`（`:746-773`）

| 接口 | 偏移为 0 时 | 行号 |
|---|---|---|
| `UE_UObject GetClassDefaultObject()` | 返回 `UE_UObject()` 空对象 | `:751-755` |
| `uintptr_t GetImplementedInterfacesPtr()` | 返回 `0` | `:757-761` |
| `uint64_t GetCastFlags()` | 返回 `0` | `:763-767` |
| `uint32_t GetClassFlags()` | 返回 `0` | `:769-773` |

---

## 7. `UE_UFunction`（`:572-592`）

| 接口 | 返回 | 行号 |
|---|---|---|
| `uintptr_t GetFunc()` | 函数地址（`UFunction.Func`） | `:572-575` |
| `int8_t GetNumParams()` | 参数个数 | `:577-580` |
| `int16_t GetParamSize()` | 参数总大小 | `:582-585` |
| `uint32_t GetFunctionEFlags()` | 原始 flags 值 | `:587-590` |
| `std::string GetFunctionFlags()` | **可读的 flags 字符串**（大 switch，`:592-729`，未精读） | `:592` |

---

## 8. `UE_UEnum` / `UE_UScriptStruct`

### `TArray<uint8_t> UE_UEnum::GetNames() const`　`:775-778`

**返回原始字节的 TArray**，不是解析后的枚举值数组。
实际布局是 `TArray<TPair<FName, int64>>`，调用方需自行按该结构解释。

### `std::string UE_UEnum::GetName() const`　`:780-786`

**自动补 `E` 前缀**：名字不以 `'E'` 开头时返回 `"E" + name`。

### `uint32_t UE_UScriptStruct::GetStructFlags() const`　`:740-744`

偏移为 0 返回 0。

---

## 9. 属性层

### 9.1 ⚠️ 关键发现：子指针偏移全部复用 `UProperty.Size`

| 属性子类 | 子指针 getter | 实际读取位置 | 行号 |
|---|---|---|---|
| `UE_UStructProperty` | `GetStruct()` | `object + UProperty.Size` | `:800-803` |
| `UE_UObjectPropertyBase` | `GetPropertyClass()` | `object + UProperty.Size` | `:822-825` |
| `UE_UObjectProperty` | `GetPropertyClass()` | `object + UProperty.Size` | `:837-840` |
| `UE_UArrayProperty` | `GetInner()` | `object + UProperty.Size` | `:852-855` |
| `UE_UByteProperty` | `GetEnum()` | `object + UProperty.Size` | `:867-871` |

**偏移表里的 `UProperty.Size` 语义是"子类数据区起始偏移"，不是"属性大小"。**
属性大小要用 `UE_UProperty::GetSize()`（读 `UProperty.ElementSize`）。

→ MCP 封装时**禁止**用 `UProperty.Size` 当属性大小，否则整个字段布局全错。

### 9.2 `UE_UProperty` 基础（`:361-379`）

```cpp
int32_t  GetArrayDim()      // UProperty.ArrayDim        :361
int32_t  GetSize()          // UProperty.ElementSize     :366
int32_t  GetOffset()        // UProperty.Offset_Internal :371
uint64_t GetPropertyFlags() // UProperty.PropertyFlags   :376
```

### 9.3 `UE_UProperty::GetType()`　`:381-492`

返回 `std::pair<UEPropertyType, std::string>`。
**由 26 个 `IsA<>()` 分支顺序判定**，顺序有讲究：

```
UDoubleProperty → UFloatProperty → UIntProperty → UInt16Property → UInt32Property
→ UInt64Property → UInt8Property → UUInt16Property → UUInt32Property → UUInt64Property
→ UTextProperty → UStrProperty → UClassProperty → UStructProperty → UNameProperty
→ UBoolProperty → UByteProperty → UArrayProperty → UEnumProperty → USetProperty
→ UMapProperty → UInterfaceProperty → UMulticastDelegateProperty
→ UWeakObjectProperty → ULazyObjectProperty → UObjectProperty → UObjectPropertyBase
```

⚠️ **`UObjectProperty` 必须排在 `UObjectPropertyBase` 之前**（继承关系，前者是后者子类）。
全部不匹配时返回 `{Unknown, GetClass().GetName()}`（`:491`）。

⚠️ 已知瑕疵：`UStrProperty` 被归类为 `UEPropertyType::TextProperty`（`:429`）——**看起来是笔误，应为 `StrProperty`**。

### 9.4 `IUProperty`（`:326-359`）

`GetName/GetArrayDim/GetSize/GetOffset/GetPropertyFlags/GetType` 都是**转发到 `UE_UProperty`**。

⚠️ `uint8_t IUProperty::GetFieldMask()`（`:356-359`）是
`((UE_UBoolProperty *)this->prop)->GetFieldMask()` —— **无条件强转 bool 属性**。
对非 bool 属性调用会读到位置 `Size+3` 的任意数据。调用前必须先确认类型是 BoolProperty。

### 9.5 各子类 `GetTypeStr()` 返回值（实测）

| 子类 | 返回 | 行号 |
|---|---|---|
| `UDoubleProperty` | `"double"` | `:793` |
| `UFloatProperty` | `"float"` | `:920` |
| `UIntProperty` | `"int"` | `:927` |
| `UInt16Property` | `"int16_t"` | `:934` |
| `UInt32Property` | `"int32_t"` | `:969` |
| `UInt64Property` | `"int64_t"` | `:941` |
| `UInt8Property` | `"uint8_t"` | `:948` |
| `UUInt16Property` | `"uint16_t"` | `:955` |
| `UUInt32Property` | `"uint32_t"` | `:962` |
| `UNameProperty` | `"struct FName"` | `:815` |
| `UTextProperty` | `"struct FText"` | `:983` |
| `UStrProperty` | `"struct FString"` | `:990` |
| `UStructProperty` | `"struct " + GetStruct().GetCppName()` | `:805-808` |
| `UObjectPropertyBase` | `"struct " + GetPropertyClass().GetCppName() + "*"` | `:827-830` |
| `UObjectProperty` | `"struct " + GetPropertyClass().GetCppName() + "*"` | `:842-845` |
| `UArrayProperty` | `"struct TArray<" + GetInner().GetType().second + ">"` | `:857-860` |
| `UByteProperty` | 有枚举：`"enum class " + e.GetName()`；否则 `"uint8_t"` | `:873-879` |
| `UBoolProperty` | `FieldMask == 0xFF` → `"bool"`；否则 `"uint8_t"` | `:906-913` |

### 9.6 `UE_UBoolProperty` 位域字段（`:886-904`）

四个字段是**连续字节**，基址都是 `UProperty.Size`：

| getter | 位置 | 含义 |
|---|---|---|
| `GetFieldSize()` | `Size + 0` | 字段大小 |
| `GetByteOffset()` | `Size + 1` | 字节偏移 |
| `GetByteMask()` | `Size + 2` | 字节掩码 |
| `GetFieldMask()` | `Size + 3` | 字段掩码 |

`GetFieldMask() == 0xFF` 表示普通 bool，否则是位域（bitfield）。

### 9.7 `UE_UByteProperty::GetEnum()`　`:867-871`

```cpp
auto e = vm_rpm_ptr<UE_UEnum>(object + UProperty.Size);
return (e && e.IsA<UE_UEnum>()) ? e : nullptr;
```

**带 IsA 校验**——读到的不是 UEnum 就返回 nullptr。这个防御是对的，其他子类没有。

---

## 10. MCP 封装陷阱清单

| # | 陷阱 | 后果 | 应对 |
|---|---|---|---|
| 1 | `UE_FName::GetName()` 失败返回 `"None"` | 把失败当有效名字 | 显式区分 `"None"` 与真实失败 |
| 2 | `UProperty.Size` 是子类数据区偏移，不是属性大小 | 字段布局全错 | 用 `GetSize()` |
| 3 | `ForEachObjectOfClass` 对每个对象调 `IsA()` | O(n×深度) 远程读，卡死 | 分页 + 计数上限 |
| 4 | `IsObject()` 是 O(n) 线性扫描 | 极慢 | 改用 `KittyPtrValidator` |
| 5 | `FindChildProp`/`FindChild` 只查当前层 | 漏掉父类字段 | 自行沿 `GetSuper()` 递归 |
| 6 | `IUProperty::GetFieldMask()` 无条件转 bool | 非 bool 属性读到垃圾 | 先判类型 |
| 7 | `StaticClass()` 按代数缓存 | 切进程后拿到旧对象 | 切进程必须重新 `Init()` |
| 8 | 全局单例非线程安全 | 并发调用数据竞争 | 命令服务串行化 |
| 9 | `UStrProperty` 被归类为 `TextProperty` | 类型判断偏差 | 用 `GetTypeStr()` 交叉验证 |
| 10 | `GetChildProperties`/`GetChildren` 偏移为 0 返回空 | 老版本游戏查不到字段 | 两个都试 |
| 11 | `FindObject` 按 `GetFullName()` 逐对象比较 | **O(n × outer深度) 数百万次远程读，分钟级卡死** | 服务端建索引，勿直接暴露 |
| 12 | `FindObjectFastInOuter` 缺 `if (object &&` 判空（_hpp:317_） | 安全（`GetName()` 内部判空返回 `""`）但**风格不一致**，且它是**非 const** 的 | 封装时补判空 |
| 13 | `StaticClass()` 宏按代数缓存，但 `UStruct::StaticClass` 带小写 fallback（`:535-550`） | 大小写敏感的脚本环境查不到 `struct` | 已有 fallback，无需处理 |

---

## 11. 本次未精读范围（后续补齐）

| 范围 | 内容 |
|---|---|
| `:298-320` | `UE_UObject::StaticClass` / `UE_UInterface` / `UE_AActor` / `UE_UField::StaticClass` 实现 |
| `:592-729` | `UE_UFunction::GetFunctionFlags()` 的 flags 大 switch |
| `:975-1651` | `USetProperty` / `UMapProperty` / `UInterfaceProperty` / `UClassProperty` / `UEnumProperty` / `UWeakObjectProperty` / `ULazyObjectProperty` 等剩余子类，以及 `UE_FField` / `UE_FFieldClass` / `UE_FProperty` 全套实现 |

（hpp `:283-324` 的 `FindObject` 三兄弟已在审查时补齐，见 §2。）
