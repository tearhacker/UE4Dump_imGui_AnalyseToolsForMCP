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
    // 启动服务（非阻塞，内部起线程）。token 由本类随机生成，通过 Token() 取用。
    // buildVersion 会写进 HELLO 帧（应传 kUEDUMPER_VERSION）
    static bool Start(uint16_t port, CommandQueue *queue, const std::string &buildVersion = "1.0.0");
    static void Stop();
    static bool IsRunning();

    // 一次性 token，需在 UMT 界面展示给用户（用户填到 PC 侧配置）
    static std::string Token();

private:
    static void ServerLoop();

    // 帧收发（NDJSON）
    static bool SendFrame(int sock, const std::string &payload);
    static bool SendAll(int sock, const char *data, size_t len);

    // 处理单条已接收的帧（请求入队 → 等待响应 → 发送）
    // authenticated 为**连接级**状态：未通过 auth 的客户端只能发 auth 帧
    static bool HandleFrame(int sock, const std::string &line, bool &authenticated);

    // 心跳（等待响应期间也会被调用）
    static void MaybeSendHeartbeat(int sock, time_t &lastHeartbeat, bool busy);

    static std::atomic<bool> running_;
    static std::atomic<bool> stopRequested_;
    static std::thread thread_;
    static std::string token_;
    static std::string buildVersion_;
    static uint16_t port_;
    static CommandQueue *queue_;
};

}  // namespace UmtMcp
