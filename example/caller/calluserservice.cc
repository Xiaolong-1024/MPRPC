//
// Created by Y2000 on 2024/12/2.
//

#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "user.pb.h"
#include <iostream>
#include <user.pb.h>

int main(int argc, char **argv)
{
    // 初始化mprpc框架
    MPRpcApplication::Init(argc, argv);

    // 创建服务桩类
    fixbug::UserServiceRpc_Stub stub(new MPRpcChannel());

    // 创建请求和响应对象
    fixbug::LoginRequest request; // 请求对象
    fixbug::LoginResponse response; // 响应对象

    // 设置请求参数
    request.set_username("zhangsan");
    request.set_password("123456");

    // 发起一次rpc方法调用，它是同步调用，会等待远程响应
    stub.Login(nullptr, &request, &response, nullptr);

    // 一次rpc调用完成，获取响应消息
    if (response.result().code() == 0)
    {
        // 调用成功
        std::cout << "login rpc response success: " << response.result().message() << std::endl;
    }
    else
    {
        // 调用失败
        std::cout << "login rpc response failed: " << response.result().message() << std::endl;
    }

    return 0;
}