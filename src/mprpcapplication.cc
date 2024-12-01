//
// Created by Y2000 on 2024/11/29.
//

#include "mprpcapplication.h"
#include "mprpcconfigure.h"
#include <iostream>
#include <unistd.h>

/**
 * 获取单例对象
 * @return 单例对象
 */
MPRpcApplication& MPRpcApplication::GetInstance()
{
    static MPRpcApplication instance;
    return instance;
}

/**
 * 初始化框架, 加载配置文件
 * @param argc 命令行参数个数
 * @param argv 命令行参数
 */
void MPRpcApplication::Init(int argc, char** argv)
{
    // 判断命令行参数个数
    if (argc < 2)
    {
        std::cout << "format command -i <configure>" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 解析命令行参数
    int opt = 0;
    std::string config_file;
    while ((opt = getopt(argc, argv, "i:")) != -1)
    {
        switch (opt)
        {
        case 'i': // 出现-i选项
            config_file = optarg;
            break;
        case '?': // 出现未知选项
            std::cout << "format command -i <configure>" << std::endl;
            break;
        case ':': // 出现-i但没有指定配置文件
            std::cout << "format command -i <configure>" << std::endl;
            exit(EXIT_FAILURE);
            break;
        default:
            break;
        }
    }

    // 加载配置文件
    MPRpcConfigure::GetInstance().LoadConfigure(config_file);
}
