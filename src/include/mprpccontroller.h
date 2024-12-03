//
// Created by Y2000 on 2024/12/3.
//

#ifndef MPRPCCONTROLLER_H
#define MPRPCCONTROLLER_H

#include <google/protobuf/service.h>

// RPC 控制器，用于控制 RPC 调用 和 获取调用状态
class MPRpcController : public google::protobuf::RpcController
{
public:
    MPRpcController();
    ~MPRpcController() override = default;

public:
    // 将 RpcController 重置为其初始状态，以便它可以在新调用中重复使用。在 rpc调用 正在进行时，不得调用。
    void Reset() override;

    // 调用完成后，如果调用失败，则返回 true, 成功返回false。失败的可能原因取决于 RPC 实现。
    // 在调用完成之前，不得调用 Failed()。如果 Failed()返回 true，则响应消息的内容未定义。
    bool Failed() const override;

    // 如果 Failed() 为 true，则返回错误的描述信息; 如果尚未完成 rpc调用，则返回空字符串。
    std::string ErrorText() const override;

    // 使 Failed() 在调用方(客户端)返回 true。“reason” 将被合并到 ErrorText() 返回的消息中。
    // 如果您发现需要返回有关失败的信息，则应将其合并到响应协议缓冲区中，并且不应调用 SetFailed()
    void SetFailed(const std::string& reason) override;

    // 通知rpc服务发布者(服务端)，当前rpc调用方(客户端)希望取消 rpc调用。rpc服务发布者(服务端)可能会立即取消它，可能会等待一段时间然后取消它，
    // 甚至可能根本不取消调用。如果取消调用，仍会调用 done（完成）回调，此时 RpcController 会提示调用失败
    void StartCancel() override;

    // 如果为 true，则表示调用方(客户端)取消了 rpc调用，因此rpc服务发布者(服务端)也可以放弃回复它。服务器仍应调用最终的 “done” 回调。
    bool IsCanceled() const override;

    // 在取消 rpc调用时，rpc调用方(客户端)给定的回调。回调将始终被调用一次。如果 rpc调用 完成且未被取消，则 done（完成）后将调用 callback。
    // 如果在调用 NotifyOnCancel() 时已取消 rpc调用，则将立即调用 callback。每个请求必须调用不超过一次 NotifyOnCancel()
    void NotifyOnCancel(google::protobuf::Closure* callback = nullptr) override;

private:
    // rpc方法执行过程中的状态
    bool m_failed;

    // rpc方法执行过程中的错误信息
    std::string m_errorText;

    // rpc方法是否取消调用
    bool m_canceled;

    // rpc方法取消调用的回调
    google::protobuf::Closure* m_callback;
};

#endif //MPRPCCONTROLLER_H
