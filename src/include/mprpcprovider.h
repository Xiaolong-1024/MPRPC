//
// Created by Y2000 on 2024/11/29.
//

#ifndef MPRPCPROVIDER_H
#define MPRPCPROVIDER_H

#include <google/protobuf/service.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

// MPRPC框架服务发布类，专门用于发布rpc服务
class MprpcProvider
{
public:
    // 用于发布rpc方法的接口
    void NotifyService(google::protobuf::Service* service);

    // 启动rpc服务，开始提供远程网络调用服务
    void Run();

private:
    // 回调函数，当有新的连接时调用
    void onConnection(const muduo::net::TcpConnectionPtr& conn);

    // 回调函数，当有新的消息到达时调用
    void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time);

private:
    // EventLoop对象
    muduo::net::EventLoop m_eventLoop;
};

#endif //MPRPCPROVIDER_H
