//
// Created by Y2000 on 2024/12/2.
//

#ifndef MPRPCCHANNEL_H
#define MPRPCCHANNEL_H

#include <google/protobuf/service.h>

// rpc通道, 用于调用远程方法
class MPRpcChannel : public google::protobuf::RpcChannel
{
public:
    ~MPRpcChannel() override = default;

    // 调用rpc方法
    // 通过rpc服务桩调用方法最终都会调用 MPRpcChannel 的 CallMethod()
    void CallMethod(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller,
                            const google::protobuf::Message* request, google::protobuf::Message* response,
                            google::protobuf::Closure* done = nullptr) override;

  private:
};

#endif //MPRPCCHANNEL_H
