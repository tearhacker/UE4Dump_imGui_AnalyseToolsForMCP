#include "PtraceSession.hpp"

#include "../../KittyMemoryEx/KittyMemoryMgr.hpp"
#include "../UE/UEMemory.hpp"

namespace UmtMcp
{

PtraceSessionRegistry &PtraceSessionRegistry::Instance()
{
    // 函数内静态对象：进程退出时析构 → 断连兜底（第 4 重）
    static PtraceSessionRegistry instance;
    return instance;
}

PtraceSessionRegistry::PtraceSessionRegistry() = default;

PtraceSessionRegistry::~PtraceSessionRegistry()
{
    stopRequested_.store(true);
    if (watchdog_.joinable())
        watchdog_.join();
    ForceDetachAll();
}

void PtraceSessionRegistry::ForceDetachAll()
{
    std::lock_guard<std::mutex> lk(mu_);
    if (attached_ && UEMemory::kMgr.isMemValid())
        UEMemory::kMgr.trace.Detach();
    attached_ = false;
    activeId_.clear();
    maxHoldMs_ = 0;
}

std::string PtraceSessionRegistry::Begin(int maxHoldMs)
{
    if (!UEMemory::kMgr.isMemValid())
        throw std::runtime_error("未 attach 到目标进程");

    std::lock_guard<std::mutex> lk(mu_);
    if (attached_)
        throw std::runtime_error("已有活动会话 " + activeId_ + "，请先调用 END_ATTACH_SESSION");

    if (!UEMemory::kMgr.trace.Attach())
        throw std::runtime_error("PTRACE_ATTACH 失败（ptrace 不可用 / 权限不足 / 目标进程已退出）");

    const auto now = std::chrono::steady_clock::now();
    createdAt_ = now;
    lastUsedAt_ = now;
    maxHoldMs_ = maxHoldMs;
    attached_ = true;
    activeId_ = "sess-" + std::to_string(++sessionCounter_);

    EnsureWatchdog();
    return activeId_;
}

int64_t PtraceSessionRegistry::End(const std::string &sessionId)
{
    std::lock_guard<std::mutex> lk(mu_);
    if (!attached_ || activeId_ != sessionId)
        return -1;

    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::steady_clock::now() - createdAt_)
                             .count();

    if (UEMemory::kMgr.isMemValid())
        UEMemory::kMgr.trace.Detach();

    attached_ = false;
    activeId_.clear();
    maxHoldMs_ = 0;
    return static_cast<int64_t>(elapsed);
}

bool PtraceSessionRegistry::Touch(const std::string &sessionId)
{
    std::lock_guard<std::mutex> lk(mu_);
    if (!attached_ || activeId_ != sessionId)
        return false;
    lastUsedAt_ = std::chrono::steady_clock::now();
    return true;
}

bool PtraceSessionRegistry::IsActive() const
{
    std::lock_guard<std::mutex> lk(mu_);
    return attached_;
}

std::string PtraceSessionRegistry::ActiveId() const
{
    std::lock_guard<std::mutex> lk(mu_);
    return activeId_;
}

int PtraceSessionRegistry::ActiveMaxHoldMs() const
{
    std::lock_guard<std::mutex> lk(mu_);
    return maxHoldMs_;
}

int64_t PtraceSessionRegistry::ElapsedMs() const
{
    std::lock_guard<std::mutex> lk(mu_);
    if (!attached_) return 0;
    return static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                                    std::chrono::steady_clock::now() - createdAt_)
                                    .count());
}

void PtraceSessionRegistry::RecordAlloc(uintptr_t addr, size_t size)
{
    std::lock_guard<std::mutex> lk(mu_);
    allocs_.emplace_back(addr, size);
}

std::vector<std::pair<uintptr_t, size_t>> PtraceSessionRegistry::Allocs() const
{
    std::lock_guard<std::mutex> lk(mu_);
    return allocs_;
}

void PtraceSessionRegistry::EnsureWatchdog()
{
    if (watchdogRunning_.load())
        return;
    watchdogRunning_.store(true);
    stopRequested_.store(false);
    watchdog_ = std::thread(&PtraceSessionRegistry::WatchdogLoop, this);
}

void PtraceSessionRegistry::WatchdogLoop()
{
    while (!stopRequested_.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::lock_guard<std::mutex> lk(mu_);
        if (!attached_)
            continue;

        const auto now = std::chrono::steady_clock::now();
        const auto heldMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - createdAt_).count();
        const auto idleMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUsedAt_).count();

        // 兜底 1：maxHoldMs 到点强制 detach
        // 兜底 2：空闲超时强制 detach
        if ((maxHoldMs_ > 0 && heldMs > maxHoldMs_) || idleMs > kSessionIdleTimeoutMs)
        {
            if (UEMemory::kMgr.isMemValid())
                UEMemory::kMgr.trace.Detach();
            attached_ = false;
            activeId_.clear();
            maxHoldMs_ = 0;
        }
    }
}

}  // namespace UmtMcp
