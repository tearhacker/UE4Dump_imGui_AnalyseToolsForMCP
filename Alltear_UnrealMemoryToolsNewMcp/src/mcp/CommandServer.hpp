#pragma once

// 设备端命令服务（服务端）。
//
// 角色：本项目是**服务端** —— bind + listen + accept，等 PC 侧 mcp_server 来连。
// 单对单（一个客户端 ↔ 一个服务端），backlog=1，新连接直接拒绝。
//
// 🔴 线程边界（docs/设备端命令服务设计.md §1）：
//    本类运行在**独立线程**，只做 socket I/O、入队请求、出队响应、发心跳。
//    **严禁调用任何 UMT 能力**（kMgr / UEWrappers / gDumpUiState 均非线程安全），
//    命令一律由主线程 CommandDispatcher::PollOnce() 执行。

#include <atomic>
#include <cstdint>
#include <string>
#include <thread>

#include "CommandQueue.hpp"
#include "Protocol.hpp"

namespace UmtMcp
{
class CommandServer
{
public:
    // 启动服务（非阻塞，内部起线程）。连接建立后可直接执行命令。
    // buildVersion 会写进 HELLO 帧（应传 kUEDUMPER_VERSION）
    static bool Start(uint16_t port, CommandQueue *queue, const std::string &buildVersion = "1.0.0");
    static void Stop();
    static bool IsRunning();

    // 🔴 运行期覆盖监听地址。传空串则回退到 kBindAddress(127.0.0.1)。
    // 需在 Start() 之前调用；Start() 内部会自动读取 mcp_bind.conf。
    // 放开到非回环（0.0.0.0 / 局域网 IP）= 把 root 级内存读写暴露到网络。
    static void SetBindAddress(const std::string &addr);
    static const std::string &GetBindAddress();

private:
    // 从 kBindConfigPath 读取可选的 bind 覆盖；文件缺失/为空则保持默认值。
    static void LoadBindAddressFromConfig();

    static void ServerLoop();

    // 帧收发（NDJSON）
    static bool SendFrame(int sock, const std::string &payload);
    static bool SendAll(int sock, const char *data, size_t len);

    // 处理单条已接收的帧（请求入队 → 等待响应 → 发送）
    static bool HandleFrame(int sock, const std::string &line);

    // 心跳（等待响应期间也会被调用）
    static void MaybeSendHeartbeat(int sock, time_t &lastHeartbeat, bool busy);

    static std::atomic<bool> running_;
    static std::atomic<bool> stopRequested_;
    static std::thread thread_;
    static std::string buildVersion_;
    static uint16_t port_;
    static CommandQueue *queue_;
    static std::string bindAddress_;   // 运行期 bind 地址，默认 kBindAddress
};

}  // namespace UmtMcp
