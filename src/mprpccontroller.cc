//
// Created by Y2000 on 2024/12/3.
//

#include "mprpccontroller.h"

MPRpcController::MPRpcController()
{
    m_failed = false;
    m_canceled = false;
    m_errorText = "";
    m_callback = nullptr;
}

/**
 * 将 RpcController 重置为其初始状态，以便它可以在新调用中重复使用。
 * 在 rpc调用 正在进行时，不得调用
 */
void MPRpcController::Reset()
{
    m_failed = false;
    m_canceled = false;
    m_errorText = "";
    m_callback = nullptr;
}

/**
 * 用于判断rpc调用是否成功，失败的可能原因取决于 RPC 实现。
 * 在rpc调用完成之前，不得调用 Failed()
 * @return 如果调用失败，则返回 true, 则响应消息的内容未定义; 成功返回false
 */
bool MPRpcController::Failed() const
{
    return m_failed;
}

/**
 * 如果 Failed() 为 true，则返回错误的描述信息; 如果尚未完成 rpc调用，则返回空字符串
 * @return 错误的描述信息
 */
std::string MPRpcController::ErrorText() const
{
    return m_errorText;
}

/**
 * 使 Failed() 在调用方(客户端)返回 true。“reason” 将被合并到 ErrorText() 返回的消息中。
 * 如果您发现需要返回有关失败的信息，则应将其合并到响应协议缓冲区中，并且不应调用 SetFailed()
 * @param reason 错误原因，将被合并到 ErrorText() 返回的消息中
 */
void MPRpcController::SetFailed(const std::string& reason)
{
    m_failed = true;
    m_errorText.append(reason);
}

/**
 * 通知rpc服务发布者(服务端)，当前rpc调用方(客户端)希望取消 rpc调用。rpc服务发布者(服务端)可能会立即取消它，可能会等待一段时间然后取消它，
 * 甚至可能根本不取消调用。如果取消调用，仍会调用 done（完成）回调，此时 RpcController 会提示调用失败
 */
void MPRpcController::StartCancel()
{
    m_canceled = true;
}

/**
 * 用于判断调用方(客户端)是否取消了 rpc调用，
 * rpc服务发布者(服务端)也可以放弃回复它。服务器仍应调用最终的 “done” 回调。
 * @return 如果为 true，则表示调用方(客户端)取消了 rpc调用
 */
bool MPRpcController::IsCanceled() const
{
    return m_canceled;
}

/**
 * 在取消 rpc调用时，rpc调用方(客户端)给定的回调。回调将始终被调用一次。如果 rpc调用 完成且未被取消，则 done（完成）后将调用 callback。
 * 如果在调用 NotifyOnCancel() 时已取消 rpc调用，则将立即调用 callback。每个请求必须调用不超过一次 NotifyOnCancel()
 * @param callback 调用方(客户端)通知取消rpc调用时，给定的回调
 */
void MPRpcController::NotifyOnCancel(google::protobuf::Closure* callback)
{
    m_callback = callback;
}
