#pragma once

// 通信协议常量 —— 与 docs/mcp-protocol.md 严格对应。
// 两端（设备端 C++ / PC 侧 Python）任一改动必须先改协议文档。

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace UmtMcp
{
// ---------------------------------------------------------------- 协议
constexpr int kProtocolVersion = 1;
constexpr uint16_t kDefaultPort = 35515;

// 默认监听地址 —— 🔴 安全红线：只监听回环，默认绝不 0.0.0.0。
// 运行期可被 kBindConfigPath 指向的配置文件覆盖。
constexpr const char *kBindAddress = "127.0.0.1";

// 🔴 可选的运行期监听地址覆盖文件。
// 文件内容一行，即 bind 地址（如 192.168.1.23 或 0.0.0.0）。
// 文件不存在 / 为空 / 非法 → 保持 kBindAddress（127.0.0.1），安全红线不放松。
// 放开到非回环 = 把 root 级内存读写接口暴露到网络，只在可信网络下用。
constexpr const char *kBindConfigPath = "/sdcard/UnrealMemoryTools/mcp_bind.conf";

// ---------------------------------------------------------------- 心跳与超时（协议 §8.1）
constexpr int kHeartbeatIntervalSec = 2;   // 设备端心跳间隔
constexpr int kHeartbeatTimeoutSec = 10;   // PC 侧判活阈值（≈5 个周期）
constexpr int kCommandTimeoutSec = 120;    // 单命令硬超时，绝不无限执行

// ---------------------------------------------------------------- 帧格式（NDJSON）
constexpr char kFrameDelimiter = '\n';
constexpr size_t kMaxFrameSize = 1024 * 1024;  // 单帧 1MB 上限
constexpr size_t kRecvChunkSize = 8192;

// ---------------------------------------------------------------- 内部循环节奏
constexpr int kSelectTimeoutMs = 100;      // select 超时,兼顾心跳与响应检查
constexpr int kPollSleepMs = 20;           // 等待响应时的轮询间隔
// 会话超时(同步到 PtraceSessionRegistry)
inline const int kSessionIdleTimeoutMs = 30000;

// ---------------------------------------------------------------- 错误码（协议 §5）
namespace Err
{
inline const char *const kProtocolMismatch = "E_PROTOCOL_MISMATCH";
inline const char *const kBadJson = "E_BAD_JSON";
inline const char *const kUnknownCmd = "E_UNKNOWN_CMD";
inline const char *const kBadArgs = "E_BAD_ARGS";
inline const char *const kTimeout = "E_TIMEOUT";
inline const char *const kInternal = "E_INTERNAL";

        // 执行层错误码：以 isError tool result 返回，非 JSON-RPC error（协议 §5 分层）
    inline const char *const kNotAttached = "E_NOT_ATTACHED";  // 未 attach / kMgr 未初始化
    inline const char *const kReadFailed = "E_READ_FAILED";
    inline const char *const kWriteFailed = "E_WRITE_FAILED";
    inline const char *const kProbeFailed = "E_PROBE_FAILED";
    inline const char *const kNotReady = "E_NOT_READY";      // 前置条件未满足（如 probe 未完成）
    inline const char *const kNotFound = "E_NOT_FOUND";      // 符号/资源未找到
    inline const char *const kDecodeFailed = "E_DECODE_FAILED"; // 指令解码失败
    inline const char *const kDecompileFailed = "E_DECOMPILE_FAILED"; // 反编译失败
    inline const char *const kCancelled = "E_CANCELLED";
    inline const char *const kPtraceFailed = "E_PTRACE_FAILED"; // ptrace attach/detach 或远程调用失败（F 组）
    inline const char *const kMapStale = "E_MAP_STALE";        // maps revision 已变化
    inline const char *const kSessionStale = "E_SESSION_STALE"; // 候选/搜索 session 已失效
}  // namespace Err

// ------------------------------------------------ 返回值约定（协议 §5，唯一对齐事实）
// 所有经 CommandDispatcher 执行的命令，响应帧统一形如（一行 NDJSON）：
//
//   成功：  {"id":<N>, "ok":true,  "data":{ ... }}      // data 缺省为 {}
//   执行层错误（isError tool result）：
//          {"id":<N>, "ok":false, "error":{"code":<ERR>, "msg":<文本> [, "detail":{...}]}}
//
// 分层原则：
//   - 协议层错误（握手/auth/帧格式）由 CommandServer 直接回，带 "type" 字段，未必带 id；
//   - 执行层错误（命令逻辑）由 handler 抛 HandlerError(code, msg)，Dispatcher 捕获后
//     包成上面的 ok:false 帧。code 必须取自 Err 命名空间，禁止 handler 现场造字符串。
//   - 耗时档位 / 长任务 / brief 分级 / cursor 分页 等高级约定尚未落地（见 MemoryHelpers.hpp TODO）。
struct HandlerError : public std::runtime_error
{
    std::string code;
    HandlerError(const char *errCode, const std::string &msg) : std::runtime_error(msg), code(errCode) {}
};

}  // namespace UmtMcp
