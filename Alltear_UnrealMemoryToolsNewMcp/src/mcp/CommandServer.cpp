#include "CommandServer.hpp"

#include <chrono>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
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
std::string CommandServer::token_;
std::string CommandServer::buildVersion_ = "1.0.0";
uint16_t CommandServer::port_ = kDefaultPort;
CommandQueue *CommandServer::queue_ = nullptr;

namespace
{
std::string GenerateToken()
{
    // 一次性随机 token（8 位十六进制）
    // 随机源只用高精度时钟：本函数在匿名 namespace，无权访问 CommandServer 的私有静态成员
    const uint64_t seed = static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::srand(static_cast<unsigned>(seed) ^ static_cast<unsigned>(seed >> 32));
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%04x%04x", std::rand() & 0xFFFF, std::rand() & 0xFFFF);
    return std::string(buf);
}
}  // namespace

std::string CommandServer::Token()
{
    return token_;
}

bool CommandServer::IsRunning()
{
    return running_.load();
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
    token_ = GenerateToken();
    stopRequested_.store(false);

    thread_ = std::thread(ServerLoop);
    thread_.detach();
    return true;
}

void CommandServer::Stop()
{
    stopRequested_.store(true);
    running_.store(false);
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

bool CommandServer::HandleFrame(int sock, const std::string &line, bool &authenticated)
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
        err["error"] = {{"code", Err::kBadJson}, {"msg", "JSON 解析失败"}};
        return SendFrame(sock, err.dump());
    }

    if (!req.is_object())
    {
        json err;
        err["ok"] = false;
        err["error"] = {{"code", Err::kBadJson}, {"msg", "帧不是 JSON 对象"}};
        return SendFrame(sock, err.dump());
    }

    // auth 帧：PC 侧证明持有 token（正确的校验方向：客户端持有密钥，服务端校验）
    const auto typeIt = req.find("type");
    const bool isAuth = (typeIt != req.end() && typeIt->is_string() &&
                         typeIt->get<std::string>() == "auth");

    // 🔴 安全红线：未认证的客户端不得执行任何命令，否则 token 形同虚设
    if (!isAuth && !authenticated)
    {
        LOGE("[MCP] 未认证的客户端尝试执行命令，断开连接");
        json err;
        err["type"] = "auth_required";
        err["error"] = {{"code", Err::kBadToken}, {"msg", "需先发送 auth 帧完成认证"}};
        SendFrame(sock, err.dump());
        return false;
    }

    if (isAuth)
    {
        // 协议版本校验（协议 §5 E_PROTOCOL_MISMATCH）
        const int peerProtocol = req.value("protocol", kProtocolVersion);
        if (peerProtocol != kProtocolVersion)
        {
            LOGE("[MCP] 协议版本不匹配（对端 %d，本端 %d），断开连接",
                 peerProtocol, kProtocolVersion);
            json err;
            err["type"] = "auth_fail";
            err["error"] = {{"code", Err::kProtocolMismatch},
                            {"msg", "协议版本不匹配"},
                            {"detail", {{"serverProtocol", kProtocolVersion},
                                        {"clientProtocol", peerProtocol}}}};
            SendFrame(sock, err.dump());
            return false;
        }

        std::string got;
        const auto tokIt = req.find("token");
        if (tokIt != req.end() && tokIt->is_string())
            got = tokIt->get<std::string>();

        if (got != token_)
        {
            LOGE("[MCP] token 校验失败，断开连接");
            json err;
            err["type"] = "auth_fail";
            err["error"] = {{"code", Err::kBadToken}, {"msg", "token 不匹配"}};
            SendFrame(sock, err.dump());
            return false;
        }

        authenticated = true;
        json ok;
        ok["type"] = "auth_ok";
        ok["protocol"] = kProtocolVersion;
        return SendFrame(sock, ok.dump());
    }

    // 普通请求：需要 id + cmd
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

    // 入队，交给主线程执行
    json args = req.contains("args") && req["args"].is_object() ? req["args"] : json::object();
    queue_->PushRequest({id, cmd, args.dump()});

    // 同步等待响应（期间持续发心跳，让 PC 侧区分「在算」与「死了」）
    time_t lastHeartbeat = std::time(nullptr);
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(kCommandTimeoutSec);

    while (std::chrono::steady_clock::now() < deadline)
    {
        CommandResponse resp;
        if (queue_->TryPopResponse(resp))
        {
            if (resp.id != id)
            {
                // 串行协议下不应出现；兜底丢弃错配响应
                LOGE("[MCP] 响应 id 错配（期望 %llu，实得 %llu）",
                     (unsigned long long)id, (unsigned long long)resp.id);
                continue;
            }
            return SendFrame(sock, resp.payload);
        }

        MaybeSendHeartbeat(sock, lastHeartbeat, true);
        std::this_thread::sleep_for(std::chrono::milliseconds(kPollSleepMs));
    }

    // 硬超时（协议 §3.9）
    LOGE("[MCP] 命令 %s 执行超时（%ds）", cmd.c_str(), kCommandTimeoutSec);
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
    // 🔴 安全红线：只监听回环，绝不 0.0.0.0
    if (::inet_pton(AF_INET, kBindAddress, &addr.sin_addr) != 1)
    {
        LOGE("[MCP] 地址解析失败: %s", kBindAddress);
        ::close(serverFd);
        return;
    }

    if (::bind(serverFd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        LOGE("[MCP] bind %s:%u 失败: %s", kBindAddress, port_, strerror(errno));
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
    LOGI("[MCP] 命令服务已启动 %s:%u", kBindAddress, port_);

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

        LOGI("[MCP] 客户端已连接");
        queue_->Clear();

        // 连接级认证状态：每个新连接都必须重新 auth
        bool authenticated = false;

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

            // 处理所有完整行（NDJSON）
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

                if (!HandleFrame(clientFd, line, authenticated))
                {
                    connected = false;  // 认证失败 / token 错误等情况
                    break;
                }
            }
        }

        ::close(clientFd);
        LOGI("[MCP] 客户端断开");
    }

    ::close(serverFd);
    running_.store(false);
    LOGI("[MCP] 命令服务已停止");
}

}  // namespace UmtMcp
