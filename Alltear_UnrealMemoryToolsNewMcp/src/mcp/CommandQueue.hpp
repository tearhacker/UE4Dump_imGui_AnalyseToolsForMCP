#pragma once

// 双向队列：请求（CommandServer 线程 → 主线程）+ 响应（主线程 → CommandServer 线程）。
//
// 设计要点（docs/设备端命令服务设计.md §1）：
//   命令在**主线程**执行（UMT 全局非线程安全），因此必须跨线程搬运请求与响应。
//   本类不触碰任何 UMT 状态，是纯线程安全的搬运层。

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <queue>
#include <string>

namespace UmtMcp
{
struct CommandRequest
{
    uint64_t id = 0;
    std::string cmd;
    std::string argsJson;  // 原始 JSON 文本，交由 handler 解析
};

struct CommandResponse
{
    uint64_t id = 0;
    std::string payload;  // 已序列化的完整帧（不含换行符）
};

class CommandQueue
{
public:
    // ---- 请求侧：CommandServer 线程入，主线程出 ----
    void PushRequest(CommandRequest req);
    bool TryPopRequest(CommandRequest &out);
    size_t PendingRequests() const;

    // ---- 响应侧：主线程入，CommandServer 线程出 ----
    void PushResponse(CommandResponse resp);
    bool TryPopResponse(CommandResponse &out);

    void Clear();

private:
    mutable std::mutex reqMtx_;
    std::queue<CommandRequest> reqQueue_;

    mutable std::mutex respMtx_;
    std::queue<CommandResponse> respQueue_;
};

}  // namespace UmtMcp
