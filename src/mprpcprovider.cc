//
// Created by Y2000 on 2024/11/29.
//

#include "mprpcprovider.h"
#include "mprpcconfigure.h"
#include "rpcheader.pb.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/InetAddress.h>

/**
 * 注册rpc服务方法
 * @param service rpc服务
 */
void MPRpcProvider::NotifyService(google::protobuf::Service* service)
{
    // 获取rpc服务描述指针
    const google::protobuf::ServiceDescriptor* pServiceDesc =  service->GetDescriptor();

    // 遍历服务中的所有rpc方法
    RpcServerInfo service_info; // rpc服务信息
    const google::protobuf::MethodDescriptor* pMethodDesc = nullptr; // rpc方法描述指针
    for (int i = 0; i < pServiceDesc->method_count(); ++i)
    {
        // 获取rpc方法描述
        pMethodDesc = pServiceDesc->method(i);

        // 将rpc方法名和方法描述存入服务信息的map中
        service_info.methodMap.insert({pMethodDesc->name(), pMethodDesc});
    }

    // 存储注册成功的rpc服务信息
    service_info.service = service; // 将rpc服务对象存入服务信息中
    m_serviceMap.insert({pServiceDesc->name(), service_info}); // 将rpc服务名和服务信息存入map中
}

/**
 * 启动rpc服务提供者，开启提供rpc远程网络调用服务
 */
void MPRpcProvider::Run(const uint32_t threadNum)
{
    // 从配置文件对象获取相关配置项
    std::string ip; uint16_t port;
    try
    {
        ip = MPRpcConfigure::GetInstance().GetConfigure("rpcserverip");
        port = stoi(MPRpcConfigure::GetInstance().GetConfigure("rpcserverport"));
    }
    catch (const std::invalid_argument& e)
    {
        std::cout << "MPRpcProvider::Run() error: " << e.what() << std::endl;
        return;
    }

    // 创建TcpServer对象
    muduo::net::InetAddress listenAddr(ip, port);
    muduo::net::TcpServer server(&m_eventLoop, listenAddr, "MprpcProvider");

    // 绑定回调函数
    server.setConnectionCallback(std::bind(&MPRpcProvider::OnConnection, this, std::placeholders::_1)); // 连接事件回调
    server.setMessageCallback(std::bind(&MPRpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3)); // 消息事件回调

    // 设置线程数量
    server.setThreadNum(threadNum);

    // 开启服务
    server.start();
    std::cout << "MPRpcProvider server at in " << ip << ":" << port << std::endl;
    std::cout << "MPRpcProvider server is running..." << std::endl;

    // 启动事件循环
    m_eventLoop.loop();
}

/**
 * 回调函数，当有新的连接和断开连接时调用
 * @param conn 连接对象
 */
void MPRpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn)
{
    // rpc请求是短连接（相当于http），处理完请求，返回响应后，然后主动关闭连接

    // 如果连接断开，关闭连接
    if (!conn->connected())
    {
        conn->shutdown();
    }
}

/**
 * 回调函数，当有新的消息到达时调用
 * 由服务发布者和服务消费者协商好之间通信的数据包,
 * 定义一个protobuf的message类型(RpcHeader)，进行数据头的序列化和反序列化,
 * 数据头: 存储数据头的长度 和 rpc方法描述 (服务名称 + 方法名称 + 参数长度) 的 序列化字符串
 * 数据体: rpc方法参数 的 message类型 的 序列化字符串,
 * header_size(4个字节) + header_data + args_data
 * @param conn 连接对象
 * @param buf 消息缓冲区
 * @param time 消息到达时间
 */
void MPRpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time)
{
    // 处理rpc请求，调用对应的服务和方法，返回结果

    // 将消息缓冲区中的数据读取出来，反序列化为字符串
    std::string recv_str = buf->retrieveAllAsString();

    // 获取数据头长度
    uint32_t header_size = 0; // 存储数据头长度
    recv_str.copy(reinterpret_cast<char*>(&header_size), 4, 0); // 从字符串中读取前4个字节，作为header_size

    // 根据数据头长度读取数据头原始数据
    std::string rpc_header_str = recv_str.substr(4, header_size); // 从字符串中读取第5个字节开始，长度为header_size的字符串

    // 数据头原始数据反序列化为为数据头对象
    mprpc::RpcHeader rpc_header;
    std::string service_name; // rpc服务名称
    std::string method_name;  // rpc方法名称
    uint32_t args_size;   // 参数长度 (rpc请求消息类型序列化后的长度)
    if (rpc_header.ParseFromString(rpc_header_str))
    {
        // 数据头反序列化成功
        service_name = rpc_header.service_name();
        method_name = rpc_header.method_name();
        args_size = rpc_header.args_size();
    }
    else
    {
        // 数据头反序列化失败
        std::cout << "MPRpcProvider::OnMessage() error: parse rpc header failed" << std::endl;
        return;
    }

    std::cout << "==========================================" << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "recv_str: " << recv_str << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "==========================================" << std::endl;

    // 根据服务名称获取服务信息对象
    if (!m_serviceMap.contains(service_name))
    {
        std::cout << "MPRpcProvider::OnMessage() error: service " << service_name << " not found" << std::endl;
        return;
    }
    const RpcServerInfo server_info =  m_serviceMap.at(service_name);

    // 根据方法名称获取方法对象
    if (!server_info.methodMap.contains(method_name))
    {
        std::cout << "MPRpcProvider::OnMessage() error: method " << method_name << " not found" << std::endl;
        return;
    }
    const google::protobuf::MethodDescriptor* pMethodDesc = server_info.methodMap.at(method_name);

    // 创建rpc请求消息类型对象和rpc响应消息类型对象：由protobuf进行释放
    google::protobuf::Message* pRequest = server_info.service->GetRequestPrototype(pMethodDesc).New(); // 根据方法对象获取rpc请求消息类型
    google::protobuf::Message* pResponse = server_info.service->GetResponsePrototype(pMethodDesc).New(); // 根据方法对象获取rpc响应消息类型

    // 获取数据体(rpc方法参数)原始数据
    std::string rpc_args_str = recv_str.substr(4 + header_size, args_size); // 从字符串中读取第5 + header_size个字节开始，长度为args_size的字符串

    // 数据体原始数据反序列化为为rpc请求消息类型
    if (!pRequest->ParseFromString(rpc_args_str))
    {
        std::cout << "MPRpcProvider::OnMessage() error: parse rpc args failed" << std::endl;
        return;
    }

    // 创建rpc方法执行完成回调对象，绑定: MprpcProvider::OnDone()，由protobuf进行释放
    google::protobuf::Closure* pClosure = google::protobuf::NewCallback<
        MPRpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>(
        this, &MPRpcProvider::OnDone, conn, pResponse);

    // 调用对应服务的rpc方法，获取结果
    server_info.service->CallMethod(pMethodDesc, nullptr, pRequest, pResponse, pClosure);
}

/**
 * 回调函数，当rpc方法执行完毕时调用
 * @param conn 连接对象
 * @param pResponse rpc响应消息类型对象
 */
void MPRpcProvider::OnDone(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* pResponse)
{
    // 将rpc响应消息类型序列化为字符串，并发送给客户端

    // rpc响应消息类型序列化为字符串
    std::string response_str;
    if (!pResponse->SerializeToString(&response_str))
    {
        std::cout << "MprpcProvider::OnDone() error: serialize response failed" << std::endl;
        conn->shutdown();
        return;
    }

    // 发送rpc响应消息类型字符串给客户端
    conn->send(response_str);

    // 主动断开连接
    conn->shutdown();
}
