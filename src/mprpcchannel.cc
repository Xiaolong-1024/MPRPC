//
// Created by Y2000 on 2024/12/2.
//

#include "mprpcchannel.h"
#include "mprpcconfigure.h"
#include "mprpclogger.h"
#include "rpcheader.pb.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * 调用rpc方法
 * 通过rpc服务桩调用方法最终都会调用 MPRpcChannel 的 CallMethod()
 * 由服务发布者和服务消费者协商好之间通信的数据包,
 * 定义一个protobuf的message类型(RpcHeader)，进行数据头的序列化和反序列化,
 * 数据头: 存储数据头的长度 和 rpc方法描述 (服务名称 + 方法名称 + 参数长度) 的 序列化字符串
 * 数据体: rpc方法参数 的 message类型 的 序列化字符串,
 * header_size(4个字节) + header_data + args_data
 * @param method 方法描述
 * @param controller rpc控制器
 * @param request 请求对象
 * @param response 响应对象
 * @param done 响应成功，执行的回调函数
 */
void MPRpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                              google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                              google::protobuf::Message* response, google::protobuf::Closure* done)
{
    // 检测参数合法性
    if (method == nullptr || controller == nullptr || request == nullptr || response == nullptr)
    {
        LOG_ERROR("MPRpcChannel::CallMethod() parameter is nullptr");
        return;
    }

    // 获取rpc服务描述指针
    const google::protobuf::ServiceDescriptor* pServiceDesc = method->service();

    // 获取rpc服务名称
    std::string serviceName = pServiceDesc->name();

    // 获取rpc方法名称
    std::string methodName = method->name();

    // 获取rpc方法请求参数的序列化长度
    std::string args_str;
    if (!request->SerializeToString(&args_str))
    {
        controller->SetFailed("serialize request failed");
        return;
    }
    uint32_t args_size = args_str.size(); // 获取rpc方法请求参数的序列化长度

    // 定义rpc数据头对象
    mprpc::RpcHeader rpc_header;
    rpc_header.set_service_name(serviceName); // 设置rpc服务名称
    rpc_header.set_method_name(methodName); // 设置rpc方法名称
    rpc_header.set_args_size(args_size); // 设置rpc方法参数长度

    // 获取rpc数据头序列化字符串
    std::string rpc_header_str;
    if (!rpc_header.SerializeToString(&rpc_header_str))
    {
        controller->SetFailed("serialize rpc header failed");
        return;
    }
    uint32_t rpc_header_size = rpc_header_str.size(); // 获取rpc数据头序列化长度

    // 定义rpc请求字符串
    std::string rpc_package;
    rpc_package.insert(0, reinterpret_cast<char*>(&rpc_header_size), 4); //  rpc数据头长度
    rpc_package.append(rpc_header_str); // rpc数据头
    rpc_package.append(args_str); // rpc参数

    // 从配置文件对象获取相关配置项
    std::string ip; uint16_t port;
    try
    {
        ip = MPRpcConfigure::GetInstance().GetConfigure("rpcserverip");
        port = stoi(MPRpcConfigure::GetInstance().GetConfigure("rpcserverport"));
    }
    catch (const std::invalid_argument& e)
    {
        controller->SetFailed("configuration item invalid_argument: " + std::string(e.what()));
        return;
    }

    // 创建tcp客户端
    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == cfd)
    {
        controller->SetFailed("create rpc client socket failed, " + std::string(strerror(errno)));
        return;
    }

    // 连接服务器
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (-1 == connect(cfd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)))
    {
        controller->SetFailed("connect rpc server failed, " + std::string(strerror(errno)));
        close(cfd);
        return;
    }

    // 发送rpc请求字符串
    if (-1 == send(cfd, rpc_package.c_str(), rpc_package.size(), 0))
    {
        controller->SetFailed("send rpc request failed, " + std::string(strerror(errno)));
        close(cfd);
        return;
    }

    // 等待接收rpc响应字符串
    std::string buffer;
    char temp[1024] = {0};
    ssize_t recv_size = 0;
    while (true)
    {
        // 从cfd中接收数据，将数据存储到temp中
        ssize_t n = recv(cfd, temp, sizeof(temp), 0);
        // 如果返回值为-1，表示接收失败
        if (-1 == n)
        {
            controller->SetFailed("recv rpc response failed, " + std::string(strerror(errno)));
            close(cfd);
            return;
        }
        // 如果返回值为0，表示连接已关闭
        if (n == 0)
        {
            break;
        }
        buffer.append(temp, n); // 将接收到的数据添加到buffer中
        recv_size += n; // 更新接收到的字节数
        memset(temp, 0, sizeof(temp)); // 清空temp
    }

    // 进行rpc响应反序列化
    if (!response->ParseFromString(buffer))
    {
        controller->SetFailed("rpc response parse failed");
        close(cfd);
        return;
    }

    // 关闭连接
    close(cfd);

    // 调用执行完成回调函数
    if (done != nullptr)
    {
        done->Run();
    }
}
