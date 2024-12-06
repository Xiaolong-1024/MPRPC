//
// Created by Y2000 on 2024/12/4.
//

#ifndef MPRPCZKCLIENT_H
#define MPRPCZKCLIENT_H

#include <zookeeper/zookeeper.h>
#include <string>

// zookeeper客户端类
class MPRpcZKClient
{
private:
    MPRpcZKClient();

    ~MPRpcZKClient();

    // 禁止拷贝构造函数
    MPRpcZKClient(const MPRpcZKClient &) = delete;

    // 禁止移动构造函数
    MPRpcZKClient(const MPRpcZKClient &&) = delete;

    // 禁止拷贝赋值操作符
    MPRpcZKClient &operator=(const MPRpcZKClient &) = delete;

    // 禁止移动赋值操作符
    MPRpcZKClient &operator=(const MPRpcZKClient &&) = delete;

public:
    // 获取单例对象
    static MPRpcZKClient& GetInstance();

private:
    // 启动zookeeper客户端
    void Start();

public:
    // 创建zookeeper节点
    void Create(std::string path, std::string data, bool state = false);

    // 获取zookeeper节点数据
    std::string GetData(std::string path);

private:
    // zookeeper客户端句柄
    zhandle_t* m_zkHandle;
};

#endif //MPRPCZKCLIENT_H
