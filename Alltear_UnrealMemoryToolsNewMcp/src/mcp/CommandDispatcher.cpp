#include "CommandDispatcher.hpp"

#include <chrono>
#include <utility>

#include "../Utils/Logger.hpp"

namespace UmtMcp
{
std::mutex CommandDispatcher::registryMtx_;
std::unordered_map<std::string, CommandHandler> CommandDispatcher::handlers_;
std::unordered_map<std::string, bool> CommandDispatcher::fastFlags_;
CommandQueue *CommandDispatcher::queue_ = nullptr;

void CommandDispatcher::BindQueue(CommandQueue *queue)
{
    queue_ = queue;
}

CommandQueue &CommandDispatcher::Queue()
{
    return *queue_;
}

void CommandDispatcher::Register(const std::string &cmd, CommandHandler handler, bool isFast)
{
    std::lock_guard<std::mutex> lock(registryMtx_);
    handlers_[cmd] = std::move(handler);
    fastFlags_[cmd] = isFast;
}

bool CommandDispatcher::IsRegistered(const std::string &cmd)
{
    std::lock_guard<std::mutex> lock(registryMtx_);
    return handlers_.find(cmd) != handlers_.end();
}

std::vector<std::string> CommandDispatcher::RegisteredCommands()
{
    std::lock_guard<std::mutex> lock(registryMtx_);
    std::vector<std::string> out;
    out.reserve(handlers_.size());
    for (const auto &kv : handlers_)
        out.push_back(kv.first);
    return out;
}

bool CommandDispatcher::PollOnce()
{
    if (!queue_)
        return false;

    CommandRequest req;
    if (!queue_->TryPopRequest(req))
        return false;

    json response;
    response["id"] = req.id;

    const auto startedAt = std::chrono::steady_clock::now();

    // 解析 args（缺省为空对象）
    json args = json::object();
    if (!req.argsJson.empty())
    {
        try
        {
            args = json::parse(req.argsJson);
            if (!args.is_object())
                args = json::object();
        }
        catch (const std::exception &e)
        {
            response["ok"] = false;
            response["error"] = {{"code", Err::kBadArgs}, {"msg", std::string("args 解析失败: ") + e.what()}};
            queue_->PushResponse({req.id, response.dump()});
            return true;
        }
    }

    CommandHandler handler;
    {
        std::lock_guard<std::mutex> lock(registryMtx_);
        auto it = handlers_.find(req.cmd);
        if (it == handlers_.end())
        {
            response["ok"] = false;
            response["error"] = {{"code", Err::kUnknownCmd}, {"msg", "未知命令: " + req.cmd}};
            queue_->PushResponse({req.id, response.dump()});
            return true;
        }
        handler = it->second;
    }

    // 执行（快命令当场执行；重活在后续版本投 gWorkerThread）
    try
    {
        json data = handler(args);
        response["ok"] = true;
        response["data"] = data.is_null() ? json::object() : std::move(data);
    }
    catch (const std::exception &e)
    {
        response["ok"] = false;
        response["error"] = {{"code", Err::kInternal}, {"msg", e.what()}};
    }
    catch (...)
    {
        response["ok"] = false;
        response["error"] = {{"code", Err::kInternal}, {"msg", "未知异常"}};
    }

    // 硬超时检查（协议 §3.9）
    // 同步执行无法中断，此处记录告警——若快命令都超时，说明分级配置错了（应标为重活）。
    const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                               std::chrono::steady_clock::now() - startedAt)
                               .count();
    if (elapsedMs > kCommandTimeoutSec * 1000)
    {
        LOGE("[MCP] 命令 %s 耗时 %lldms 超过硬超时 %ds —— 该命令不应标记为快命令",
             req.cmd.c_str(), (long long)elapsedMs, kCommandTimeoutSec);
    }

    queue_->PushResponse({req.id, response.dump()});
    return true;
}

}  // namespace UmtMcp
