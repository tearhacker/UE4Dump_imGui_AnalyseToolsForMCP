#include "CommandQueue.hpp"

namespace UmtMcp
{
void CommandQueue::PushRequest(CommandRequest req)
{
    std::lock_guard<std::mutex> lock(reqMtx_);
    reqQueue_.push(std::move(req));
}

bool CommandQueue::TryPopRequest(CommandRequest &out)
{
    std::lock_guard<std::mutex> lock(reqMtx_);
    if (reqQueue_.empty())
        return false;
    out = std::move(reqQueue_.front());
    reqQueue_.pop();
    return true;
}

size_t CommandQueue::PendingRequests() const
{
    std::lock_guard<std::mutex> lock(reqMtx_);
    return reqQueue_.size();
}

void CommandQueue::PushResponse(CommandResponse resp)
{
    std::lock_guard<std::mutex> lock(respMtx_);
    respQueue_.push(std::move(resp));
}

bool CommandQueue::TryPopResponse(CommandResponse &out)
{
    std::lock_guard<std::mutex> lock(respMtx_);
    if (respQueue_.empty())
        return false;
    out = std::move(respQueue_.front());
    respQueue_.pop();
    return true;
}

void CommandQueue::Clear()
{
    {
        std::lock_guard<std::mutex> lock(reqMtx_);
        std::queue<CommandRequest> emptyReq;
        reqQueue_.swap(emptyReq);
    }
    {
        std::lock_guard<std::mutex> lock(respMtx_);
        std::queue<CommandResponse> emptyResp;
        respQueue_.swap(emptyResp);
    }
}

}  // namespace UmtMcp
