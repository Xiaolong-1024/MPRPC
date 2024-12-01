//
// Created by Y2000 on 2024/11/29.
//

#ifndef MPRPCCONFIGURE_H
#define MPRPCCONFIGURE_H

#include <unordered_map>
#include <string>

// 读取配置文件类
class MPRpcConfigure
{
private:
    MPRpcConfigure() = default;

    // 禁止拷贝构造函数
    MPRpcConfigure(const MPRpcConfigure &) = delete;

    // 禁止移动构造函数
    MPRpcConfigure(const MPRpcConfigure &&) = delete;

    // 禁止拷贝赋值操作符
    MPRpcConfigure &operator=(const MPRpcConfigure &) = delete;

    // 禁止移动赋值操作符
    MPRpcConfigure &operator=(const MPRpcConfigure &&) = delete;

public:
    // 获取单例对象
    static MPRpcConfigure& GetInstance();

public:
    // 加载解析配置文件
    bool LoadConfigure(const std::string &path);

    // 获取配置项
    std::string GetConfigure(const std::string &key);

private:
    // 检查配置文件是否正确
    bool CheckConfigure();

private:
    std::unordered_map<std::string, std::string> m_configMap;
};

#endif //MPRPCCONFIGURE_H
