//
// Created by Y2000 on 2024/11/29.
//

#include "mprpcconfigure.h"
#include "mprpclogger.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>

/**
 * 获取单例对象
 * @return 单例对象
 */
MPRpcConfigure& MPRpcConfigure::GetInstance()
{
    static MPRpcConfigure instance;
    return instance;
}

/**
 * 加载解析配置文件
 * @param path 配置文件路径
 * @return 成功返回true, 失败返回false
 */
bool MPRpcConfigure::LoadConfigure(const std::string& path)
{
    // 检测路径是否存在
    if (!std::filesystem::exists(path))
    {
        LOG_ERROR("The configuration file path does not exist: %s", path.c_str());
        return false;
    }

    // 判断路径是否为文件
    if (!std::filesystem::is_regular_file(path))
    {
        LOG_ERROR("The configuration file path is not a file: %s", path.c_str());
        return false;
    }

    // 打开文件
    std::ifstream ifs(path);
    if (!ifs.is_open())
    {
        LOG_ERROR("Failed to open configure file: %s", path.c_str());
        return false;
    }

    // 读取文件内容
    std::string line; // 创建一个字符串变量，用于存储读取的每一行内容
    while (std::getline(ifs, line)) // 逐行读取文件内容
    {
        // 判断是否为空行
        if (line.empty())
        {
            continue;
        }

        // 判断是否为注释信息
        if (line.find('#') != std::string::npos)
        {
            continue;
        }

        // 判断是否为配置项
        if (line.find('=') != std::string::npos)
        {
            // 去除所有空白字符（空格、制表符、换行符、回车符、垂直制表符和换页符）
            line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());  // 检查普通字符是否为空白字符
            line.erase(remove_if(line.begin(), line.end(), ::iswspace), line.end()); // 检查宽字符是否为空白字符

            // 解析配置项
            std::stringstream ss(line);  // 创建一个字符串流，将line作为输入
            char delimiter = '='; // 定义分隔符为等号
            std::string token; // 创建一个字符串变量，用于存储分割后的字符串
            std::vector<std::string> tokens; // 创建一个字符串数组，用于存储分割后的字符串
            while (std::getline(ss, token, delimiter)) // 将字符串流按照分隔符分割
            {
                // 将分割后的字符串存储到tokens中
                if (!token.empty())
                {
                    tokens.push_back(token);
                }
            }

            // 判断配置项是否合法
            if (tokens.size() != 2)
            {
                LOG_ERROR("Invalid configure line: %s", line.c_str());
                continue;
            }

            // 插入配置项
            m_configMap.insert({tokens[0], tokens[1]});
        }
    }

    // 检测配置项
    if (!CheckConfigure())
    {
        return false;
    }

    // 关闭文件
    ifs.close();

    return true;
}

/**
 * 获取配置项
 * @param key 配置项名称
 * @return 配置项内容
 */
std::string MPRpcConfigure::GetConfigure(const std::string& key)
{
    std::string value;

    // 判断配置项是否存在
    if (m_configMap.contains(key))
    {
        // 获取配置项内容
        value = m_configMap.at(key);
    }
    else
    {
        LOG_ERROR("If the configuration item is not found, MPRpcApplication::Init() may not be called");
    }

    return value;
}

/**
 * 检查配置文件是否正确
 * @return 成功返回true, 失败返回false
 */
bool MPRpcConfigure::CheckConfigure()
{
    // 检测配置项是否为空
    if (m_configMap.empty())
    {
        LOG_ERROR("No configuration item in file");
        return false;
    }

    // 检测配置项是否存在
    if (m_configMap.find("rpcserverip") == m_configMap.end())
    {
        LOG_ERROR("\"rpcserverip\" configuration item is not found in file");
        return false;
    }
    if (m_configMap.find("rpcserverport") == m_configMap.end())
    {
        LOG_ERROR("\"rpcserverport\" configuration item is not found in file");
        return false;
    }
    if (m_configMap.find("zkserverip") == m_configMap.end())
    {
        LOG_ERROR("\"zkserverip\" configuration item is not found in file");
        return false;
    }
    if (m_configMap.find("zkserverport") == m_configMap.end())
    {
        LOG_ERROR("\"zkserverport\" configuration item is not found in file");
        return false;
    }

    for (auto& [key, value] : m_configMap)
    {
        LOG_INFO("%s = %s", key.c_str(), value.c_str());
    }

    return true;
}
