#pragma once

// 命令分发器：命令名 → handler 映射 + 分级 + 硬超时。
//
// 🔴 执行位置（docs/设备端命令服务设计.md §1）：
//    PollOnce() 由**主线程**（渲染循环内）调用——UMT 全部能力依赖非线程安全的
//    文件级全局（kMgr / UEWrappers / gDumpUiState），放主线程即与 UI 操作天然串行，
//    无需给既有代码加锁。
//    CommandServer 线程严禁调用本类的执行接口。

#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

#include "CommandQueue.hpp"
#include "Protocol.hpp"

namespace UmtMcp
{
using json = nlohmann::json;

// handler 返回 data 内容（成功时）；抛异常或返回 null 视为失败
using CommandHandler = std::function<json(const json &args)>;

class CommandDispatcher
{
public:
    // 注册命令。isFast=true → 快命令（主线程当场执行，不投 worker）
    static void Register(const std::string &cmd, CommandHandler handler, bool isFast = true);

    static bool IsRegistered(const std::string &cmd);

    // 已注册命令名列表（供 HELLO 的 capabilities 字段使用，避免硬编码）
    static std::vector<std::string> RegisteredCommands();

    // 主线程每帧调用一次：取一条请求、执行、产出响应帧。
    // 返回 true 表示本帧确实处理了一条命令（调用方可据此决定是否继续 poll）。
    static bool PollOnce();

    static void BindQueue(CommandQueue *queue);
    static CommandQueue &Queue();

private:
    static std::mutex registryMtx_;
    static std::unordered_map<std::string, CommandHandler> handlers_;
    static std::unordered_map<std::string, bool> fastFlags_;
    static CommandQueue *queue_;
};

}  // namespace UmtMcp
