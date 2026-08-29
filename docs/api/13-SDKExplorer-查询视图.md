# L6 查询层 API · SDKExplorer（对象浏览器 / 检索 / 容器视图）

> **源码**：`src/SDKExplorer.cpp`（1455 行）/ `SDKExplorer.hpp`（7 行）
> **定位**：对象浏览器 + 类/字段检索 + 容器视图 + 运行时视图。MCP 的 `searchClasses` / `describeClass` / `inspectObject` 逻辑来源。
> **已精读范围**：1–50（全局状态）+ 590–850（纯逻辑函数全文）。其余为 ImGui UI 渲染（250 处 `ImGui::` 调用，MCP 不依赖）。

---

## 0. 全局约定

1. **全部逻辑在 `namespace SDKExplorer { namespace { ... } }` 内层匿名 namespace**（`:16-19`），**不可外部调用**。
2. **状态全在文件级全局**（`:20-48`）：`gPage` / `gPageSize` / `gSearchMode` / `gSearchBuf` / `gFilteredIdx` / `gCachedNeedle` / `gScanCursor` / `gScanning` / `gSelectedObj` / `gInspectObj` / `gTags` / `gBackStack` 等。
3. ⚠️ **结论（《设备端API清单》L6 已定）**：UI 与状态耦合太深，**不可直接复用，必须另起无头 `SDKQuery` 层**，只搬纯逻辑函数（复用 `UEWrappers`）。

---

## 1. 可复用的纯逻辑函数（MCP `SDKQuery` 层的骨架）

| 函数 | 签名 | 作用 | 行号 |
|---|---|---|---|
| 读字段值 | `std::string ReadFieldValue(uint8_t *base, const UE_FProperty &prop, UEPropertyType ty)` | 按类型读运行时值 | `:598-707` |
| 收集字段 | `void CollectFields(const UE_UStruct &s, std::vector<FieldRow> &out, int depth = 0)` | **递归父类**收集字段 | `:720-755` |
| 找字段行 | `const FieldRow *FindFieldRow(const std::vector<FieldRow> &rows, std::initializer_list<const char *> names)` | 多候选名字匹配 | `:757-768` |
| 数字段 | `int CountChildProperties(const UE_UStruct &s)` / `CountChildren(...)` | 上限 4096 | `:770-784` |
| 属性声明 | `std::string BuildPropertyDecl(type, name, flags)` | const/& 修饰 | `:795-805` |
| 函数签名 | `std::string BuildFunctionSignature(const UE_UFunction &fn)` | 参数列表 + 返回值 | `:807-...` |
| 搜索匹配 | `MatchNeedle(...)` | 异步搜索的匹配逻辑 | `:120` |
| 格式化 | `FormatHexQword` / `FormatHexU32` | 十六进制格式化 | `:591-596` / `:788-793` |

### 1.1 `CollectFields`（`:720-755`）—— 关键：递归父类

```cpp
if (!s || depth > 16) return;              // 深度上限 16
UE_UStruct super = s.GetSuper();
if (super) CollectFields(super, out, depth + 1);   // ← 先父类
// 再当前层：先 ChildProperties（isF=true，FField 体系）
//         再 Children（isF=false，UField 体系）
```

⚠️ **这个递归正好补上 `UE_UStruct::FindChildProp` 只查当前层的缺口**（03 文档 §5）。MCP 的 `describeClass` 要拿完整字段列表（含继承），必须用这个递归模式，不是 `FindChildProp`。

### 1.2 `ReadFieldValue`（`:598-707`）—— 各类型读值语义

| 类型 | 返回 |
|---|---|
| Float/Double | 固定精度浮点 |
| Int/Byte/UInt 系列 | 十进制整数 |
| BoolProperty | `"true"` / `"false"`（读 uint8） |
| NameProperty | `UE_FName(p).GetName()` |
| Object/Class/Weak/Lazy/Soft/Interface | **`0x地址 + 名字`**（nullptr → `"nullptr"`） |
| StrProperty | `FString::ToString()` |
| **Array/Set/Map** | **只返回 `Count/Max/Data` 摘要，不展开元素** |
| default | `FormatHexQword(读 uintptr)` |

⚠️ **Array/Set/Map 只回摘要不展开**——展开元素的容器视图逻辑在 UI 层（`RenderContainerView` `:1246`），MCP 的 `inspectObject` 若要展开数组，需单独实现（这是 `describeClass`/`inspectObject` 的分界点）。

### 1.3 `BuildFunctionSignature`（`:807-...`）

`CPF_Parm` 过滤 → `CPF_ReturnParm` 设返回值（默认 "void"）→ 其余走 `BuildPropertyDecl` 加 const/&。**ChildProperties 优先，空则回退 Children**（`:812-846`）。

---

## 2. UI 视图（不可复用，但 MCP 工具设计可借鉴）

| 视图 | 函数 | 行号 | MCP 对应 |
|---|---|---|---|
| 对象浏览器 | `RenderObjectBrowser` | `:316-455` | `searchClasses`（分页 + 异步搜索 + 命中计数） |
| 属性 tab | `RenderPropertiesPanel` | `:458` | `inspectObject`（Address/VFTable/ClassPrivate/FullName...） |
| 函数列表 | `RenderFunctionsList` | `:523` | `describeClass` 的 functions 部分 |
| 元数据 tab | `RenderMetadataPanel` | `:922` | `describeClass` 的 Size/对齐/CDO/CastFlags |
| **容器视图** | `RenderContainerView` | `:1246` | **Array/Set/Map 逐元素读值（MaxRows 1–4096）** |
| **运行时视图** | `RenderRuntimePanel` | `:1007` | UWorld/GameInstance/NetDriver + Actor 预览 |
| 成员视图 | `RenderInspectorPanel` | `:1106-1243` | `inspectObject` 字段表 + 下级对象跳转 |

⚠️ **容器视图和运行时视图是 MCP 尚未覆盖的能力**：`RenderContainerView` 的逐元素展开、`RenderRuntimePanel` 的 UWorld/NetDriver 导航，是 `inspectObject` 扩展的方向。

---

## 3. MCP 封装陷阱清单

| # | 陷阱 | 后果 | 应对 |
|---|---|---|---|
| 1 | 全部锁匿名 namespace + 文件级全局 | 无法直接调用 | 另起 SDKQuery 层，搬纯逻辑 |
| 2 | `CollectFields` 递归父类 | 只用 FindChildProp 会漏继承字段 | `describeClass` 用递归模式 |
| 3 | `ReadFieldValue` 数组只回摘要 | inspectObject 拿不到元素 | 容器展开单独实现 |
| 4 | UI 状态（gPage/gSearchBuf 等）无锁 | 多线程访问竞态 | SDKQuery 层加锁/无状态 |
| 5 | `CountChildProperties` 上限 4096 | 超限截断 | describeClass 标注"字段过多已截断" |
| 6 | 搜索是异步（gScanning/gScanCursor） | 状态机复杂 | SDKQuery 层简化为同步+分页 |

---

## 4. 本次未精读范围

| 范围 | 内容 |
|---|---|
| `:51-589`、`:851-1455` | 全部 ImGui UI 渲染（Render*Panel 系列、虚拟键盘 `:220`、跳转栈等） |

MCP 不依赖 UI 渲染。纯逻辑函数已全文精读（§1），UI 视图的功能边界已由《设备端API清单》L6 的行号索引标注。
