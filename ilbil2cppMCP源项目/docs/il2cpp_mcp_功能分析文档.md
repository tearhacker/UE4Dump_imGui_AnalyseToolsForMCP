# il2cpp MCP Server 完整功能分析文档

> **版本**: v2.2.1 | **协议版本**: 2025-11-25  
> **文件**: `mcp_server.py` (2997 行) | **零外部依赖**，仅使用 Python 标准库

---

## 目录

1. [整体架构总览](#1-整体架构总览)
2. [核心基础设施](#2-核心基础设施)
3. [Module 1 — 连接管理 (connection)](#3-module-1--连接管理-connection)
4. [Module 2 — UI 交互 (ui)](#4-module-2--ui-交互-ui)
5. [Module 3 — IL2CPP 元数据 (il2cpp_metadata)](#5-module-3--il2cpp-元数据-il2cpp_metadata)
6. [Module 4 — IL2CPP 方法调用 (il2cpp_invoke)](#6-module-4--il2cpp-方法调用-il2cpp_invoke)
7. [Module 5 — IL2CPP 对象检查 (il2cpp_objects)](#7-module-5--il2cpp-对象检查-il2cpp_objects)
8. [Module 6 — IL2CPP 方法 Hook (il2cpp_hook)](#8-module-6--il2cpp-方法-hook-il2cpp_hook)
9. [Module 7 — 内存映射 (memory_maps)](#9-module-7--内存映射-memory_maps)
10. [Module 8 — 内存读取 (memory_read)](#10-module-8--内存读取-memory_read)
11. [Module 9 — 内存写入 (memory_write)](#11-module-9--内存写入-memory_write)
12. [Module 10 — 内存搜索 (memory_search)](#12-module-10--内存搜索-memory_search)
13. [Module 11 — 指针链 (pointer_chain)](#13-module-11--指针链-pointer_chain)
14. [Module 12 — Dobby Hook (dobby)](#14-module-12--dobby-hook-dobby)
15. [Module 13 — 执行追踪 (trace)](#15-module-13--执行追踪-trace)
16. [Module 14 — LuaJIT 脚本 (lua)](#16-module-14--luajit-脚本-lua)
17. [Module 15 — 汇编引擎 (assembly)](#17-module-15--汇编引擎-assembly)
18. [Module 16 — 反编译器 (decompiler)](#18-module-16--反编译器-decompiler)
19. [Module 17 — 硬件断点 (breakpoint)](#19-module-17--硬件断点-breakpoint)
20. [诊断与原始桥接 (diagnostics)](#20-诊断与原始桥接-diagnostics)
21. [Feature 管理与 Admin Web UI](#21-feature-管理与-admin-web-ui)
22. [MCP 协议层详解](#22-mcp-协议层详解)
23. [数据流全链路图](#23-数据流全链路图)

---

## 1. 整体架构总览

### 1.1 五层架构

```
┌─────────────────────────────────────────────────┐
│              AI Agent (Claude/Codebuff)          │
│              通过 MCP 协议调用工具               │
├─────────────────────────────────────────────────┤
│           McpServer (JSON-RPC 2.0 stdio)         │  ← 第5层：MCP 协议层
├─────────────────────────────────────────────────┤
│           ToolDispatcher (命令调度)               │  ← 第4层：调度层
├─────────────────────────────────────────────────┤
│       FeatureRegistry (功能开关管理)              │  ← 第3层：Feature 控制层
├─────────────────────────────────────────────────┤
│       HookSocketClient (TCP Socket 客户端)       │  ← 第2层：传输层
├─────────────────────────────────────────────────┤
│    Android Native Hook Service (Zygisk 注入)     │  ← 第1层：Native 执行层
│    IL2CPP / Dobby / Memory / LuaJIT / ASM       │
└─────────────────────────────────────────────────┘
```

### 1.2 类职责一览

| 类名 | 行数(约) | 职责 |
|------|---------|------|
| `FeatureRegistry` | ~100行 | 线程安全的功能开关管理，JSON 持久化，订阅者模式通知变更 |
| `BridgeError` | 1行 | 统一错误类型，贯穿整个调用链 |
| `ConnectionConfig` | ~20行 | 连接参数数据类（host/port/timeout/ADB 配置）+ 校验 |
| `HookSocketClient` | ~80行 | TCP 客户端，实现 `OK <length>\n<body>` 原生协议，自动 ADB 端口转发 |
| `McpAdminServer` | ~120行 | HTTP 管理服务，提供浏览器 Web UI 控制功能开关 |
| `ToolDispatcher` | ~700行 | 核心调度器，70+ 工具方法实现，参数校验、编码、JSON 解析 |
| `McpServer` | ~100行 | MCP JSON-RPC 2.0 服务端，stdio 读写，协议握手 |

---

## 2. 核心基础设施

### 2.1 原生通信协议

HookSocketClient 实现了与 Android native hook 服务的 TCP 通信协议：

```
客户端发送:  <command>\n          (单行 UTF-8 命令)
服务端响应:  OK <body_length>\n   (成功，后续跟指定长度 body)
             ERR <message>\n     (失败)
服务端 body: <body_length 字节的 UTF-8 内容>\n  (以换行结尾)
```

**关键实现**：

```python
# HookSocketClient.call() 核心流程
def call(self, command: str, *, timeout: float | None = None, retry_forward: bool = True) -> str:
    # 1. 校验命令合法性（单行、非空、长度限制 64KB）
    # 2. 建立 TCP 连接 (TCP_NODELAY)
    # 3. 发送命令 + 换行
    # 4. 逐字节读取响应头（最大 16KB），解析 OK/ERR
    # 5. 精确读取 body（最大 4MB）
    # 6. 连接失败时自动尝试 adb forward 重试
```

**ADB 自动端口转发**：当连接目标为 localhost 且连接失败时，自动执行：
```
adb [-s <serial>] forward tcp:<port> tcp:<port>
```
Windows 下使用 `CREATE_NO_WINDOW` 隐藏窗口。

### 2.2 FeatureRegistry 功能开关

每个 MCP 工具关联一个或多个 feature flag，所有 flag 默认 **开启**：

```python
# tool_features() 将工具名映射到所需 feature 集合
def tool_features(name: str) -> tuple[str, ...]:
    if name in {"ping", "connection_info", "configure_connection"}:
        return ("connection",)
    if name.startswith("il2cpp_"):
        return ("il2cpp_metadata",)
    # ... 共 20+ 条映射规则
```

**持久化机制**：通过 `--feature-config` 指定 JSON 文件路径（默认 `mcp_features.json`），原子写入（先写 `.tmp` 再 `os.replace`）。

**变更通知**：订阅者模式 — `McpServer` 注册回调，当功能开关变更时发送 `notifications/tools/list_changed` JSON-RPC 通知，Agent 会重新获取工具列表。

### 2.3 参数编码体系

MCP 层接收 JSON 参数后，需要编码为 native 命令的文本格式：

| 编码前缀 | 含义 | 示例 |
|---------|------|------|
| `s<hex>` | UTF-8 字符串 | `s48656c6c6f` = "Hello" |
| `n<hex>` | 数值 | `n312e30` = "1.0" |
| `b0` / `b1` | 布尔值 | false / true |
| `z` | null | null |
| `b<hex>` | bool typed | `b1` = true |
| `h<hex>` | hex text | 用于文本字段的 hex 编码 |

`_invoke_token()` 方法处理 IL2CPP 调用参数的复杂编码，支持：
- 基础类型（string/number/bool/null）
- 显式类型标注（`{"type": "i32", "value": 42}`）
- 枚举值（`{"enum": "RoleSyncState.Walking"}` 或 `{"enum": "Walking"}`）
- 对象地址（`{"address": "0x1234"}`）

---

## 3. Module 1 — 连接管理 (connection)

> **Feature flag**: `connection`  
> **工具数**: 3  
> **目的**: 检测目标 hook 服务是否可达，管理连接参数

### 3.1 `ping` — 检查 hook 连接

**功能**：向 native 端发送 `PING` 命令，验证目标游戏 hook 服务是否在线。

```python
def ping(self, _: dict[str, Any]) -> dict[str, Any]:
    response = self._client().call("PING")
    if response != "PONG":
        raise BridgeError(f"unexpected ping response: {response!r}")
    return {"connected": True, **self._info(), "response": response}
```

**返回**：`{"connected": true, "host": "127.0.0.1", "port": 27184, "timeout": 5.0, "auto_adb_forward": true, "response": "PONG"}`

**代码路径**：`ping()` → `HookSocketClient.call("PING")` → TCP 发送 → 解析 `OK 4\nPONG`

### 3.2 `connection_info` — 获取连接设置

**功能**：返回当前连接配置，不发起网络请求。

```python
def connection_info(self, _: dict[str, Any]) -> dict[str, Any]:
    return self._info()
```

### 3.3 `configure_connection` — 修改连接设置

**功能**：动态修改 host/port/timeout/ADB 配置，支持部分更新。

```python
def configure_connection(self, args: dict[str, Any]) -> dict[str, Any]:
    # 基于当前配置创建候选配置
    candidate = ConnectionConfig(host=self.config.host, ...)
    # 逐字段覆盖传入的参数
    if "host" in args:
        candidate.host = args["host"]
    # ...
    candidate.validate()  # 校验端口范围、超时范围等
    self.config = candidate  # 原子替换
```

---

## 4. Module 2 — UI 交互 (ui)

> **Feature flag**: `ui`  
> **工具数**: 7  
> **目的**: 与 Android 应用的 UI 层交互（剪贴板、输入框、Toast 通知）

### 4.1 `get_clipboard` — 读取剪贴板

通过 native hook 读取应用的系统剪贴板内容：

```python
def get_clipboard(self, _: dict[str, Any]) -> dict[str, Any]:
    return {"text": self._client().call("GET_CLIPBOARD")}
```

### 4.2 `mcp_toast_status` / `mcp_toast_set_enabled` — Toast 通知控制

控制是否在应用内显示 JNI Toast 通知（当 MCP 工具被调用时自动弹出）。

```python
def mcp_toast_set_enabled(self, args: dict[str, Any]) -> dict[str, Any]:
    enabled = args.get("enabled")
    if not isinstance(enabled, bool):
        raise BridgeError("enabled must be a boolean")
    return self._json_call(f"UI_TOAST_SET {'true' if enabled else 'false'}")
```

### 4.3 `mcp_toast_show` — 显示自定义 Toast

立即在应用内显示自定义文本，最大 1000 字符：

```python
def mcp_toast_show(self, args: dict[str, Any]) -> dict[str, Any]:
    message = self._required_text(args, "message")
    return self._json_call(f"UI_TOAST_SHOW {self._hex_text(message)}")
```

### 4.4 `show_input_box` / `wait_input` / `input_and_wait` — Unity 输入框

三步流程控制 Unity TouchScreenKeyboard：

1. **`show_input_box`** — 打开输入框（标题/提示/预填内容），不等待
2. **`wait_input`** — 等待输入框关闭，返回用户输入（超时 100-300000ms）
3. **`input_and_wait`** — 打开 + 等待，一步完成

```python
def show_input_box(self, args: dict[str, Any]) -> dict[str, Any]:
    title, hint, prefill = self._input_args(args)
    # 格式: SHOW_INPUT_BOX <title>|<hint>|<prefill>
    response = self._client().call(f"SHOW_INPUT_BOX {title}|{hint}|{prefill}")
    return {"shown": response == "INPUT_BOX_SHOWN", "response": response}
```

### 4.5 `push_input_result` — 推送输入结果

向 hook 输入状态推送文本，主要用于自动化测试：

```python
def push_input_result(self, args: dict[str, Any]) -> dict[str, Any]:
    text = _single_line(args.get("text", ""), "text")
    response = self._client().call(f"INPUT_RESULT {text}")
    return {"accepted": response == "OK", "response": response}
```

---

## 5. Module 3 — IL2CPP 元数据 (il2cpp_metadata)

> **Feature flag**: `il2cpp_metadata`  
> **工具数**: 7  
> **目的**: 访问 IL2CPP 运行时元数据（镜像、类、方法、字段、搜索、dump）

### 5.1 `il2cpp_status` — 获取运行时状态

初始化/附加到 IL2CPP 运行时，返回基址和 domain 地址：

```python
def il2cpp_status(self, _: dict[str, Any]) -> dict[str, Any]:
    return self._json_call("IL2CPP_STATUS")
# native 端返回: {"base": "0x7xxxxxxx", "domain": "0x7xxxxxxx", ...}
```

### 5.2 `il2cpp_dump_file` — 导出元数据到文件

将完整的 IL2CPP 元数据 dump 到应用私有目录 `files/zygisk_il2cpp_mcp/il2cpp_dump.cs`：

```python
def il2cpp_dump_file(self, _: dict[str, Any]) -> dict[str, Any]:
    return self._json_call("IL2CPP_DUMP_FILE", timeout=300.0)  # 5分钟超时
```

**注意**：dump 数据不通过 MCP 传输（可能太大），只返回成功/失败状态。

### 5.3 `il2cpp_list_images` — 列出所有镜像

返回所有已加载的 IL2CPP assembly image 名称列表（最多 5000 个）：

```python
def il2cpp_list_images(self, args: dict[str, Any]) -> dict[str, Any]:
    limit = int(args.get("limit", 256))
    return self._json_call(f"IL2CPP_IMAGES {limit}")
# native 命令: IL2CPP_IMAGES 256
```

### 5.4 `il2cpp_list_classes` — 列出类

在指定镜像中列出/过滤类，支持命名空间过滤和名称过滤：

```python
def il2cpp_list_classes(self, args: dict[str, Any]) -> dict[str, Any]:
    image = self._hex_text(self._required_text(args, "image"))
    namespace_filter = self._hex_text(...)
    name_filter = self._hex_text(...)
    limit = int(args.get("limit", 200))
    return self._json_call(f"IL2CPP_CLASSES {image} {namespace_filter} {name_filter} {limit}")
```

### 5.5 `il2cpp_list_methods` — 列出方法

列出指定类的所有方法，包含原生地址、RVA、参数类型、返回类型：

```python
def il2cpp_list_methods(self, args: dict[str, Any]) -> dict[str, Any]:
    # IL2CPP_METHODS <image> <namespace> <class_name> <name_filter> <limit>
    return self._json_call(f"IL2CPP_METHODS {image} {namespace} {class_name} {name_filter} {limit}")
```

### 5.6 `il2cpp_list_fields` — 列出字段

列出类的字段信息，包含类型、偏移、标志、静态/literal 状态：

```python
def il2cpp_list_fields(self, args: dict[str, Any]) -> dict[str, Any]:
    inherited = args.get("include_inherited", False)
    # IL2CPP_FIELDS <image> <ns> <class> <filter> <inherited> <offset> <limit>
    return self._json_call(
        f"IL2CPP_FIELDS {image} {namespace} {class_name} {name_filter} "
        f"{'1' if inherited else '0'} {offset} {limit}"
    )
```

### 5.7 `il2cpp_search` — 模糊搜索元数据

跨类/方法/字段进行模糊搜索，支持多种匹配模式：

```python
def il2cpp_search(self, args: dict[str, Any]) -> dict[str, Any]:
    # 支持 entity: class | method | field
    # 支持 match_mode: contains | prefix | exact
    # 支持 case_sensitive 开关
    # 支持 image_filter / namespace_filter / class_filter 三级过滤
    # IL2CPP_SEARCH <entity> <query> <image> <ns> <class> <mode> <case> <offset> <limit>
    return self._json_call("IL2CPP_SEARCH " + " ".join([...]), timeout=60.0)
```

---

## 6. Module 4 — IL2CPP 方法调用 (il2cpp_invoke)

> **Feature flag**: `il2cpp_metadata` + `il2cpp_invoke`  
> **工具数**: 2  
> **目的**: 运行时调用 IL2CPP 托管方法

### 6.1 `il2cpp_invoke` / `il2cpp_call` — 调用方法

两者是别名关系（`il2cpp_call` = `il2cpp_invoke`），通过 `il2cpp_runtime_invoke` 执行方法。

**参数编码流程**：

```python
def il2cpp_invoke(self, args: dict[str, Any]) -> dict[str, Any]:
    words = self._lookup_words(args)  # [image, namespace, class, method, param_count]
    instance = self._address(args.get("instance_address", "0x0"), "instance_address")
    values = args.get("arguments", [])  # 最多 64 个参数
    encoded = [self._invoke_token(value) for value in values]
    command = "IL2CPP_INVOKE " + " ".join(words + [instance, str(len(encoded)), *encoded])
    return self._json_call(command, timeout=30.0)
```

**参数编码规则** (`_invoke_token`)：

| 参数类型 | 编码格式 | 示例 |
|---------|---------|------|
| null | `z` | `z` |
| bool | `b0` / `b1` | `b1` = true |
| int/float | `n<hex>` | `n34302e30` = "42.0" |
| string | `s<hex>` | `s48656c6c6f` = "Hello" |
| 枚举 | `s<hex>` (枚举名) | `s57616c6b696e67` = "Walking" |
| 对象地址 | `a<hex>` | 直接传入地址 |
| 显式类型 | 复合编码 | `{"type": "i32", "value": 42}` → `b`/`n` 前缀 |

**IL2CPP 方法查找** (`_lookup_words`)：

```python
def _lookup_words(self, args):
    return [
        self._hex_text(args["image"]),      # Assembly-CSharp.dll
        self._hex_text(args["namespace"]),   # Game.Logic
        self._hex_text(args["class_name"]),  # PlayerController
        self._hex_text(args["method"]),      # GetHealth
        str(int(args.get("parameter_count", -1))),
    ]
```

---

## 7. Module 5 — IL2CPP 对象检查 (il2cpp_objects)

> **Feature flag**: `il2cpp_metadata` + `il2cpp_invoke` + `il2cpp_objects`  
> **工具数**: 3  
> **目的**: 检查运行时 IL2CPP 对象的字段值、List/数组内容、Dictionary 键值

### 7.1 `il2cpp_object_inspect` — 检查对象字段

加载一个活的 IL2CPP 对象，返回其类信息和分页的字段值：

```python
def il2cpp_object_inspect(self, args: dict[str, Any]) -> dict[str, Any]:
    address = self._address(args["address"], "address")
    # IL2CPP_OBJECT_INSPECT <addr> <inherited> <offset> <limit>
    return self._json_call(f"IL2CPP_OBJECT_INSPECT {address} {'1' if inherited else '0'} {offset} {limit}")
```

支持参数：
- `address`: 对象地址（不能为 0）
- `include_inherited`: 是否包含继承字段（默认 true）
- `offset` / `limit`: 分页控制

### 7.2 `il2cpp_list_items` — 读取 List/数组元素

读取 `List<T>` 或一维数组的分页内容：

```python
def il2cpp_list_items(self, args: dict[str, Any]) -> dict[str, Any]:
    address = self._address(args["address"], "address")
    # IL2CPP_LIST_ITEMS <addr> <offset> <limit>
    return self._json_call(f"IL2CPP_LIST_ITEMS {address} {offset} {limit}")
```

限制：每页最多 256 项，总偏移最大 1000000。

### 7.3 `il2cpp_dictionary_get` — 获取 Dictionary 键值

调用 `Dictionary<TKey,TValue>.get_Item` 获取指定键的值：

```python
def il2cpp_dictionary_get(self, args: dict[str, Any]) -> dict[str, Any]:
    address = self._address(args["address"], "address")
    token = self._invoke_token(args.get("key"))  # 支持所有 invoke 参数类型
    return self._json_call(f"IL2CPP_DICTIONARY_GET {address} {token}")
```

---

## 8. Module 6 — IL2CPP 方法 Hook (il2cpp_hook)

> **Feature flag**: `il2cpp_metadata` + `il2cpp_hook`  
> **工具数**: 3  
> **目的**: 对 IL2CPP 方法安装/移除 Dobby Hook

### 8.1 `il2cpp_hook` — Hook 到自定义替换地址

解析 IL2CPP 方法后安装 Dobby Hook，跳转到用户提供的替换地址：

```python
def il2cpp_hook(self, args: dict[str, Any]) -> dict[str, Any]:
    words = self._lookup_words(args)
    replacement = self._address(args["replacement_address"], "replacement_address")
    return self._json_call("IL2CPP_HOOK " + " ".join(words + [replacement]))
# 返回: {"original": "0x7xxxxxxx"} (trampoline 地址)
```

### 8.2 `il2cpp_hook_return` — Hook 返回固定值

安装一个自动生成的替换桩，直接返回指定 ABI 类型的固定值：

```python
def il2cpp_hook_return(self, args: dict[str, Any]) -> dict[str, Any]:
    words = self._lookup_words(args)
    kind = args["return_kind"]  # void | bool | i64 | u64 | ptr | f32 | f64
    value = self._return_value(args)
    return self._json_call("IL2CPP_HOOK_RETURN " + " ".join(words + [kind, value]))
```

**支持的 return_kind**：

| 类型 | 说明 | 示例值 |
|------|------|--------|
| `void` | 无返回值 | 不需要 value |
| `bool` | 布尔 | `true` / `false` |
| `i64` / `u64` | 64位整数 | `42` / `0xFFFFFFFF` |
| `ptr` | 指针 | `0x12345678` |
| `f32` / `f64` | 浮点数 | `3.14` |

### 8.3 `il2cpp_unhook` — 移除 Hook

先解析方法地址，然后调用 `DOBBY_DESTROY` 恢复原函数：

```python
def il2cpp_unhook(self, args: dict[str, Any]) -> dict[str, Any]:
    method = self.il2cpp_find_method(args)  # 先解析方法
    address = method.get("address")
    return self._json_call(f"DOBBY_DESTROY {self._address(address, 'address')}")
```

---

## 9. Module 7 — 内存映射 (memory_maps)

> **Feature flag**: `memory_maps`  
> **工具数**: 4  
> **目的**: 解析 `/proc/self/maps`，管理模块基址、地址归属查询

### 9.1 `memory_list_modules` — 列出已加载模块

列出目标进程中所有可执行模块：

```python
def memory_list_modules(self, args: dict[str, Any]) -> dict[str, Any]:
    name_filter = _single_line(args.get("name_filter", ""), "name_filter")
    limit = self._bounded_integer(args.get("limit", 256), "limit", 1, 4096)
    return self._json_call(f"MEMORY_MODULES {self._hex_text(name_filter)} {limit}")
```

**返回字段**：path, occurrence, start, end, load_bias, size, region_count

### 9.2 `memory_find_module` — 查找指定模块

解析精确的模块名 + 出现次数（重复模块 occurrence >= 1）：

```python
def memory_find_module(self, args: dict[str, Any]) -> dict[str, Any]:
    module_name = self._required_text(args, "module_name")
    occurrence = self._bounded_integer(args.get("occurrence", 1), "occurrence", 1, 4096)
    return self._json_call(f"MEMORY_MODULE_FIND {self._hex_text(module_name)} {occurrence}")
```

### 9.3 `memory_address_info` — 地址归属查询

确定一个地址属于哪个映射区域、哪个模块、相对偏移是多少：

```python
def memory_address_info(self, args: dict[str, Any]) -> dict[str, Any]:
    address = self._address(self._required_text(args, "address"), "address")
    return self._json_call(f"MEMORY_ADDRESS_INFO {address}")
```

### 9.4 `memory_resolve_address` — 模块基址 + 偏移解析

ASLR 安全的地址解析：从模块名 + occurrence + 偏移量计算绝对地址：

```python
def memory_resolve_address(self, args: dict[str, Any]) -> dict[str, Any]:
    # 1. 查找模块获取基址
    module = self.memory_find_module({"module_name": module_name, "occurrence": occurrence})
    base = int(self._address(module[base_kind], base_kind), 16)
    # 2. 计算最终地址
    offset = self._signed_offset(args.get("offset", "0x0"), "offset")
    address = base + offset
    # 3. 返回完整解析结果
    return {"module": module, "base_kind": base_kind, "base": hex(base),
            "offset": hex(offset), "address": hex(address)}
```

---

## 10. Module 8 — 内存读取 (memory_read)

> **Feature flag**: `memory_read`  
> **工具数**: 2  
> **目的**: 读取目标进程内存

### 10.1 `memory_read` — 读取原始字节

读取 1-65536 字节的原始十六进制数据：

```python
def memory_read(self, args: dict[str, Any]) -> dict[str, Any]:
    address = self._address(args["address"], "address")
    size = int(args["size"])  # 1 ~ 65536
    return self._json_call(f"MEMORY_READ {address} {size}")
# 返回: {"hex": "488b05..."}  (小写十六进制)
```

### 10.2 `memory_read_value` — 读取类型化值

读取一个固定大小的类型化值（小端序）：

```python
def memory_read_value(self, args: dict[str, Any]) -> dict[str, Any]:
    kind, size = self._memory_value_format(args["value_type"])  # 获取字节数
    result = self.memory_read({"address": args["address"], "size": size})
    # 将原始 hex 解码为类型化值
    value = self._decode_memory_value(kind, hex_bytes)
    return {"value_type": kind, "value": value, ...}
```

**支持的 value_type**：

| 类型 | 字节大小 | struct 格式 | 示例值 |
|------|---------|-------------|--------|
| `bool` | 1 | `B` | `true` / `false` |
| `i8` / `u8` | 1 | `b` / `B` | `-128` / `255` |
| `i16` / `u16` | 2 | `h` / `H` | `-32768` / `65535` |
| `i32` / `u32` | 4 | `i` / `I` | `-2147483648` / `4294967295` |
| `i64` / `u64` | 8 | `q` / `Q` | 64位整数 |
| `f32` | 4 | `f` | `3.14` |
| `f64` | 8 | `d` | `3.14159265` |
| `ptr32` | 4 | `I` | `0x12345678` |
| `ptr64` | 8 | `Q` | `0x7fff12345678` |

**解码实现** (`_decode_memory_value`)：

```python
@classmethod
def _decode_memory_value(cls, value_type, hex_bytes):
    kind, size = cls._memory_value_format(value_type)
    data = bytes.fromhex(hex_bytes)
    value = struct.unpack("<" + MEMORY_VALUE_FORMATS[kind], data)[0]
    if kind == "bool":
        return bool(value)
    if kind.startswith("ptr"):
        return f"0x{value:x}"
    return value
```

---

## 11. Module 9 — 内存写入 (memory_write)

> **Feature flag**: `memory_write`  
> **工具数**: 2  
> **目的**: 写入目标进程内存（破坏性操作）

### 11.1 `memory_write` — 写入原始字节

```python
def memory_write(self, args: dict[str, Any]) -> dict[str, Any]:
    address = self._address(args["address"], "address")
    hex_bytes = self._required_text(args, "hex_bytes").strip()
    # 校验: 非空、偶数长度、最大 131072 hex chars (65536 bytes)、合法 hex
    return self._json_call(f"MEMORY_WRITE {address} {hex_bytes}")
# 返回: {"previous_hex": "...", "verified": true}
```

### 11.2 `memory_write_value` — 写入类型化值

先编码为 hex bytes，写入后返回之前的值：

```python
def memory_write_value(self, args: dict[str, Any]) -> dict[str, Any]:
    kind, _ = self._memory_value_format(args["value_type"])
    hex_bytes, normalized_value = self._encode_memory_value(kind, args["value"])
    result = self.memory_write({"address": args["address"], "hex_bytes": hex_bytes})
    previous_value = self._decode_memory_value(kind, result["previous_hex"])
    return {**result, "value_type": kind, "value": normalized_value,
            "previous_value": previous_value}
```

**编码实现** (`_encode_memory_value`)：

```python
@classmethod
def _encode_memory_value(cls, value_type, value):
    kind, _ = cls._memory_value_format(value_type)
    # 根据类型解析和校验值
    if kind == "bool":
        parsed = bool(value)
    elif kind in {"f32", "f64"}:
        parsed = float(value)  # 校验 isfinite
    else:
        parsed = int(value)    # 支持 str/int/float.is_integer
    # struct pack 为小端序 hex
    encoded = struct.pack("<" + MEMORY_VALUE_FORMATS[kind], parsed)
    return encoded.hex(), cls._decode_memory_value(kind, encoded.hex())
```

---

## 12. Module 10 — 内存搜索 (memory_search)

> **Feature flag**: `memory_search`  
> **工具数**: 7  
> **目的**: 在目标进程内存中搜索字节模式、类型化值、未知值

### 12.1 `memory_search` — 字节模式搜索

在模块或地址范围内搜索十六进制字节模式（支持通配符 `?`）：

```python
def memory_search(self, args: dict[str, Any]) -> dict[str, Any]:
    pattern = self._memory_search_pattern(args["pattern"])  # 校验 hex + 通配符
    start, end = self._memory_search_range(args)  # 模块或显式范围
    # MEMORY_SEARCH <start> <end> <pattern> <max> <align> <types>
    return self._json_call(f"MEMORY_SEARCH {start} {end} {self._hex_text(pattern)} "
                           f"{max_results} {alignment} {self._hex_text(memory_types)}",
                           timeout=300.0)
```

**通配符示例**：`48 8B ?? A?` — 固定字节 + nibble 通配

### 12.2 `memory_search_value` — 类型化值搜索

将类型化值编码为 hex pattern 后搜索：

```python
def memory_search_value(self, args: dict[str, Any]) -> dict[str, Any]:
    kind, value_size = self._memory_value_format(args["value_type"])
    pattern, normalized_value = self._encode_memory_value(kind, args["value"])
    # 默认 alignment = 值大小（如 i32 对齐到 4 字节）
    search_args["pattern"] = pattern
    search_args.setdefault("alignment", value_size)
    return {**result, "value_type": kind, "value": normalized_value}
```

### 12.3 `memory_search_exact` — 多类型同时搜索

将同一值用多种编码同时搜索，每种创建独立的搜索会话：

```python
def memory_search_exact(self, args: dict[str, Any]) -> dict[str, Any]:
    # value_types: ["i32", "f32", "ptr32"] — 同一数值多种编码
    searches = []
    for raw_kind in value_types:
        searches.append(self.memory_search_value({...}))
    return {"exact": True, "searches": searches, "search_count": len(searches)}
```

### 12.4 `memory_search_fuzzy` — 模糊搜索（未知值）

不指定初始值，快照指定大小的所有对齐值，后续通过过滤缩小范围：

```python
def memory_search_fuzzy(self, args: dict[str, Any]) -> dict[str, Any]:
    value_size = args["value_size"]  # 1, 2, 4, 8
    # MEMORY_SEARCH_FUZZY <start> <end> <size> <max> <align> <types>
    return self._json_call(f"MEMORY_SEARCH_FUZZY {start} {end} {value_size} "
                           f"{max_results} {alignment} {self._hex_text(memory_types)}")
```

### 12.5 `memory_filter` — 过滤搜索结果

在搜索会话上应用过滤器，逐步缩小结果集：

```python
def memory_filter(self, args: dict[str, Any]) -> dict[str, Any]:
    session_id = args["session_id"]
    mode = args["mode"]  # equals | not_equals | changed | unchanged | increased | decreased
    # equals/not_equals 需要 pattern，其余模式自动比较快照
    return self._json_call(f"MEMORY_FILTER {session_id} {mode} {self._hex_text(pattern)}")
```

**过滤模式**：

| 模式 | 说明 | 需要 pattern |
|------|------|-------------|
| `equals` | 值等于 pattern | ✅ |
| `not_equals` | 值不等于 pattern | ✅ |
| `changed` | 自上次快照以来变化 | ❌ |
| `unchanged` | 自上次快照以来未变 | ❌ |
| `increased` | 值增大（无符号比较） | ❌ |
| `decreased` | 值减小（无符号比较） | ❌ |

### 12.6 `memory_filter_value` — 类型化值过滤

与 `memory_filter` 相同，但接受类型化值参数：

```python
def memory_filter_value(self, args: dict[str, Any]) -> dict[str, Any]:
    kind, _ = self._memory_value_format(args["value_type"])
    pattern, normalized_value = self._encode_memory_value(kind, args["value"])
    result = self.memory_filter({"session_id": ..., "mode": mode, "pattern": pattern})
    return {**result, "value_type": kind, "value": normalized_value}
```

### 12.7 `memory_search_results` — 分页获取结果

```python
def memory_search_results(self, args: dict[str, Any]) -> dict[str, Any]:
    # MEMORY_SEARCH_RESULTS <session_id> <offset> <limit>
    return self._json_call(f"MEMORY_SEARCH_RESULTS {session_id} {offset} {limit}")
```

### 12.8 `memory_search_clear` — 清除搜索会话

释放 native 端的搜索快照数据：

```python
def memory_search_clear(self, args: dict[str, Any]) -> dict[str, Any]:
    return self._json_call(f"MEMORY_SEARCH_CLEAR {session_id}")
```

---

## 13. Module 11 — 指针链 (pointer_chain)

> **Feature flag**: `memory_maps` + `memory_read` + `pointer_chain`（写操作额外需要 `memory_write`）  
> **工具数**: 5  
> **目的**: 多级指针解引用、指针链扫描

### 13.1 `memory_resolve_pointer_chain` — 解析多级指针链

从模块基址或绝对地址出发，逐级读取指针 + 偏移：

```python
def memory_resolve_pointer_chain(self, args: dict[str, Any]) -> dict[str, Any]:
    # 1. 确定起始地址（模块或绝对地址）
    if has_module:
        resolved = self.memory_resolve_address({...})
        current = int(resolved["address"], 16)
    else:
        base = int(self._address(args["base_address"], ...), 16)
        current = base + base_offset

    # 2. 逐级解引用（最多 32 级）
    pointer_type = f"ptr{pointer_size * 8}"  # ptr32 或 ptr64
    for index, raw_offset in enumerate(raw_offsets):
        read = self.memory_read_value({"address": hex(current), "value_type": pointer_type})
        pointer = int(read["value"], 16)
        offset = self._signed_offset(raw_offset, f"offsets[{index}]")
        current = pointer + offset
        steps.append({...})

    # 3. 可选最终解引用
    if dereference_final:
        read = self.memory_read_value({"address": hex(current), "value_type": pointer_type})
        current = int(read["value"], 16)

    return {"pointer_size": pointer_size, "origin": origin, "steps": steps, "address": hex(current)}
```

### 13.2 `memory_read_pointer_chain` — 指针链 + 读取值

解析指针链后在最终地址读取类型化值：

```python
def memory_read_pointer_chain(self, args: dict[str, Any]) -> dict[str, Any]:
    chain_args["dereference_final"] = False
    chain = self.memory_resolve_pointer_chain(chain_args)
    value = self.memory_read_value({"address": chain["address"], "value_type": value_type})
    return {"chain": chain, "read": value}
```

### 13.3 `memory_write_pointer_chain` — 指针链 + 写入值

解析指针链后在最终地址写入类型化值：

```python
def memory_write_pointer_chain(self, args: dict[str, Any]) -> dict[str, Any]:
    chain_args["dereference_final"] = False
    chain = self.memory_resolve_pointer_chain(chain_args)
    written = self.memory_write_value({"address": chain["address"], ...})
    return {"chain": chain, "write": written}
```

### 13.4 `memory_scan_base` — 扫描指向目标的指针

多线程扫描内存区域，找到所有指向指定地址的指针：

```python
def memory_scan_base(self, args: dict[str, Any]) -> dict[str, Any]:
    # 确定目标地址
    if has_module:
        target = self.memory_resolve_address({...})["address"]
    else:
        target = self._checked_address(raw_target + target_offset)
    # 确定扫描范围
    if scan_module:
        start, end = module.start, module.end
    # 自动确定工作线程数
    workers = min(32, max(2, os.cpu_count() or 4))
    # MEMORY_POINTER_SCAN_MT <start> <end> <target> <ptr_size> <max> <workers> <types>
    return self._json_call(f"MEMORY_POINTER_SCAN_MT {start} {end} {target} ...", timeout=300.0)
```

---

## 14. Module 12 — Dobby Hook (dobby)

> **Feature flag**: `dobby`  
> **工具数**: 7  
> **目的**: 原生级别的函数 Hook、代码补丁、符号解析

### 14.1 `dobby_version` — 获取版本号

```python
def dobby_version(self, _):
    return self._json_call("DOBBY_VERSION")
```

### 14.2 `dobby_resolve_symbol` — 解析符号地址

使用 `DobbySymbolResolver` 查找动态库中的符号：

```python
def dobby_resolve_symbol(self, args):
    image = self._hex_text(args.get("image", ""))  # 空字符串=全局搜索
    symbol = self._hex_text(args["symbol"])
    return self._json_call(f"DOBBY_RESOLVE {image} {symbol}")
```

### 14.3 `dobby_hook` — 安装原生 Hook

```python
def dobby_hook(self, args):
    target = self._address(args["target_address"], ...)
    replacement = self._address(args["replacement_address"], ...)
    return self._json_call(f"DOBBY_HOOK {target} {replacement}")
# 返回: {"original": "0x7xxxxxxx"} (trampoline 地址)
```

### 14.4 `dobby_hook_return` — 安装固定返回值 Hook

```python
def dobby_hook_return(self, args):
    target = self._address(args["target_address"], ...)
    kind = args["return_kind"]  # void|bool|i64|u64|ptr|f32|f64
    value = self._return_value(args)
    return self._json_call(f"DOBBY_HOOK_RETURN {target} {kind} {value}")
```

### 14.5 `dobby_patch_code` — 直接补丁代码

在可执行页面上写入机器码（最多 4096 字节）：

```python
def dobby_patch_code(self, args):
    address = self._address(args["address"], ...)
    hex_bytes = args["hex_bytes"]  # 校验: 偶数长度, 最大 8192 hex chars
    return self._json_call(f"DOBBY_PATCH {address} {hex_bytes}")
```

### 14.6 `dobby_destroy` — 恢复 Hook

使用 `DobbyDestroy` 恢复被 Hook 的原始函数：

```python
def dobby_destroy(self, args):
    target = self._address(args["target_address"], ...)
    return self._json_call(f"DOBBY_DESTROY {target}")
```

### 14.7 `dobby_list_hooks` — 列出所有活动 Hook

```python
def dobby_list_hooks(self, _):
    return self._json_call("DOBBY_LIST_HOOKS")
# 返回: 所有 fixed-return hooks, raw hooks, instrumentation counters
```

---

## 15. Module 13 — 执行追踪 (trace)

> **Feature flag**: `dobby` + `trace`  
> **工具数**: 3  
> **目的**: Dobby 执行计数和回溯

### 15.1 `dobby_instrument` — 安装执行计数器

```python
def dobby_instrument(self, args):
    address = self._address(args["address"], ...)
    return self._json_call(f"DOBBY_INSTRUMENT {address}")
```

### 15.2 `dobby_trace_get` — 读取执行计数

```python
def dobby_trace_get(self, args):
    address = self._address(args["address"], ...)
    return self._json_call(f"DOBBY_TRACE_GET {address}")
# 返回: {"count": 12345}
```

### 15.3 `dobby_trace_backtrace` — 读取回溯

读取 Dobby instrumentation 捕获的寄存器/帧指针回溯，并自动解析模块信息：

```python
def dobby_trace_backtrace(self, args):
    address = self._address(args["address"], ...)
    max_frames = self._bounded_integer(args.get("max_frames", 16), ...)
    result = self._json_call(f"DOBBY_TRACE_BACKTRACE {address} {max_frames}")
    return self._resolve_backtrace_modules(result)  # 自动关联模块信息
```

**回溯模块解析** (`_resolve_backtrace_modules`)：

```python
def _resolve_backtrace_modules(self, result):
    for frame in raw_frames:
        address = frame.get("address")
        if resolve_modules and address:
            info = self.memory_address_info({"address": address})
            frame["mapping"] = info  # {module, offset, region, ...}
    return {**result, "frames": resolved_frames}
```

---

## 16. Module 14 — LuaJIT 脚本 (lua)

> **Feature flag**: `lua`  
> **工具数**: 4  
> **目的**: 在目标进程中执行 Lua 脚本

### 16.1 `lua_status` — VM 状态查询

```python
def lua_status(self, _):
    return self._json_call("LUA_STATUS")
# 返回: {"available": true, "vm_state": "ready"}
```

### 16.2 `lua_execute` — 执行 Lua 脚本

延迟创建 LuaJIT VM 并执行脚本（最大 256KB，超时 0.1-300s）：

```python
def lua_execute(self, args):
    script = args["script"]
    if "\x00" in script:
        raise BridgeError("script cannot contain NUL bytes")
    if len(script.encode("utf-8")) > 256 * 1024:
        raise BridgeError("script exceeds 256 KiB")
    timeout = float(args.get("timeout", 30))
    return self._json_call(f"LUA_EXEC {self._hex_text(script)}", timeout=timeout)
```

### 16.3 `lua_reset` — 重置 VM

清理 Lua 拥有的 hooks 并替换为干净的 VM：

```python
def lua_reset(self, _):
    return self._json_call("LUA_RESET")
```

### 16.4 `lua_logs` — 读取脚本输出日志

```python
def lua_logs(self, args):
    clear = args.get("clear", True)  # 默认读取后清除
    return self._json_call(f"LUA_LOGS {1 if clear else 0}")
```

---

## 17. Module 15 — 汇编引擎 (assembly)

> **Feature flag**: `assembly`  
> **工具数**: 4  
> **目的**: ARM64 汇编/反汇编/指令补丁

### 17.1 `assembly_status` — 引擎状态

```python
def assembly_status(self, _):
    return self._json_call("ASM_STATUS")
# 返回: ASMJit/Capstone/Dobby 可用性
```

### 17.2 `assembly_assemble` — 汇编单条指令

将文本 AArch64 指令编译为机器码（不修改内存）：

```python
def assembly_assemble(self, args):
    instruction = self._required_text(args, "instruction").strip()
    # 最大 1024 字节
    return self._json_call(f"ASM_ASSEMBLE {self._hex_text(instruction)}")
# 返回: {"hex": "ff4300d1", "size": 4}
```

### 17.3 `assembly_disassemble` — 反汇编

通过原始系统路径读取内存并用 Capstone 反汇编：

```python
def assembly_disassemble(self, args):
    address = self._address(args["address"], ...)
    size = self._bounded_integer(args.get("size", 64), ..., 1, 4096)
    max_instructions = self._bounded_integer(args.get("max_instructions", 16), ..., 1, 256)
    return self._json_call(f"ASM_DISASSEMBLE {address} {size} {max_instructions}")
# 返回: {"instructions": [{"address": "0x...", "mnemonic": "stp", "op_str": "x29, x30, [sp, #-16]!"}, ...]}
```

### 17.4 `assembly_patch` — 汇编并补丁

一步完成汇编 + 补丁到可执行地址：

```python
def assembly_patch(self, args):
    address = self._address(args["address"], ...)
    instruction = self._required_text(args, "instruction").strip()
    return self._json_call(f"ASM_PATCH {address} {self._hex_text(instruction)}")
```

---

## 18. Module 16 — 反编译器 (decompiler)

> **Feature flag**: `decompiler`  
> **工具数**: 2  
> **目的**: 使用 Ghidra-native ARM64 反编译器生成 C 伪代码

### 18.1 `decompiler_status` — 引擎状态

```python
def decompiler_status(self, _):
    return self._json_call("DECOMP_STATUS")
# 返回: Ghidra/Sleigh 反编译器是否在目标进程中加载
```

### 18.2 `decompile_function` — 反编译函数

使用 Ghidra/Sleigh 反编译 ARM64 函数，利用**活目标内存**读取引用的字符串和全局变量：

```python
def decompile_function(self, args):
    address = self._address(args["address"], ...)
    size = self._bounded_integer(args.get("size", 256), ..., 4, 65536)
    if size % 4:
        raise BridgeError("size must be a multiple of 4 for ARM64")
    max_instructions = self._bounded_integer(args.get("max_instructions", 256), ..., 1, 4096)
    max_output_bytes = self._bounded_integer(args.get("max_output_bytes", 262144), ...)
    optimize = args.get("optimize", True)
    stop_at_return = args.get("stop_at_return", True)  # 遇到 RET 停止
    return self._json_call(
        f"DECOMP_DECOMPILE {address} {size} {max_instructions} {max_output_bytes} "
        f"{1 if optimize else 0} {1 if stop_at_return else 0}",
        timeout=60.0,
    )
```

**关键参数**：
- `size`: 函数大小（必须 4 的倍数，ARM64 指令对齐）
- `optimize`: 是否优化伪代码
- `stop_at_return`: 遇到线性 RET 时停止（当函数大小不精确时有用）
- `max_output_bytes`: 最大输出大小（最大 1MB）

**特性**：IL2CPP 方法地址会自动获得托管返回类型、参数类型、类名和字段偏移类型注解。

---

## 19. Module 17 — 硬件断点 (breakpoint)

> **Feature flag**: `breakpoint`  
> **工具数**: 7  
> **目的**: ARM64 perf 硬件断点/观察点管理

### 19.1 `breakpoint_status` — 断点引擎状态

```python
def breakpoint_status(self, _):
    return self._json_call("BREAKPOINT_STATUS")
# 返回: ARM64 perf 硬件断点支持和延迟初始化状态
```

### 19.2 `breakpoint_set` — 设置断点/观察点

为所有当前和未来目标线程设置非停止型硬件断点：

```python
def breakpoint_set(self, args):
    address = self._address(args["address"], ...)
    kind = args["type"]  # x(执行) | r(读) | w(写) | rw(读写)
    length = args.get("length", 4)  # 1, 2, 4, 8 字节
    return self._json_call(f"BREAKPOINT_SET {address} {kind} {length}")
```

**断点类型**：

| 类型 | 说明 | 典型用途 |
|------|------|---------|
| `x` | 执行断点 | 函数调用追踪 |
| `r` | 读观察点 | 监控内存读取 |
| `w` | 写观察点 | 监控内存写入（如血量修改） |
| `rw` | 读写观察点 | 监控所有内存访问 |

### 19.3 `breakpoint_list` — 列出所有断点

```python
def breakpoint_list(self, _):
    return self._json_call("BREAKPOINT_LIST")
```

### 19.4 `breakpoint_hits` — 读取命中记录

分页读取断点命中记录，包含线程 ID 和 ARM64 寄存器状态：

```python
def breakpoint_hits(self, args):
    address = self._address(args.get("address", "0"), "address")  # 可选过滤
    offset = self._bounded_integer(args.get("offset", 0), ...)
    limit = self._bounded_integer(args.get("limit", 100), ...)
    return self._json_call(f"BREAKPOINT_HITS {address} {offset} {limit}")
```

### 19.5 `breakpoint_backtrace` — 断点命中回溯

为指定的命中记录获取帧指针回溯，并自动解析模块信息：

```python
def breakpoint_backtrace(self, args):
    hit_id = self._bounded_integer(args["hit_id"], ..., 1, (1<<63)-1)
    max_frames = self._bounded_integer(args.get("max_frames", 32), ..., 1, 64)
    result = self._json_call(f"BREAKPOINT_BACKTRACE {hit_id} {max_frames}")
    return self._resolve_backtrace_modules(result)  # 复用 Dobby 的模块解析
```

### 19.6 `breakpoint_clear` / `breakpoint_clear_all` — 清除断点

```python
def breakpoint_clear(self, args):
    address = self._address(args["address"], ...)
    return self._json_call(f"BREAKPOINT_CLEAR {address}")

def breakpoint_clear_all(self, _):
    return self._json_call("BREAKPOINT_CLEAR_ALL")
```

---

## 20. 诊断与原始桥接 (diagnostics)

> **Feature flag**: `diagnostics`（隐含，不受开关控制）  
> **工具数**: 3  
> **目的**: 帮助文档、能力查询、原始命令透传

### 20.1 `debug_help` — 原生命令帮助

列出所有自定义原生桥接命令，或返回指定命令的详细说明：

```python
def debug_help(self, args):
    requested = args.get("command", "").strip()
    if requested:
        # 查找 MCP 工具详情
        for tool in tools_for_registry(self.registry):
            if tool["name"].lower() == requested.lower():
                return {"tool": tool["name"], "title": ..., "description": ..., "inputSchema": ...}
    # 否则查询原生命令帮助
    # 命令别名映射: MCP 工具名 → native 命令名
    aliases = {
        "LUA_EXECUTE": "LUA_EXEC",
        "ASSEMBLY_ASSEMBLE": "ASM_ASSEMBLE",
        "DECOMPILE_FUNCTION": "DECOMP_DECOMPILE",
        # ...
    }
    return self._json_call("HELP" + (f" {command}" if command else ""))
```

### 20.2 `runtime_capabilities` — 运行时能力查询

延迟报告各子系统的可用性（不触发懒加载）：

```python
def runtime_capabilities(self, _):
    return self._json_call("CAPABILITIES")
# 返回: {"memory_backend": "ptrace", "luajit": true, "assembly": true,
#        "breakpoint": true, "dobby": true, "il2cpp": true, ...}
```

### 20.3 `raw_hook_call` — 原始命令透传

发送任意单行原生命令，包括未来 hook 添加的新命令。**内置 feature 校验**：

```python
def raw_hook_call(self, args):
    command = _single_line(args["command"], "command").strip()
    native_name = command.split(None, 1)[0].upper()

    # 根据命令前缀匹配 feature flags
    if native_name.startswith("IL2CPP_"):
        if native_name in {"IL2CPP_HOOK", "IL2CPP_HOOK_RETURN", "IL2CPP_UNHOOK"}:
            raw_features = ("il2cpp_metadata", "il2cpp_hook")
        elif native_name == "IL2CPP_INVOKE":
            raw_features = ("il2cpp_metadata", "il2cpp_invoke")
        # ...
    elif native_name.startswith("MEMORY_"):
        # ...
    elif native_name.startswith("DOBBY_"):
        raw_features = ("dobby", "trace") if "TRACE" in native_name else ("dobby",)
    # ... 其余前缀 ...

    # 校验 feature 是否启用
    disabled = [f for f in raw_features if not self.registry.enabled(f)]
    if disabled:
        raise BridgeError(f"raw command {native_name} is disabled: {', '.join(disabled)}")

    return {"response": self._client().call(command, timeout=timeout)}
```

---

## 21. Feature 管理与 Admin Web UI

### 21.1 FeatureRegistry 架构

```python
class FeatureRegistry:
    def __init__(self, config_path):
        self._states = {name: True for name in FEATURES}  # 17 个功能全开
        self._config_path = Path(config_path)              # JSON 持久化路径
        self._listeners = []                                # 变更订阅者

    def require(self, tool_name):
        """工具调用前校验: 检查所有关联 feature 是否启用"""
        disabled = [name for name in tool_features(tool_name) if not self.enabled(name)]
        if disabled:
            raise BridgeError(f"MCP feature disabled: {', '.join(disabled)}")

    def set(self, name, enabled):
        """原子修改 + 持久化 + 通知"""
        self._states[name] = enabled
        self._save()  # 原子写入 JSON
        for listener in self._listeners:
            listener()  # 触发 McpServer 发送 notifications/tools/list_changed
```

### 21.2 Admin Web UI

内置 HTTP 服务（默认端口 27185），提供浏览器可视化控制面板：

**API 端点**：

| 方法 | 路径 | 说明 |
|------|------|------|
| `GET` | `/` | 返回 HTML 控制面板页面 |
| `GET` | `/api/features` | 获取所有功能开关状态（需认证） |
| `POST` | `/api/features/<name>` | 开关单个功能（需认证） |
| `POST` | `/api/features/all` | 全部开启/关闭（需认证） |

**认证机制**：
- 非 loopback 地址必须设置 `--admin-token`
- 支持 `Authorization: Bearer <token>` 和 `X-MCP-Token: <token>` 两种方式

---

## 22. MCP 协议层详解

### 22.1 协议握手

```json
// 请求
{"jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {"protocolVersion": "2025-11-25"}}

// 响应
{
  "jsonrpc": "2.0", "id": 1,
  "result": {
    "protocolVersion": "2025-11-25",
    "capabilities": {"tools": {"listChanged": true}},
    "serverInfo": {
      "name": "zygisk-il2cpp-mcp",
      "title": "Zygisk Runtime MCP Bridge",
      "version": "2.2.1",
      "description": "Controls IL2CPP, native memory, LuaJIT, Dobby, assembly, and breakpoints."
    },
    "instructions": "Use ping first. The target game must be running; ADB forwarding is automatic."
  }
}
```

**协议版本兼容**：支持 `2024-11-05`, `2025-03-26`, `2025-06-18`, `2025-11-25` 四个版本，客户端请求的版本如果不在支持列表中，自动降级到最新版本。

### 22.2 工具列表

```json
// 请求
{"jsonrpc": "2.0", "id": 2, "method": "tools/list"}

// 响应（受 FeatureRegistry 过滤）
{"jsonrpc": "2.0", "id": 2, "result": {"tools": [...]}}
```

### 22.3 工具调用

```json
// 请求
{
  "jsonrpc": "2.0", "id": 3, "method": "tools/call",
  "params": {
    "name": "il2cpp_invoke",
    "arguments": {
      "image": "Assembly-CSharp.dll",
      "namespace": "Game.Logic",
      "class_name": "PlayerController",
      "method": "GetHealth",
      "instance_address": "0x7fff12340000",
      "arguments": []
    }
  }
}

// 成功响应
{
  "jsonrpc": "2.0", "id": 3,
  "result": {
    "content": [{"type": "text", "text": "{\"result\":100,\"type\":\"i32\"}"}],
    "structuredContent": {"result": 100, "type": "i32"}
  }
}

// 错误响应
{
  "jsonrpc": "2.0", "id": 3,
  "result": {
    "content": [{"type": "text", "text": "method not found"}],
    "isError": true
  }
}
```

### 22.4 变更通知

当功能开关变更时，服务端主动推送：

```json
{"jsonrpc": "2.0", "method": "notifications/tools/list_changed"}
```

### 22.5 stdio 传输

```python
def run_stdio(self):
    self._stdio_active = True
    for raw_line in sys.stdin.buffer:       # 逐行读取
        if not raw_line.strip():
            continue
        message = json.loads(raw_line)       # 解析 JSON-RPC
        response = self.handle(message)      # 调度处理
        if response is not None:
            self._write_message(response)    # 写入 stdout + 换行
```

---

## 23. 数据流全链路图

### 23.1 典型调用链（以 il2cpp_invoke 为例）

```
AI Agent
  │  JSON-RPC: tools/call {name: "il2cpp_invoke", arguments: {...}}
  ▼
McpServer.handle()
  │  解析 JSON-RPC, 提取 method="tools/call"
  ▼
ToolDispatcher.call("il2cpp_invoke", args)
  │  1. FeatureRegistry.require("il2cpp_invoke")
  │     → 检查 il2cpp_metadata + il2cpp_invoke 是否启用
  │  2. _notify_mcp_call() → UI_TOAST_NOTIFY (可选)
  │  3. 调用 il2cpp_invoke(args)
  ▼
ToolDispatcher.il2cpp_invoke(args)
  │  1. _lookup_words(args) → [image_hex, ns_hex, class_hex, method_hex, param_count]
  │  2. _address() → instance_address 校验
  │  3. _invoke_token() × N → 编码每个参数
  │  4. 组装命令: "IL2CPP_INVOKE <words> <instance> <argc> <encoded_args...>"
  │  5. _json_call(command) → 调用 native 端
  ▼
HookSocketClient.call("IL2CPP_INVOKE ...")
  │  1. 建立 TCP 连接到 127.0.0.1:27184
  │  2. 发送: "IL2CPP_INVOKE ... \n"
  │  3. 读取响应头: "OK 42\n"
  │  4. 读取 body: '{"result":100,"type":"i32"}\n'
  │  (连接失败时自动 adb forward 重试)
  ▼
Native Hook Service (Android)
  │  Zygisk 注入的 IL2CPP 运行时
  │  il2cpp_runtime_invoke(method, instance, args, exception)
  ▼
返回路径: JSON → TCP → MCP → AI Agent
```

### 23.2 命令编码总表

| MCP 工具 | Native 命令格式 | Feature 依赖 |
|---------|----------------|-------------|
| `ping` | `PING` | connection |
| `get_clipboard` | `GET_CLIPBOARD` | ui |
| `mcp_toast_set_enabled` | `UI_TOAST_SET true/false` | ui |
| `mcp_toast_show` | `UI_TOAST_SHOW <hex_text>` | ui |
| `show_input_box` | `SHOW_INPUT_BOX <title>\|<hint>\|<prefill>` | ui |
| `wait_input` | `WAIT_INPUT <timeout_ms>` | ui |
| `il2cpp_status` | `IL2CPP_STATUS` | il2cpp_metadata |
| `il2cpp_dump_file` | `IL2CPP_DUMP_FILE` | il2cpp_metadata |
| `il2cpp_list_images` | `IL2CPP_IMAGES <limit>` | il2cpp_metadata |
| `il2cpp_list_classes` | `IL2CPP_CLASSES <img> <ns> <class> <filter> <limit>` | il2cpp_metadata |
| `il2cpp_list_methods` | `IL2CPP_METHODS <img> <ns> <class> <filter> <limit>` | il2cpp_metadata |
| `il2cpp_list_fields` | `IL2CPP_FIELDS <img> <ns> <class> <filter> <inh> <off> <lim>` | il2cpp_metadata |
| `il2cpp_search` | `IL2CPP_SEARCH <entity> <query> <img> <ns> <class> <mode> <cs> <off> <lim>` | il2cpp_metadata |
| `il2cpp_find_method` | `IL2CPP_FIND_METHOD <5 lookup words>` | il2cpp_metadata |
| `il2cpp_invoke` | `IL2CPP_INVOKE <5 words> <instance> <argc> <args...>` | il2cpp_metadata + il2cpp_invoke |
| `il2cpp_hook` | `IL2CPP_HOOK <5 words> <replacement>` | il2cpp_metadata + il2cpp_hook |
| `il2cpp_hook_return` | `IL2CPP_HOOK_RETURN <5 words> <kind> <value>` | il2cpp_metadata + il2cpp_hook |
| `memory_read` | `MEMORY_READ <addr> <size>` | memory_read |
| `memory_write` | `MEMORY_WRITE <addr> <hex_bytes>` | memory_write |
| `memory_backend_status` | `MEMORY_BACKEND_STATUS` | memory_maps |
| `memory_list_modules` | `MEMORY_MODULES <name_hex> <limit>` | memory_maps |
| `memory_find_module` | `MEMORY_MODULE_FIND <name_hex> <occ>` | memory_maps |
| `memory_address_info` | `MEMORY_ADDRESS_INFO <addr>` | memory_maps |
| `memory_search` | `MEMORY_SEARCH <start> <end> <pattern> <max> <align> <types>` | memory_search |
| `memory_search_fuzzy` | `MEMORY_SEARCH_FUZZY <start> <end> <size> <max> <align> <types>` | memory_search |
| `memory_filter` | `MEMORY_FILTER <session> <mode> <pattern>` | memory_search |
| `memory_search_results` | `MEMORY_SEARCH_RESULTS <session> <offset> <limit>` | memory_search |
| `memory_search_clear` | `MEMORY_SEARCH_CLEAR <session>` | memory_search |
| `memory_scan_base` | `MEMORY_POINTER_SCAN_MT <start> <end> <target> <ps> <max> <workers> <types>` | memory_maps + memory_search + pointer_chain |
| `dobby_version` | `DOBBY_VERSION` | dobby |
| `dobby_resolve_symbol` | `DOBBY_RESOLVE <image> <symbol>` | dobby |
| `dobby_hook` | `DOBBY_HOOK <target> <replacement>` | dobby |
| `dobby_hook_return` | `DOBBY_HOOK_RETURN <target> <kind> <value>` | dobby |
| `dobby_instrument` | `DOBBY_INSTRUMENT <addr>` | dobby + trace |
| `dobby_trace_get` | `DOBBY_TRACE_GET <addr>` | dobby + trace |
| `dobby_trace_backtrace` | `DOBBY_TRACE_BACKTRACE <addr> <frames>` | dobby + trace |
| `dobby_patch_code` | `DOBBY_PATCH <addr> <hex_bytes>` | dobby |
| `dobby_destroy` | `DOBBY_DESTROY <target>` | dobby |
| `dobby_list_hooks` | `DOBBY_LIST_HOOKS` | dobby |
| `lua_status` | `LUA_STATUS` | lua |
| `lua_execute` | `LUA_EXEC <hex_script>` | lua |
| `lua_reset` | `LUA_RESET` | lua |
| `lua_logs` | `LUA_LOGS <clear>` | lua |
| `assembly_status` | `ASM_STATUS` | assembly |
| `assembly_assemble` | `ASM_ASSEMBLE <hex_instruction>` | assembly |
| `assembly_disassemble` | `ASM_DISASSEMBLE <addr> <size> <max>` | assembly |
| `assembly_patch` | `ASM_PATCH <addr> <hex_instruction>` | assembly |
| `decompiler_status` | `DECOMP_STATUS` | decompiler |
| `decompile_function` | `DECOMP_DECOMPILE <addr> <size> <max_ins> <max_out> <opt> <stop>` | decompiler |
| `breakpoint_status` | `BREAKPOINT_STATUS` | breakpoint |
| `breakpoint_set` | `BREAKPOINT_SET <addr> <type> <length>` | breakpoint |
| `breakpoint_list` | `BREAKPOINT_LIST` | breakpoint |
| `breakpoint_hits` | `BREAKPOINT_HITS <addr> <offset> <limit>` | breakpoint |
| `breakpoint_backtrace` | `BREAKPOINT_BACKTRACE <hit_id> <max_frames>` | breakpoint |
| `breakpoint_clear` | `BREAKPOINT_CLEAR <addr>` | breakpoint |
| `breakpoint_clear_all` | `BREAKPOINT_CLEAR_ALL` | breakpoint |
| `runtime_capabilities` | `CAPABILITIES` | diagnostics |
| `debug_help` | `HELP [command]` | diagnostics |
| `raw_hook_call` | `<user command>` | 动态匹配 |

---

> **文档生成日期**: 2026-09-01  
> **源文件**: `ilbil2cppMCP源项目/mcp/mcp_server.py`  
> **总行数**: 2997 行 | **总工具数**: 70+ | **功能模块数**: 17 + diagnostics
