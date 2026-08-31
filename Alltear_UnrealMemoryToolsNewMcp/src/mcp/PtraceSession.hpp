#pragma once

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "Protocol.hpp"  // kSessionIdleTimeoutMs / kSessionMaxHoldLimitMs

namespace UmtMcp
{

// ptrace 附加会话管理（F 组远程调用）。
//
// 风险：PTRACE_ATTACH 会**冻结目标进程**；漏调 endAttachSession 会让游戏永久冻结。
// 因此按 docs/泪心UE4_MCP工具功能规格.md §7 实现「四重兜底，缺一不可」：
//   1. maxHoldMs 到点 —— 服务端强制 detach（WatchdogLoop）
//   2. 空闲看门狗   —— 会话内超时无操作自动 detach（WatchdogLoop）
//   3. 异常兜底     —— 任何工具异常先 detach 再返回错误（handler 内 try/catch 保证）
//   4. 断连兜底     —— 进程退出时析构强制 detach（~PtraceSessionRegistry）
//
// ptrace 每个目标进程只允许一个 tracer，故同一时刻只允许一个会话；
// 重复 Begin 会被拒绝（提示先 END_ATTACH_SESSION），避免双重 attach 造成状态错乱。
//
// 补充保险：即便以上全部失效，tracer 进程被杀时内核也会自动 detach 并恢复 tracee，
// 所以"永久冻结"的最坏情况只发生在"本进程存活但会话泄漏"，而这正是看门狗覆盖的场景。
class PtraceSessionRegistry
{
public:
    static PtraceSessionRegistry &Instance();

    // attach 并建立会话，返回 sessionId。失败抛 std::runtime_error。
    std::string Begin(int maxHoldMs);

    // detach 并结束会话，返回会话存活毫秒数；sessionId 无效返回 -1。
    int64_t End(const std::string &sessionId);

    // 校验会话有效并刷新活跃时间；无效返回 false。
    bool Touch(const std::string &sessionId);

    bool IsActive() const;
    std::string ActiveId() const;
    int ActiveMaxHoldMs() const;
    int64_t ElapsedMs() const;

    // 记录 allocScratch 的分配，便于排障/审计（不自动释放：目标进程内的内存随其生命周期）。
    void RecordAlloc(uintptr_t addr, size_t size);
    std::vector<std::pair<uintptr_t, size_t>> Allocs() const;

    // 强制 detach 并清理（看门狗 / 析构 / 异常兜底共用）。
    void ForceDetachAll();

private:
    PtraceSessionRegistry();
    ~PtraceSessionRegistry();
    PtraceSessionRegistry(const PtraceSessionRegistry &) = delete;
    PtraceSessionRegistry &operator=(const PtraceSessionRegistry &) = delete;

    void EnsureWatchdog();
    void WatchdogLoop();

private:
    mutable std::mutex mu_;

    std::string activeId_;
    bool attached_ = false;
    std::chrono::steady_clock::time_point createdAt_{};
    std::chrono::steady_clock::time_point lastUsedAt_{};
    int maxHoldMs_ = 0;

    std::vector<std::pair<uintptr_t, size_t>> allocs_;

    std::thread watchdog_;
    std::atomic<bool> watchdogRunning_{false};
    std::atomic<bool> stopRequested_{false};
    unsigned long long sessionCounter_ = 0;
};

// 会话默认/上限持有时间（docs §7：maxHoldMs 默认 3000，≤30000）
inline const int kSessionDefaultHoldMs = 3000;
inline const int kSessionMaxHoldLimitMs = 30000;
// 空闲看门狗：会话内无操作超过此值自动 detach
// （kSessionIdleTimeoutMs 定义在 Protocol.hpp:32，此处勿重复定义）

}  // namespace UmtMcp
