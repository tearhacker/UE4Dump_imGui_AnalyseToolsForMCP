"""MCP 工具层：camelCase 工具名 → 设备端 UPPER_SNAKE 命令。

这一层只负责「名字 → 参数 → 调用」，不碰 socket（那是 bridge 的事）。

命名说明（协议 §6）：
    MCP 工具名用 camelCase，设备端命令名用 UPPER_SNAKE，是两层，不混。

⚠️ 命名不一致（以设备端代码实际注册名为准，否则必然 E_UNKNOWN_CMD）：
    协议文档 §6 写 `MEMORY_WRITE`，设备端注册的却是 `WRITE_MEMORY`；
    而 `MEMORY_READ` / `MEMORY_READ_VALUE` 又是"动作在前"的写法。
    设备端 D 组实际是：MEMORY_READ / MEMORY_READ_VALUE / WRITE_MEMORY / READ_STRING
    —— 读和写的语序是反的。三条都在下面 _CMD_ALIASES 里显式映射。
"""

from __future__ import annotations

import json
from typing import Any

from mcp.server.fastmcp.exceptions import ToolError

from . import bridge as br
from . import config, protocol as proto

# 函数名(snake_case) → 设备端命令名。仅列出不能直接转大写的（D 组读/写语序是反的）
_CMD_ALIASES = {
    "read_memory": "MEMORY_READ",             # 🔴 不是 READ_MEMORY
    "read_memory_value": "MEMORY_READ_VALUE", # 🔴 不是 READ_MEMORY_VALUE
    "write_memory": "WRITE_MEMORY",           # 🔴 协议文档写的是 MEMORY_WRITE
}

# 设备端实际注册的全部命令（src/executable.cpp SetupMcpCommands，43 条）。
# 用作启动自检，防止工具名改动后静默发出不存在的命令名。
DEVICE_COMMANDS = frozenset({
    "PING", "LIST_PROCESSES", "GET_LOGS", "GET_CAPABILITIES",
    "MEMORY_READ", "MEMORY_READ_VALUE", "READ_STRING", "LIST_MODULES",
    "DECODE_ADRL", "WRITE_MEMORY", "SCAN_PATTERN", "DISASSEMBLE",
    "BEGIN_ATTACH_SESSION", "END_ATTACH_SESSION", "CALL_REMOTE_FUNCTION",
    "CALL_REMOTE_FUNCTION_BATCH", "ALLOC_SCRATCH",
    "SCAN_GNAMES", "SAMPLE_GNAMES", "SCAN_OBJECTS", "SAMPLE_OBJECTS",
    "SEARCH_CLASSES", "DESCRIBE_CLASS", "INSPECT_OBJECT", "RESOLVE_SYMBOL",
    "FOLLOW_POINTER_CHAIN", "SELECT_PROCESS", "ATTACH",
    "START_PROBE", "DETECT_UE_VERSION", "START_DUMP", "DUMP_UNREAL_LIBRARY",
    "GET_PROBE_RESULTS", "GET_PROBE_STATUS", "GET_DUMP_STATUS",
    "LOCATE_ENGINE_GLOBALS", "DUMP_SDK", "ANALYZE_CLASS", "SCAN_CANDIDATES",
    "LIST_OUTPUT_FILES", "READ_OUTPUT_FILE", "CANCEL_JOB", "APPLY_PROBE_OVERRIDES",
})


def _cmd(func_name: str) -> str:
    """函数名（snake_case）→ 设备端命令名（UPPER_SNAKE）。

    绝大多数就是直接转大写，只有 D 组三条因语序不同需查别名表。
    """
    return _CMD_ALIASES.get(func_name, func_name.upper())


def to_camel_case(name: str) -> str:
    """snake_case → camelCase。MCP 对外暴露的工具名用 camelCase（协议 §6）。"""
    head, *rest = name.split("_")
    return head + "".join(word.capitalize() for word in rest)


def _dev(tool_name: str, timeout: float | None = None, **args: Any) -> str:
    """发一条设备端命令，返回给 AI 的 JSON 文本。

    None 参数一律不下发，让设备端用自己的默认值。
    """
    payload = {k: v for k, v in args.items() if v is not None}
    cmd = _cmd(tool_name)

    kwargs = {} if timeout is None else {"timeout": timeout}
    try:
        data = br.call(cmd, payload, **kwargs)
    except proto.UmtExecutionError as exc:
        # 执行层失败 → isError tool result（协议 §5）：是设备端失败了，不是我调错了
        raise ToolError(
            f"设备端执行失败 [{exc.code}] {exc.msg}"
            + (f"\n详情: {json.dumps(exc.detail, ensure_ascii=False)}" if exc.detail else "")
        ) from exc
    except proto.UmtProtocolError as exc:
        # 协议层错误。规范说应转 JSON-RPC error，但 FastMCP 抛非 ToolError 异常
        # 会让 AI 只拿到一个"内部错误"，反而丢掉最关键的排障信息（比如 token 不对）。
        # 这里仍走 isError 结果，但打上 [协议层] 前缀以保留分层语义。
        raise ToolError(f"[协议层错误] {exc.code}: {exc.msg}") from exc
    except (proto.UmtConnectionError, proto.UmtTimeoutError) as exc:
        raise ToolError(f"[连接/超时] {exc}") from exc

    return json.dumps(data, ensure_ascii=False, indent=2)


# ============================================================ A 连接与能力
def ping() -> str:
    """连通性检查。返回设备端 build / 协议版本 / 连接状态。

    排障第一步：连不上先跑 adb forward 再试。
    """
    return _dev("ping")


def get_capabilities() -> str:
    """列出设备端当前支持的全部命令，以及 ptrace 是否可用。

    F 组（远程调用）在 ptrace 不可用时不会出现在这里。
    """
    return _dev("get_capabilities")


# ============================================================ B 进程
def list_processes(dedicated_only: bool | None = None) -> str:
    """枚举设备上的 UE 进程。

    dedicated_only=true 只返回已适配的专属游戏（profile 精确匹配）；
    默认 false 时还会包含 /proc 扫描到的通用 UE4/UE5 进程。
    """
    return _dev("list_processes", dedicatedOnly=dedicated_only)


def select_process(package: str, pid: int | None = None) -> str:
    """选中目标进程并 attach。

    🔴 切进程会让旧的探针结果立即失效，之后必须重新 startProbe。
    """
    return _dev("select_process", package=package, pid=pid)


def attach(pid: int) -> str:
    """直接按 pid attach（不走进程枚举）。selectProcess 失败时的退路。"""
    return _dev("attach", pid=pid)


# ============================================================ C 流程与产出
def start_probe(wait_ms: int | None = None) -> str:
    """启动引擎探测（定位 GNames / GUObjectArray / 偏移表）。

    这是几乎所有引擎语义命令的前置条件。耗时较长，之后用 getProbeStatus 轮询。
    """
    return _dev("start_probe", waitMs=wait_ms)


def get_probe_status() -> str:
    """查询探测进度（phase / percent / 是否完成）。startProbe 之后轮询这个。"""
    return _dev("get_probe_status")


def get_probe_results() -> str:
    """取探测得到的偏移表与结构体布局。需探测完成后调用。"""
    return _dev("get_probe_results")


def start_dump(wait_ms: int | None = None) -> str:
    """启动 SDK 转储。需先完成 startProbe。

    🔴 会先清空设备端输出目录（/sdcard/UnrealMemoryTools/<pkg>/）。
    """
    return _dev("start_dump", waitMs=wait_ms)


def get_dump_status() -> str:
    """查询转储进度。startDump / dumpSdk 之后轮询这个。"""
    return _dev("get_dump_status")


def dump_unreal_library() -> str:
    """单独转储 libUE4.so / libUnreal.so 本体。

    抓不到符号时用它拿原始 so，再在 PC 侧离线分析。
    """
    return _dev("dump_unreal_library")


def dump_sdk(wait_ms: int | None = None) -> str:
    """一键跑完 probe + dump 并产出完整 SDK。

    返回 jobId，可用 getDumpStatus 或 cancelJob 跟踪/取消。
    """
    return _dev("dump_sdk", waitMs=wait_ms)


def get_logs(since_index: int | None = None, max_lines: int | None = None) -> str:
    """拉取设备端日志（环形缓冲，上限 1500 行，超出丢最旧）。

    用 since_index 增量拉取，避免重复读。默认 50 行。
    """
    return _dev("get_logs", sinceIndex=since_index, maxLines=max_lines)


def list_output_files() -> str:
    """列出设备端 /sdcard/UnrealMemoryTools 下已产出的文件。

    🔴 大文件（SDK_Classes.hpp 等）要用 adb pull 拉，不要走 readOutputFile 整读。
    """
    return _dev("list_output_files")


def read_output_file(path: str, max_bytes: int | None = None) -> str:
    """读取设备端产物文件内容。

    🔴 只用于小文件；几十 MB 的 SDK 文件一律走 adb pull，禁止整读进上下文。
    """
    return _dev("read_output_file", path=path, maxBytes=max_bytes)


def cancel_job(job_id: str | None = None) -> str:
    """取消正在跑的重活。不传 jobId 则取消当前活跃任务。"""
    return _dev("cancel_job", jobId=job_id)


def apply_probe_overrides(overrides: dict[str, str]) -> str:
    """手动指定偏移地址，覆盖自动探测结果。

    key 为偏移名（如 "GNames"），value 为 "0x..." 字符串。
    🔴 必须在 startProbe 之前调用。
    """
    return _dev("apply_probe_overrides", overrides=overrides)


# ============================================================ D 内存原语
def read_memory(address: str, size: int) -> str:
    """读目标进程内存，返回 hex。

    address 用 "0x..." 字符串。size 上限 4096（单响应 ≤4K token 硬约束）。
    """
    return _dev("read_memory", address=address, size=size)


def read_memory_value(address: str, value_type: str) -> str:
    """按类型读一个内存值。

    value_type: bool/i8/u8/i16/u16/i32/u32/i64/u64/f32/f64/ptr32/ptr64
    """
    return _dev("read_memory_value", address=address, type=value_type)


def read_string(address: str, max_length: int | None = None, wide: bool | None = None) -> str:
    """读内存中的字符串。wide=true 按 UTF-16 读（FString）。"""
    return _dev("read_string", address=address, maxLength=max_length, wide=wide)


def write_memory(address: str, hex_bytes: str, confirm_dangerous: bool = False) -> str:
    """写目标进程内存。🔴 危险操作。

    会失败如果目标映射不可写。confirm_dangerous 必须为 true。
    写前建议先用 scanPattern / disassemble 确认地址，并用 listModules 确认区间可写。
    """
    if not confirm_dangerous:
        raise ToolError(
            "写内存是危险操作，必须显式传 confirm_dangerous=true。\n"
            "写前请确认：1) 地址正确（先 scanPattern/disassemble 验证）  "
            "2) 该区间可写（先 listModules 确认）\n"
            "写错会直接把游戏搞崩。"
        )
    return _dev("write_memory", address=address, hex=hex_bytes, confirmDangerous=True)


def scan_pattern(pattern: str, module: str | None = None,
                 max_results: int | None = None) -> str:
    """按 IDA 风格 pattern 扫描内存（"FF DD ? 99 CC ? 00"）。

    不指定 module 则扫整个目标进程。耗时较长。
    """
    return _dev("scan_pattern", pattern=pattern, module=module, maxResults=max_results)


def list_modules() -> str:
    """列出目标进程已加载的模块（基址 / 大小 / 路径）。"""
    return _dev("list_modules")


def resolve_symbol(symbol: str, module: str | None = None) -> str:
    """按符号名解析地址。先查动态符号表，失败回退调试符号。

    🔴 strip 过的 ELF 查不到符号，此时应回退 scanPattern。
    """
    return _dev("resolve_symbol", symbol=symbol, module=module)


# ============================================================ E 理解层
def decode_adrl(address: str) -> str:
    """解码 ARM64 ADRP/ADR+LDR 指令序列，算出它引用的绝对地址。"""
    return _dev("decode_adrl", address=address)


def disassemble(address: str, count: int | None = None) -> str:
    """反汇编指定地址处的 ARM64 指令。

    走设备端自研解码器（不依赖 capstone）。未识别指令返回 "???"。
    """
    return _dev("disassemble", address=address, count=count)


# ============================================================ G 引擎语义
def detect_ue_version() -> str:
    """探测目标进程的 UE 版本（UE4.x / UE5.x）。"""
    return _dev("detect_ue_version")


def sample_gnames(start_index: int | None = None, count: int | None = None) -> str:
    """抽样读 GNames/FNamePool 条目。快命令，先看样本再决定要不要全扫。"""
    return _dev("sample_gnames", startIndex=start_index, count=count)


def scan_gnames() -> str:
    """全量扫描 GNames。

    🔴 分钟级重活。当前设备端 isFast 未接线（见 docs/api/12 §5.3.1），
    执行期间会冻结设备端 UI 与心跳，PC 侧可能因 10s 无心跳判定假死。
    能用 sampleGnames 解决就别跑全扫。
    """
    return _dev("scan_gnames")


def sample_objects(start_index: int | None = None, count: int | None = None) -> str:
    """抽样读 GUObjectArray 对象。快命令。"""
    return _dev("sample_objects", startIndex=start_index, count=count)


def scan_objects() -> str:
    """全量扫描 GUObjectArray。

    🔴 分钟级重活，同 scanGnames 的冻结风险。优先用 sampleObjects。
    """
    return _dev("scan_objects")


def search_classes(query: str, max_results: int | None = None) -> str:
    """按名字模糊搜索 UClass。支持子串匹配。"""
    return _dev("search_classes", query=query, maxResults=max_results)


def describe_class(class_name: str) -> str:
    """取一个 UClass 的完整定义：字段（含继承）、函数签名、Size/对齐/CDO。

    字段列表会递归父类，不是只看当前层。
    """
    return _dev("describe_class", className=class_name)


def inspect_object(address: str) -> str:
    """读一个 UObject 实例的运行时字段值。

    🔴 Array/Set/Map 只回 Count/Max/Data 摘要，不展开元素。
    """
    return _dev("inspect_object", address=address)


# ============================================================ H 高层用例
def locate_engine_globals() -> str:
    """一站式定位引擎全局变量（GWorld / GEngine / GNames / GUObjectArray）。"""
    return _dev("locate_engine_globals")


def analyze_class(class_name: str) -> str:
    """深度分析单个类（内部会跑 probe + dump）。

    🔴 重活，耗时不可控，用 getDumpStatus 跟踪进度。
    """
    return _dev("analyze_class", className=class_name)


# ============================================================ I 复合操作
def follow_pointer_chain(base_address: str, offsets: list[int]) -> str:
    """按偏移链逐级解引用：base → +off[0] → +off[1] → ...

    offsets 支持负数索引表示数组下标（如 -1 表示第 1 个元素）。
    """
    return _dev("follow_pointer_chain", baseAddress=base_address, offsets=offsets)


def scan_candidates(max_candidates: int | None = None, brief: bool | None = None) -> str:
    """扫描并列出可分析的目标候选（进程 / 模块 / 关键地址）。

    brief=true 只返回统计与少量摘要，省 token。默认 50 条上限。
    """
    return _dev("scan_candidates", maxCandidates=max_candidates, brief=brief)


# ============================================================ F 远程调用（ptrace）
def begin_attach_session(max_hold_ms: int | None = None) -> str:
    """开启一个 ptrace 附着会话（有状态）。

    同一目标进程只允许一个会话，重复开启会被拒。
    推荐优先用 call_remote_function_batch（无状态，不会泄漏）。
    """
    return _dev("begin_attach_session", maxHoldMs=max_hold_ms)


def end_attach_session() -> str:
    """结束当前 ptrace 会话。🔴 用完必须调用，否则目标进程会一直被挂住。"""
    return _dev("end_attach_session")


def call_remote_function(address: str, arg_types: list[str] | None = None,
                         args: list[Any] | None = None,
                         return_type: str | None = None) -> str:
    """在当前 ptrace 会话内远程调用一个函数（需先 begin_attach_session）。

    args 元素支持两种形式：数字直接传值，"0x..." 字符串按地址处理。
    最多 8 个参数（aarch64 寄存器上限）。
    """
    return _dev("call_remote_function", address=address, argTypes=arg_types,
                args=args, returnType=return_type)


def call_remote_function_batch(calls: list[dict[str, Any]],
                               confirm_dangerous: bool = False) -> str:
    """批量远程调用函数（无状态，主推）。内部 attach → N 次调用 → detach。

    🔴 危险操作，confirm_dangerous 必须为 true。

    返回陷阱靠 SIGSEGV/SIGILL 判定，无法区分"陷阱正常触发"与"被调函数自己崩了"
    —— 每个结果都附 rawHex，必须自行校验返回值是否合理。
    """
    if not confirm_dangerous:
        raise ToolError(
            "远程调用会直接操控目标进程，必须显式传 confirm_dangerous=true。\n"
            "调用前请确认函数地址与参数正确（先用 disassemble/resolveSymbol 验证）。"
        )
    return _dev("call_remote_function_batch", calls=calls, confirmDangerous=True)


def alloc_scratch(size: int, confirm_dangerous: bool = False) -> str:
    """在目标进程内 mmap 一块可读写内存（供远程调用当缓冲区）。

    🔴 无对应的释放命令，分配的内存随目标进程生命周期存在。
    """
    if not confirm_dangerous:
        raise ToolError("在目标进程内分配内存需要显式传 confirm_dangerous=true。")
    return _dev("alloc_scratch", size=size, confirmDangerous=True)


# ---------------------------------------------------------------- 注册表
# 顺序即挂载顺序。前 RESIDENT_TOOLS 个常驻，其余可按需暴露。
TOOLS: list[Any] = [
    # A
    ping, get_capabilities,
    # B
    list_processes, select_process, attach,
    # C
    start_probe, get_probe_status, get_probe_results,
    start_dump, get_dump_status, dump_sdk, dump_unreal_library,
    get_logs, list_output_files, read_output_file, cancel_job, apply_probe_overrides,
    # D
    read_memory, read_memory_value, read_string, write_memory,
    scan_pattern, list_modules, resolve_symbol,
    # E
    decode_adrl, disassemble,
    # G
    detect_ue_version, sample_gnames, scan_gnames,
    sample_objects, scan_objects,
    search_classes, describe_class, inspect_object,
    # H
    locate_engine_globals, analyze_class,
    # I
    follow_pointer_chain, scan_candidates,
    # F
    begin_attach_session, end_attach_session,
    call_remote_function, call_remote_function_batch, alloc_scratch,
]


def self_check() -> list[str]:
    """启动自检：每个工具推导出的命令名必须真实存在。

    这是为了防止"工具名改了 / 设备端改名了 / camelCase 推导规则有边界情况"
    这类问题留到真机上才以 E_UNKNOWN_CMD 的形式暴露。

    返回问题列表，空列表表示通过。
    """
    problems: list[str] = []
    covered: set[str] = set()

    for fn in TOOLS:
        cmd = _cmd(fn.__name__)
        covered.add(cmd)
        if cmd not in DEVICE_COMMANDS:
            problems.append(f"工具 {fn.__name__!r} 推导出未知命令 {cmd!r}")

    missing = DEVICE_COMMANDS - covered
    if missing:
        problems.append(f"设备端有但 PC 侧未暴露的命令：{sorted(missing)}")

    if len(TOOLS) > config.MAX_TOOLS:
        problems.append(f"工具数 {len(TOOLS)} 超过硬上限 {config.MAX_TOOLS}")

    return problems
