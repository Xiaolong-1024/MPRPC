//
// Created by Y2000 on 2024/12/3.
//

#ifndef MPRPCLOGGER_H
#define MPRPCLOGGER_H

#include "mprpclockqueue.h"

// 普通日志宏函数
#define LOG_INFO(msg, ...) \
    do \
    { \
        char c[1024] = {0}; \
        snprintf(c, 1024, msg, ##__VA_ARGS__); \
        MPRpcLogger::GetInstance().Log(LogLevel::INFO, c); \
    } while (0)

// 警告日志宏函数
#define LOG_WARNING(msg, ...) \
    do \
    { \
        char c[1024] = {0}; \
        snprintf(c, 1024, msg, ##__VA_ARGS__); \
        MPRpcLogger::GetInstance().Log(LogLevel::WARNING, c); \
    } while (0)

// 错误日志宏函数
#define LOG_ERROR(msg, ...) \
    do \
    { \
        char c[1024] = {0}; \
        snprintf(c, 1024, msg, ##__VA_ARGS__); \
        MPRpcLogger::GetInstance().Log(LogLevel::ERROR, c); \
    } while (0)

// 日志级别
enum LogLevel
{
    INFO = 0, // 普通信息
    WARNING = 1, // 警告信息
    ERROR = 2 // 错误信息
};

class MPRpcLogger
{
private:
    MPRpcLogger();

    // 禁止拷贝构造函数
    MPRpcLogger(const MPRpcLogger &) = delete;

    // 禁止移动构造函数
    MPRpcLogger(const MPRpcLogger &&) = delete;

    // 禁止拷贝赋值操作符
    MPRpcLogger &operator=(const MPRpcLogger &) = delete;

    // 禁止移动赋值操作符
    MPRpcLogger &operator=(const MPRpcLogger &&) = delete;

public:
    // 获取单例对象
    static MPRpcLogger& GetInstance();

    // 日志输出
    void Log(LogLevel level, const std::string &msg);

private:
    // 日志队列
    MPRpcLockQueue<std::string> m_lckQue;

    // 日志线程任务
    void LogTask();
};

#endif //MPRPCLOGGER_H
