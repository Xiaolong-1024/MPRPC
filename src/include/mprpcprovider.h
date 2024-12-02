//
// Created by Y2000 on 2024/11/29.
//

#ifndef MPRPCPROVIDER_H
#define MPRPCPROVIDER_H

#include <muduo/net/EventLoop.h>
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <unordered_map>

// MPRPC框架服务发布类，专门用于发布rpc服务
class MprpcProvider
{
public:
    // 用于发布rpc方法的接口
    void NotifyService(google::protobuf::Service* service);

    // 启动rpc服务，开始提供远程网络调用服务
    void Run(uint32_t threadNum = 4);

private:
    // 回调函数，当有新的连接时调用
    void OnConnection(const muduo::net::TcpConnectionPtr& conn);

    // 回调函数，当有新的消息到达时调用
    void OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time);

    // 回调函数，当rpc方法执行完毕时调用
    void OnDone(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* pResponse);

private:
    // EventLoop对象
    muduo::net::EventLoop m_eventLoop;

    // rpc服务类型信息
    struct RpcServerInfo
    {
        // rpc服务对象指针
        google::protobuf::Service* service;

        // rpc方法信息: key-方法名称, value-rpc方法描述对象指针
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> methodMap;
    };

    // 存储注册成功的rpc服务信息: key-服务名称, value-服务信息
    std::unordered_map<std::string, RpcServerInfo> m_serviceMap;
};


#endif //MPRPCPROVIDER_H
