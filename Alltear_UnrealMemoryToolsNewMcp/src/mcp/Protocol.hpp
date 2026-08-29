#pragma once

// 通信协议常量 —— 与 docs/mcp-protocol.md 严格对应。
// 两端（设备端 C++ / PC 侧 Python）任一改动必须先改协议文档。

#include <cstddef>
#include <cstdint>

namespace UmtMcp
{
// ---------------------------------------------------------------- 协议
constexpr int kProtocolVersion = 1;
constexpr uint16_t kDefaultPort = 27185;
constexpr const char *kBindAddress = "127.0.0.1";  // 绝不 0.0.0.0

// ---------------------------------------------------------------- 心跳与超时（协议 §8.1）
constexpr int kHeartbeatIntervalSec = 2;   // 设备端心跳间隔
constexpr int kHeartbeatTimeoutSec = 10;   // PC 侧判活阈值（≈5 个周期）
constexpr int kCommandTimeoutSec = 120;    // 单命令硬超时，绝不无限执行

// ---------------------------------------------------------------- 帧格式（NDJSON）
constexpr char kFrameDelimiter = '\n';
constexpr size_t kMaxFrameSize = 1024 * 1024;  // 单帧 1MB 上限
constexpr size_t kRecvChunkSize = 8192;

// ---------------------------------------------------------------- 内部循环节奏
constexpr int kSelectTimeoutMs = 100;      // select 超时，兼顾心跳与响应检查
constexpr int kPollSleepMs = 20;           // 等待响应时的轮询间隔

// ---------------------------------------------------------------- 错误码（协议 §5）
namespace Err
{
inline const char *const kProtocolMismatch = "E_PROTOCOL_MISMATCH";
inline const char *const kBadToken = "E_BAD_TOKEN";
inline const char *const kBadJson = "E_BAD_JSON";
inline const char *const kUnknownCmd = "E_UNKNOWN_CMD";
inline const char *const kBadArgs = "E_BAD_ARGS";
inline const char *const kTimeout = "E_TIMEOUT";
inline const char *const kInternal = "E_INTERNAL";
}  // namespace Err

}  // namespace UmtMcp
