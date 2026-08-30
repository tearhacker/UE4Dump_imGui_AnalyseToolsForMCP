#pragma once

// 设备端 MCP 基础工具集（地址解析 / 字节编码 / 格式化）。
//
// 定位：MCP 命令 handler 的「地基」——纯无状态工具函数，不依赖任何 UMT 全局
// （kMgr / UEWrappers / gDumpUiState 均非线程安全），可在任意线程安全调用。
//
// 被 src/executable.cpp 的 SetupMcpCommands() 中多个命令引用：
//   MEMORY_READ / MEMORY_READ_VALUE / READ_STRING / LIST_MODULES /
//   DECODE_ADRL / RESOLVE_SYMBOL / FOLLOW_POINTER_CHAIN。
//
// 命名空间统一为 UmtMcp，与 Protocol/CommandDispatcher/CommandServer 保持一致。

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace UmtMcp
{
// 指针链偏移解析结果
struct ParsedOffset
{
    long long offset = 0;       // 字节偏移（可负，表示向前回溯）
    long long arrayIndex = -1;  // 数组下标；>=0 时按指针宽度步进，默认 -1 表示无下标
};

// 解析地址字符串。支持：
//   "0x1234" / "0X1234" —— 0x 前缀十六进制
//   "1a2b"             —— 纯十六进制（含 a-f/A-F 即按 hex 解析）
//   "4096"             —— 纯十进制
// 成功返回 true 并写入 out；空串 / 非法字符返回 false。
bool ParseAddress(const std::string &s, uintptr_t &out);

// 解析偏移/步进字符串。支持：
//   "+0x10" / "-8" / "0x10" —— 单偏移
//   "0x10[2]"          —— 偏移 + 数组下标（取下标后按指针宽度步进）
// 成功返回 true。
bool ParseOffset(const std::string &s, ParsedOffset &out);

// 地址格式化为 "0x" + 小写十六进制（固定适配指针宽度）。
std::string FormatAddress(uintptr_t addr);

// 字节序列转大写十六进制串（每字节两位，无分隔），用于内存/值回显。
std::string BytesToHex(const uint8_t *data, size_t len);

// 取值类型名 → 字节大小；未知类型返回 0（handler 据此报 E_BAD_ARGS）。
// 支持：bool/i8/u8/i16/u16/i32/u32/i64/u64/f32/f64/ptr32/ptr64
size_t ValueTypeSize(const std::string &type);

// UTF-32(wchar_t) → UTF-8。Android wchar_t 为 4 字节，不依赖 codecvt/ICU。
std::string WstringToUtf8(const std::wstring &ws);

// 权限位 → "rwx" / "rw-" 形式字符串（LIST_MODULES 回显）。
std::string FormatPermissions(bool readable, bool writeable, bool executable);

// 大小写十六进制串（可含空格 / 0x 前缀）解码为字节序列。非法字符返回 false。
// 用于 WRITE_MEMORY 的 hex 入参（每字节两位，长度须为偶数）。
bool HexToBytes(const std::string &s, std::vector<uint8_t> &out);

// 规范化 IDA pattern：把 "??" 合并为 "?"、转大写、压缩空白为单空格、去首尾空格。
// 用于 SCAN_PATTERN 的 pattern 入参（findIdaPatternAll 期望 "FF DD ? 99 CC ? 00" 格式）。
std::string NormalizeIdaPattern(const std::string &pattern);

// ---------------------------------------------------------------------------
// TODO(2026-08-31 核对后重写)：下面这份清单**只剩 2 项为真**，此前列的 13 条命令
// （START_PROBE / GET_PROBE_STATUS / GET_PROBE_RESULTS / SELECT_PROCESS /
//  START_DUMP / GET_DUMP_STATUS / DUMP_UNREAL_LIBRARY / LOCATE_ENGINE_GLOBALS /
//  SCAN_GNAMES / SAMPLE_GNAMES / ANALYZE_CLASS / CALL_REMOTE_FUNCTION_BATCH /
//  ATTACH）已全部注册并实现在 src/executable.cpp SetupMcpCommands()，勿再当成待办。
// 其中 LOCATE_ENGINE_GLOBALS / SCAN_GNAMES / SAMPLE_GNAMES / ANALYZE_CLASS 走的是
// **内联 handler** 路线，未新建 SDKQuery 层（见 docs/api/13 §0.3）。
//
// 仍未实现：
//   1. INVALIDATE_PROBE —— 切进程后显式作废旧探针。
//      注：SELECT_PROCESS 内部已调 InvalidateProbeReuse（executable.cpp 编排层），
//      缺的只是把它单独暴露成命令。
//   2. 协议 §8 高级约定：长轮询(waitMs hold) / 日志推送帧 / 心跳 activeJobId /
//      brief 响应分级 / cursor 分页 / 流式分块 / adb pull 数据通道。
//      建议：先打通 M2 真机验证再补。
//      jobId 例外：JobRegistry(:161-208) 已建，DUMP_SDK 已返回 jobId；
//      START_PROBE / START_DUMP 尚未接入。
// ---------------------------------------------------------------------------
}  // namespace UmtMcp
