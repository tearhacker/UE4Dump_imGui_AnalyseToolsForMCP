#include "CommandServer.hpp"

#include <chrono>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <vector>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <nlohmann/json.hpp>

#include "../Utils/Logger.hpp"
#include "CommandDispatcher.hpp"

namespace UmtMcp
{
using json = nlohmann::json;

std::atomic<bool> CommandServer::running_{false};
std::atomic<bool> CommandServer::stopRequested_{false};
std::thread CommandServer::thread_;
std::string CommandServer::buildVersion_ = "1.0.0";
uint16_t CommandServer::port_ = kDefaultPort;
CommandQueue *CommandServer::queue_ = nullptr;
// 🔴 默认只监听回环。可被 mcp_bind.conf 或 SetBindAddress() 覆盖。
std::string CommandServer::bindAddress_ = kBindAddress;

bool CommandServer::IsRunning()
{
    return running_.load();
}

void CommandServer::SetBindAddress(const std::string &addr)
{
    bindAddress_ = addr.empty() ? kBindAddress : addr;
}

const std::string &CommandServer::GetBindAddress()
{
    return bindAddress_;
}

void CommandServer::LoadBindAddressFromConfig()
{
    // 文件不存在是常态（默认只监听回环），不是错误，静默返回即可。
    FILE *f = ::fopen(kBindConfigPath, "r");
    if (!f)
        return;

    char buf[64] = {0};
    if (::fgets(buf, sizeof(buf), f))
    {
        const std::string line(buf);
        const auto b = line.find_first_not_of(" \t\r\n");
        const auto e = line.find_last_not_of(" \t\r\n");
        if (b != std::string::npos && e != std::string::npos)
        {
            const std::string addr = line.substr(b, e - b + 1);
            bindAddress_ = addr;
            if (addr != kBindAddress)
            {
                LOGW("[MCP] 警告：监听非回环地址 %s，服务将暴露到网络。"
                     "root 级内存读写接口，仅在可信网络下使用。",
                     addr.c_str());
            }
            else
            {
                LOGI("[MCP] 配置文件指定监听 %s", addr.c_str());
            }
        }
    }
    ::fclose(f);
}

bool CommandServer::Start(uint16_t port, CommandQueue *queue, const std::string &buildVersion)
{
    if (running_.load())
        return true;
    if (!queue)
        return false;

    queue_ = queue;
    port_ = port;
    if (!buildVersion.empty())
        buildVersion_ = buildVersion;
    stopRequested_.store(false);

    // 运行期 bind 地址覆盖：读 mcp_bind.conf。
    // 文件缺失或内容非法 → bindAddress_ 保持默认 127.0.0.1，安全红线不放松。
    LoadBindAddressFromConfig();

    // 注：此处不 detach，由 Stop() 统一 join 回收，避免线程泄漏
    // （见 Stop() 说明；单连接串行模型下关闭代价可控）。
    thread_ = std::thread(ServerLoop);
    return true;
}

void CommandServer::Stop()
{
    stopRequested_.store(true);
    running_.store(false);

    // 干净回收服务端线程：ServerLoop 每轮都检查 stopRequested_，且 HandleFrame
    // 的等待循环也已接入 stopRequested_，故 join 通常百毫秒内返回。
    // 例外：若恰有命令在飞（主线程执行、最长 kCommandTimeoutSec），join 会等到
    // 该命令响应或硬超时——这是单连接串行模型下可接受的关闭代价
    // （见 docs/设备端命令服务设计.md §5）。
    if (thread_.joinable())
        thread_.join();
}

bool CommandServer::SendAll(int sock, const char *data, size_t len)
{
    size_t sent = 0;
    while (sent < len)
    {
        ssize_t n = ::send(sock, data + sent, len - sent, 0);
        if (n <= 0)
        {
            if (errno == EINTR)
                continue;
            return false;
        }
        sent += static_cast<size_t>(n);
    }
    return true;
}

bool CommandServer::SendFrame(int sock, const std::string &payload)
{
    std::string frame = payload;
    frame.push_back(kFrameDelimiter);
    return SendAll(sock, frame.data(), frame.size());
}

void CommandServer::MaybeSendHeartbeat(int sock, time_t &lastHeartbeat, bool busy)
{
    const time_t now = std::time(nullptr);
    if (now - lastHeartbeat < kHeartbeatIntervalSec)
        return;

    lastHeartbeat = now;
    json hb;
    hb["type"] = "heartbeat";
    hb["ts"] = static_cast<int64_t>(now);
    hb["busy"] = busy;
    SendFrame(sock, hb.dump());
}

bool CommandServer::HandleFrame(int sock, const std::string &line)
{
    json req;
    try
    {
        req = json::parse(line);
    }
    catch (const std::exception &)
    {
        json err;
        err["ok"] = false;
        // 🔴 含 id 字段:PC 侧工具调用栈才能正确关联错误与请求
        const auto idIt = req.find("id");
        if (idIt != req.end())
            err["id"] = *idIt;
        err["error"] = {{"code", Err::kBadJson}, {"msg", "JSON 解析失败"}};
        return SendFrame(sock, err.dump());
    }

    if (!req.is_object())
    {
        json err;
        err["ok"] = false;
        const auto idIt = req.find("id");
        if (idIt != req.end())
            err["id"] = *idIt;
        err["error"] = {{"code", Err::kBadJson}, {"msg", "帧不是 JSON 对象"}};
        return SendFrame(sock, err.dump());
    }

    // 普通请求:需要 id + cmd
    if (!req.contains("id") || !req.contains("cmd"))
    {
        json err;
        err["ok"] = false;
        err["error"] = {{"code", Err::kBadArgs}, {"msg", "缺少 id 或 cmd"}};
        return SendFrame(sock, err.dump());
    }

    const uint64_t id = req.value("id", 0ULL);
    const std::string cmd = req.value("cmd", "");

    if (!CommandDispatcher::IsRegistered(cmd))
    {
        json err;
        err["id"] = id;
        err["ok"] = false;
        err["error"] = {{"code", Err::kUnknownCmd}, {"msg", "未知命令: " + cmd}};
        return SendFrame(sock, err.dump());
    }

    // 入队,交给主线程执行
    json args = req.contains("args") && req["args"].is_object() ? req["args"] : json::object();

    // 🔴 让 IMGUI 日志能看到「AI 调了哪个工具/函数」——否则界面里只剩连接上下线刷屏
    {
        std::string argsSummary = args.dump();
        if (argsSummary.size() > 240)
            argsSummary = argsSummary.substr(0, 240) + "...(已截断)";
        LOGI("[MCP·调用] %s  id=%llu  args=%s",
             cmd.c_str(), (unsigned long long)id, argsSummary.c_str());
    }

    queue_->PushRequest({id, cmd, args.dump()});

    // 同步等待响应(期间持续发心跳,让 PC 侧区分「在算」与「死了」)
    time_t lastHeartbeat = std::time(nullptr);
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(kCommandTimeoutSec);

    // 🔴 孤儿响应检测:命令在超时后完成时会入队,本循环必须跳过 id 不匹配的响应
    // 否则下一条连接会收到上一条超时命令的响应(协议 §3.9)
    while (std::chrono::steady_clock::now() < deadline && !stopRequested_.load())
    {
        CommandResponse resp;
        if (queue_->TryPopResponse(resp))
        {
            if (resp.id != id)
            {
                // 串行协议下不应出现;兜底丢弃错配响应
                LOGE("[MCP] 响应 id 错配(期望 %llu,实得 %llu)",
                     (unsigned long long)id, (unsigned long long)resp.id);
                continue;
            }
            return SendFrame(sock, resp.payload);
        }

        MaybeSendHeartbeat(sock, lastHeartbeat, true);
        std::this_thread::sleep_for(std::chrono::milliseconds(kPollSleepMs));
    }

    // 硬超时(协议 §3.9)
    LOGE("[MCP] 命令 %s 执行超时(%ds),后续可能有无主响应入队", cmd.c_str(), kCommandTimeoutSec);
    json err;
    err["id"] = id;
    err["ok"] = false;
    err["error"] = {{"code", Err::kTimeout},
                    {"msg", "命令执行超时"},
                    {"detail", {{"timeoutSec", kCommandTimeoutSec}}}};
    return SendFrame(sock, err.dump());
}

void CommandServer::ServerLoop()
{
    const int serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0)
    {
        LOGE("[MCP] socket 创建失败: %s", strerror(errno));
        return;
    }

    int reuse = 1;
    ::setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    // 🔴 安全红线：默认只监听回环（kBindAddress）。
    // 非回环（0.0.0.0 / 局域网 IP）必须显式写 mcp_bind.conf 才生效。
    const std::string &bindAddr = bindAddress_;
    if (::inet_pton(AF_INET, bindAddr.c_str(), &addr.sin_addr) != 1)
    {
        LOGE("[MCP] 地址解析失败: %s", bindAddr.c_str());
        ::close(serverFd);
        return;
    }

    if (::bind(serverFd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        LOGE("[MCP] bind %s:%u 失败: %s", bindAddr.c_str(), port_, strerror(errno));
        ::close(serverFd);
        return;
    }

    // 单对单：backlog = 1
    if (::listen(serverFd, 1) < 0)
    {
        LOGE("[MCP] listen 失败: %s", strerror(errno));
        ::close(serverFd);
        return;
    }

    running_.store(true);
    LOGI("[MCP] 命令服务已启动 %s:%u", bindAddress_.c_str(), port_);

    // 连接事件合并：若断开后极短时间内又连上（重连风暴），降级为单行「重连」，
    // 避免 IMGUI 日志被「已连接/已断开」成对刷屏
    static time_t s_lastConnectTs = 0;
    static time_t s_lastDisconnectTs = 0;
    constexpr int kConnCoalesceSec = 5;  // 间隔小于此值视为同一会话的反复重连

    while (!stopRequested_.load())
    {
        // 带超时等待连接，便于响应 Stop()
        fd_set readFds;
        FD_ZERO(&readFds);
        FD_SET(serverFd, &readFds);
        timeval tv{};
        tv.tv_sec = 0;
        tv.tv_usec = kSelectTimeoutMs * 1000;

        const int ready = ::select(serverFd + 1, &readFds, nullptr, nullptr, &tv);
        if (ready <= 0)
            continue;

        const int clientFd = ::accept(serverFd, nullptr, nullptr);
        if (clientFd < 0)
            continue;

        {
            const time_t now = std::time(nullptr);
            // 🔴 重连风暴合并：距上次断开 < 阈值 → 只记一行「重连」，不重复「已连接」
            if ((now - s_lastDisconnectTs) < kConnCoalesceSec)
                LOGI("[MCP·连接] 重连（距上次断开 %lds）", (long)(now - s_lastDisconnectTs));
            else
                LOGI("[MCP·连接] 客户端已连接");
            s_lastConnectTs = now;
        }
        // 🔴 新连接:清空队列,丢弃上一连接的未消费响应(防止孤儿响应污染)
        CommandDispatcher::Clear();

        // HELLO：capabilities 由实际注册的命令生成，避免硬编码漂移
        json hello;
        hello["type"] = "hello";
        hello["protocol"] = kProtocolVersion;
        hello["build"] = buildVersion_;
        hello["capabilities"] = CommandDispatcher::RegisteredCommands();
        if (!SendFrame(clientFd, hello.dump()))
        {
            ::close(clientFd);
            continue;
        }

        // 通信循环
        std::string recvBuf;
        std::vector<char> chunk(kRecvChunkSize);
        time_t lastHeartbeat = std::time(nullptr);
        bool connected = true;

        while (connected && !stopRequested_.load())
        {
            MaybeSendHeartbeat(clientFd, lastHeartbeat, queue_->PendingRequests() > 0);

            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(clientFd, &rfds);
            timeval ctv{};
            ctv.tv_sec = 0;
            ctv.tv_usec = kSelectTimeoutMs * 1000;

            const int n = ::select(clientFd + 1, &rfds, nullptr, nullptr, &ctv);
            if (n < 0)
            {
                connected = false;
                break;
            }
            if (n == 0)
                continue;  // 超时，回到心跳检查

            const ssize_t got = ::recv(clientFd, chunk.data(), chunk.size(), 0);
            if (got <= 0)
            {
                connected = false;
                break;
            }

            recvBuf.append(chunk.data(), static_cast<size_t>(got));

            // 🔴 recvBuf 上限：防止客户端不发 \n 导致无限增长 OOM
            if (recvBuf.size() > kMaxFrameSize * 16)
            {
                LOGE("[MCP] recvBuf 累积 %zu 字节超过阈值(%zu),断开",
                     recvBuf.size(), kMaxFrameSize * 16);
                connected = false;
                break;
            }

            // 处理所有完整行(NDJSON)
            size_t pos;
            while ((pos = recvBuf.find(kFrameDelimiter)) != std::string::npos)
            {
                std::string line = recvBuf.substr(0, pos);
                recvBuf.erase(0, pos + 1);

                if (line.empty())
                    continue;
                if (line.size() > kMaxFrameSize)
                {
                    LOGE("[MCP] 单帧超过 %zu 字节，断开", kMaxFrameSize);
                    connected = false;
                    break;
                }

                if (!HandleFrame(clientFd, line))
                {
                    connected = false;
                    break;
                }
            }
        }

        ::close(clientFd);
        {
            const time_t now = std::time(nullptr);
            // 仅当本次连接曾持续一段时间，才单独记「断开」；
            // 否则它会被下一次「重连」行概括，避免成对刷屏
            if ((now - s_lastConnectTs) >= kConnCoalesceSec)
                LOGI("[MCP·连接] 客户端断开");
            s_lastDisconnectTs = now;
        }
    }

    ::close(serverFd);
    running_.store(false);
    LOGI("[MCP] 命令服务已停止");
}

}  // namespace UmtMcp
