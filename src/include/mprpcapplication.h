//
// Created by Y2000 on 2024/11/29.
//

#ifndef MPRPCAPPLICATION_H
#define MPRPCAPPLICATION_H

// MPRPC框架基础类，负责框架初始化操作
class MPRpcApplication
{
private:
    MPRpcApplication() = default;

    // 禁止拷贝构造函数
    MPRpcApplication(const MPRpcApplication &) = delete;

    // 禁止移动构造函数
    MPRpcApplication(const MPRpcApplication &&) = delete;

    // 禁止拷贝赋值操作符
    MPRpcApplication &operator=(const MPRpcApplication &) = delete;

    // 禁止移动赋值操作符
    MPRpcApplication &operator=(const MPRpcApplication &&) = delete;

public:
    // 获取单例对象
    static MPRpcApplication& GetInstance();

public:
    // 初始化框架, 加载配置文件
    static void Init(int argc, char **argv);
};

#endif //MPRPCAPPLICATION_H
