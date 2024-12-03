//
// Created by Y2000 on 2024/12/3.
//

#include "mprpclogger.h"
#include <thread>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <bits/fs_ops.h>
#include <bits/fs_path.h>

MPRpcLogger::MPRpcLogger()
{
    // 启动写日志线程
    std::thread logTask(&MPRpcLogger::LogTask, this);
    logTask.detach();
}

/**
 * 获取单例对象
 * @return 单例对象
 */
MPRpcLogger& MPRpcLogger::GetInstance()
{
    static MPRpcLogger instance;
    return instance;
}

/**
 * 日志输出
 * @param level 日志级别
 * @param msg 日志内容
 */
void MPRpcLogger::Log(LogLevel level, const std::string& msg)
{
    // 将日志级别转为字符串
    std::string levelStr;
    switch (level)
    {
    case LogLevel::INFO:
        levelStr = "INFO";
        break;
    case LogLevel::WARNING:
        levelStr = "WARNING";
        break;
    case LogLevel::ERROR:
        levelStr = "ERROR";
        break;
    default:
        break;
    }

    // 获取当前时间戳字符串
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S ");
    std::string timestamp = ss.str();

    // 拼接日志
    std::string log_info;
    std::stringstream log_ss;
    log_ss << "[" << levelStr << "] " << timestamp << msg << std::endl;
    log_info = log_ss.str();

    // 将日志信息放入队列
    m_lckQue.push(log_info);
}

/**
 * 写日志线程任务
 */
void MPRpcLogger::LogTask()
{
    // 如果./log文件夹不存在，则创建
    std::string logPath = "./logs/";
    if (!std::filesystem::exists(logPath))
    {
        // 创建目录
        if (!std::filesystem::create_directory(logPath))
        {
            // 如果创建失败
            logPath = "./";
        }
    }

    // 循环获取日志信息并写入文件
    while (true)
    {
        // 获取当前时间戳字符串
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d");
        std::string timestamp = ss.str();

        // 根据日期在./log中生成日志文件
        std::string logFileName = logPath + timestamp + ".log";

        // 打开日志文件
        std::ofstream logFile(logFileName, std::ios::app);
        if (!logFile.is_open())
        {
            // 如果打开失败，输出错误信息
            LOG_ERROR("Failed to open log file: %s", logFileName.c_str());
            break;
        }

        // 获取队列中所日志信息
        std::string log_info = m_lckQue.pop();

        // 将日志信息写入文件
        logFile << log_info;

        // 关闭日志文件
        logFile.close();
    }
}
