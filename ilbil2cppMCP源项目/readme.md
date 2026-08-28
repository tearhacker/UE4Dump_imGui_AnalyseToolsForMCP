# Zygisk IL2CPP MCP

把 Android Unity IL2CPP 游戏进程中的运行时查询、方法调用和 Dobby Hook 暴露给 MCP 客户端的 Zygisk 模块。
我要Star⭐ QAQ

## 频道

TG:@il2cppmcp QQ:276342773

## 已实现

- 通过持久目录中的 `apps.txt` 配置多个目标包名，同时匹配应用主进程和 `包名:子进程`。
- 通过持久目录中的 `port.txt` 自定义 MCP/命令 Socket 端口，默认 `27184`。
- 由 Zygisk Root companion 读取配置并通过 IPC 传给目标进程，兼容应用进程无法访问 `/data/adb` 的环境。
- 全新 KernelSU/Magisk WebUI：添加/移除多个包名、修改端口、连接检测、复制 MCP 配置、一键导出 `MCP.zip`。
- IL2CPP：镜像、类、方法枚举，方法定位，静态/实例方法调用，方法 Hook 和固定返回值 Hook。
- IL2CPP Dump：直接写入目标应用私有目录的 `files/zygisk_il2cpp_mcp/il2cpp_dump.cs`，MCP 仅返回是否成功。
- 非 IL2CPP 内存工具：安全读写映射、模块起止地址/重复实例定位、地址反查、字节/类型化搜索及多轮过滤。
- 可选内核内存后端：WebUI 可选择 System、KPM KMA、ditPro、APRead、ioctl hook、Netlink、GT1/GT2、Paradise 或 QX；仅内存读写/搜索走所选后端。
- 搜索：精确多类型搜索、未知值模糊搜索、变化/不变/增大/减小过滤、结果分页，以及内存区域类型多选。
- Dobby：符号解析、原生地址 Hook、固定返回、Instrument 计数、代码 Patch、Destroy 与 Hook 列表。
- 动态调试：内置 LuaJIT+FFI、ARM64 AsmJit 汇编、Capstone 反汇编/指令修改、perf 硬件断点与命中寄存器。
- 可选能力全部懒加载；目标 ABI、内核或运行时不支持时只禁用对应工具，Socket、内存、Dobby 和其他能力继续工作。
- JNI Toast：显示当前 MCP tool 与参数，可通过 MCP 开关或主动显示自定义内容。
- 注入目标启动提示：目标进程初始化时会通过 Toast 显示 `TG: @il2cppmcp`；如果 Android 应用上下文尚未就绪，模块会在启动后短暂重试，不影响目标进程运行。
- 无第三方 Python 依赖的 stdio MCP Server，默认自动执行 `adb forward`。

## 配置

模块安装后可直接通过 WebUI 保存配置，也可手动编辑：

```text
/data/adb/zygisk_il2cpp_mcp/apps.txt
/data/adb/zygisk_il2cpp_mcp/port.txt
/data/adb/zygisk_il2cpp_mcp/memory_backend.txt
/data/adb/zygisk_il2cpp_mcp/driver_node.txt
```

`apps.txt` 每行一个包名，例如：

```text
com.example.game
com.example.anothergame
```

程序会在目标进程启动时读取配置。修改后请彻底结束并重新启动目标游戏。
`driver_node.txt` 通常留空；GT1/QX 使用随机设备名且自动发现失败时，可填写明确的 `/dev/...` 路径。

## MCP 启动

```powershell
python mcp/mcp_server.py --port 27184
```

如果 MCP Server 就运行在目标 Android 设备上，使用直连模式，不需要 ADB 端口转发：

```sh
python mcp/mcp_server.py --port 27184 --direct
```

默认模式会先尝试直连 `127.0.0.1:27184`，连接失败后才自动执行 `adb forward`；`--direct` 会关闭这一回退行为。

客户端配置示例：

```json
{
  "mcpServers": {
    "zygisk-il2cpp": {
      "command": "python",
      "args": ["D:/path/Zygisk-il2cpp-mcp/mcp/mcp_server.py", "--port", "27184"]
    }
  }
}
```

完整工具说明见 [mcp/README.md](mcp/README.md)。

> **本地目录提示**：上游仓库里 PC 侧文件放在 `mcp/` 子目录，本目录是**平铺**的。
> 因此上面的 `python mcp/mcp_server.py` 在本机应写成 `python mcp_server.py`，
> `mcp/README.md` 上游未提供，其等价内容见文末「代码级补充」。

## 代码级补充

以下内容来自对 `mcp_server.py`（1978 行）的逐行阅读，官方 README 未覆盖。

### 一次调用到底发生了什么

```
① MCP 客户端 ──stdio 一行 JSON──> handle()            （McpServer，第 1878 行）
② tools/call ──> dispatcher.call(name, args)          （第 1054 行，RLock 串行）
③ _notify_mcp_call() 先发一次 UI_TOAST_NOTIFY         （第 1129 行，失败静默）
④ 具体方法 → 拼出单行原生命令（hex 化 / 地址归一化 / struct 编码）
⑤ HookSocketClient.call() ──> socket 127.0.0.1:27184
⑥ 原生端回 `OK <len>\n<body>` 或 `ERR <msg>`
⑦ _json_call() 解析 body 为 dict，包成 content + structuredContent 返回
```

连接失败时：若 host ∈ {`127.0.0.1`, `localhost`, `::1`} 且未禁用，**自动执行一次 `adb forward tcp:P tcp:P` 后重试一次**（第 120–129 行）。这是默认模式的行为，`--direct` 会关掉这个回退。

### 原生协议

- 请求：一行命令 + `\n`，如 `MEMORY_READ 0x1000 16`
- 响应：`OK <body字节数>\n<body>`（body 多为 JSON）／`ERR <错误信息>`
- header 上限 16 KB，body 上限 4 MB
- 不需要改 Python 也能探新命令：`raw_hook_call` 直发任意单行；`debug_help` 查原生命令用法

### 三条编码约定（改代码必守）

| 约定 | 实现 | 位置 |
|---|---|---|
| 字符串参数一律 UTF-8 **hex** 传输 | `_hex_text()`，空串返回 `-` | 1245 行 |
| 地址 `int(x, 0)` 解析后统一 `0x...` 下发 | `_address()`，同时接受 `0x1000` 与十进制 | 1258 行 |
| 类型化值小端 `struct` pack | `MEMORY_VALUE_FORMATS`（bool/i8~i64/u8~u64/f32/f64/ptr32/ptr64） | 312 行 |

`SHOW_INPUT_BOX` 是唯一直用 `|` 分隔的：`SHOW_INPUT_BOX 标题|提示|预填`，三个字段都走 `_single_line(..., allow_pipe=False)` 校验（第 1208 行）。

### 工具清单（62 个，与 `TOOLS` 表一一对应）

| 分组 | 数量 | 工具 |
|---|---|---|
| 连接 / UI | 12 | `ping` `connection_info` `configure_connection` `get_clipboard` `mcp_toast_status` `mcp_toast_set_enabled` `mcp_toast_show` `show_input_box` `wait_input` `input_and_wait` `push_input_result` `raw_hook_call` |
| IL2CPP | 10 | `il2cpp_status` `il2cpp_dump_file` `il2cpp_list_images` `il2cpp_list_classes` `il2cpp_list_methods` `il2cpp_find_method` `il2cpp_invoke` `il2cpp_hook` `il2cpp_hook_return` `il2cpp_unhook` |
| Memory | 16 | `memory_backend_status` `memory_read` `memory_write` `memory_read_value` `memory_write_value` `memory_list_modules` `memory_find_module` `memory_address_info` `memory_search` `memory_search_value` `memory_search_exact` `memory_search_fuzzy` `memory_filter` `memory_filter_value` `memory_search_results` `memory_search_clear` |
| Dobby | 9 | `dobby_version` `dobby_resolve_symbol` `dobby_hook` `dobby_hook_return` `dobby_instrument` `dobby_trace_get` `dobby_patch_code` `dobby_destroy` `dobby_list_hooks` |
| LuaJIT | 5 | `lua_status` `lua_execute` `lua_reset` `lua_logs` `debug_help` |
| Assembly | 4 | `assembly_status` `assembly_assemble` `assembly_disassemble` `assembly_patch` |
| Breakpoint | 6 | `breakpoint_status` `breakpoint_set` `breakpoint_list` `breakpoint_hits` `breakpoint_clear` `breakpoint_clear_all` |

### 值得记住的实现细节

- **错误不走 JSON-RPC error**：所有异常兜成 `isError: true` 的 tool result（第 1922 行），LLM 拿到的是人话而不是被客户端吞掉；成功时额外回 `structuredContent`。
- **协议兼容**：支持 `2024-11-05` / `2025-03-26` / `2025-06-18` / `2025-11-25`，客户端报不认识的版本就静默用最新（第 1892 行）。
- **超时下限提权**：`memory_search` / `search_fuzzy` 强制 `max(timeout, 300)`、`il2cpp_invoke` 30 s、`memory_filter` 60 s、`il2cpp_dump_file` 固定 300 s。
- **Toast 开关**：`mcp_toast_set_enabled` 控制自动提示；`connection_info`、`configure_connection`、`mcp_toast_set_enabled`、`mcp_toast_show` 四个工具自己**不会**触发提示（第 1130 行）。
- **能力懒加载**：`runtime_capabilities` / `*_status` 只查询不初始化，LuaJIT VM 首次 `lua_execute` 才创建。

### 已知坑

1. **单线程串行**：`run_stdio` 逐行读、逐个同步处理。`wait_input` 最长 300 s、`memory_search` 下限 300 s，期间整条 MCP 通道阻塞。要并发就得改后台线程 + 任务 id 轮询。
2. **每次调用多一个往返**：`_notify_mcp_call` 在真正命令前先发一次 Toast 通知，延迟翻倍；原生端响应慢时会连带拖长。
3. **测试跑不起来**：`test_mcp_server.py` 第 8 行是 `from mcp.mcp_server import ...`，需按 zip 内的结构放成 `mcp/mcp_server.py` 才能在上级目录跑 `python -m unittest`。
4. `memory_search_exact` 是**串行**发 N 次独立搜索（每次都带 300 s 下限），类型选多了很慢。
5. `il2cpp_dump_file` 只回成功与否，dump 文件在 App 私有目录，需自己 `run-as` 或 root 取回。

## 风险提示

此项目面向你有权调试的应用。错误的实例地址、replacement 地址、返回 ABI 或机器码 Patch 会直接导致目标进程崩溃。代码 Patch 不会由 `DobbyDestroy` 自动恢复。
