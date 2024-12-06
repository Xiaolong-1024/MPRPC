//
// Created by Y2000 on 2024/12/4.
//

#include "mprpczkclient.h"
#include "mprpcconfigure.h"
#include "mprpclogger.h"
#include <semaphore.h>

/**
 * zookeeper 连接监听器回调
 * zookeeper服务端 通过这个回调给 zookeeper客户端 通知
 * @param zh zookeeper句柄
 * @param type 事件类型
 * @param state 事件状态
 * @param path 事件路径
 * @param watcherCtx 上下文
 */
void global_watcher(zhandle_t* zh, int type, int state, const char* path,
                    void* watcherCtx)
{
    // 会话事件发生
    if (type == ZOO_SESSION_EVENT)
    {
        // 如果zookeeper连接成功
        if (state == ZOO_CONNECTED_STATE)
        {
            // 将watcherCtx转换为sem_t类型
            sem_t* sem = (sem_t*)watcherCtx;

            // 改变信号量的值，通知其他线程 (sem++)
            sem_post(sem);
        }
    }
}

MPRpcZKClient::MPRpcZKClient() : m_zkHandle(nullptr)
{
    // 启动zookeeper客户端（连接zookeeper服务端）
    Start();
}

MPRpcZKClient::~MPRpcZKClient()
{
    if (m_zkHandle != nullptr)
    {
        // 关闭句柄，释放资源
        zookeeper_close(m_zkHandle);
    }
}

/**
 * 获取单例对象
 * @return 单例对象
 */
MPRpcZKClient& MPRpcZKClient::GetInstance()
{
    static MPRpcZKClient instance;
    return instance;
}

/**
 * 启动zookeeper客户端（连接zookeeper服务端）
 */
void MPRpcZKClient::Start()
{
    // 获取zookeeper配置信息
    std::string ip = MPRpcConfigure::GetInstance().GetConfigure("zkserverip");
    std::string port = MPRpcConfigure::GetInstance().GetConfigure("zkserverport");

    // 创建zookeeper连接参数
    std::string connectStr = ip + ":" + port; // zookeeper服务端地址
    int timeout = 3000; // 连接超时时间

    // 连接zookeeper服务端，返回句柄
    // 执行 zookeeper_init 后返回，连接 zookeeper 服务端并没有成功，
    // 当接收到服务端响应后，会调用了回调函数（global_watcher），可以在里面判断连接状态，并改变信号量的值
    m_zkHandle = zookeeper_init(connectStr.c_str(), global_watcher, timeout, nullptr, nullptr, 0);
    if (m_zkHandle == nullptr)
    {
        LOG_ERROR("zookeeper init error");
        return;
    }

    // 创建zookeeper上下文（给句柄添加额外信息）
    sem_t sem; // 信号量
    sem_init(&sem, 0, 0); // 初始化信号量
    zoo_set_context(m_zkHandle, &sem); // 给句柄添加信号量，当连接成功后会改变信号量的值

    // 等待zookeeper连接成功
    sem_wait(&sem); // 阻塞等待信号量改变（当sem为0的时候, 引起阻塞）
    LOG_INFO("zookeeper connect success");
}

/**
 * 创建zookeeper节点
 * @param path 节点路径
 * @param data 节点数据
 * @param state 节点状态: true:持久节点 false:临时节点
 */
void MPRpcZKClient::Create(std::string path, std::string data, bool state)
{
    // 检查指定路径的节点是否存在
    int flag = zoo_exists(m_zkHandle, path.c_str(), 0, nullptr);

    // 如果节点不存在
    if (ZNONODE == flag)
    {
        // 创建节点
        std::string buffer(256, '\0');
        flag = zoo_create(m_zkHandle, path.c_str(), data.c_str(), data.size(), &ZOO_OPEN_ACL_UNSAFE, state, buffer.data(), buffer.size());
        if (flag == ZOK)
        {
            // 创建成功
            LOG_INFO("create node success");
        }
        else
        {
            // 创建失败
            LOG_ERROR("create node error: %d", flag);
            return;
        }
    }
}

/**
 * 获取zookeeper节点数据
 * @param path 节点路径
 * @return 节点数据
 */
std::string MPRpcZKClient::GetData(std::string path)
{
    std::string buffer(1024, '\0');
    int bufferSize = 1024;

    // 获取节点数据
    int flag = zoo_get(m_zkHandle, path.c_str(), 0, buffer.data(), &bufferSize, nullptr);
    if (flag == ZOK)
    {
        // 获取成功
        LOG_INFO("get data success");
        buffer.reserve(bufferSize);
    }
    else
    {
        // 获取失败
        LOG_ERROR("get data error: %d", flag);
        buffer.clear();
        buffer.reserve(0);
    }

    return buffer;
}
