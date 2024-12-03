//
// Created by Y2000 on 2024/11/28.
//

#include <iostream>
#include "user.pb.h"
#include "mprpcapplication.h"

/**
 * UserService 原来是一个本地服务，提供了两个进程内的本地方法
 * fixbug::UserServiceRpc 使用在服务发布端（rpc服务提供者）
 */
class UserService : public fixbug::UserServiceRpc
{
public:
    /**
     * 本地登录业务
     * @param username
     * @param password
     * @return
     */
    bool login(const std::string &username, const std::string &password)
    {
        std::cout << "UserService::login " << username << " " << password << std::endl;
        return true;
    }

    /**
     * 重写基类 UserServiceRpc 的虚函数，实现rpc登录服务
     * @param controller 
     * @param request 
     * @param response 
     * @param done 
     */
    void Login(::google::protobuf::RpcController* controller,
               const ::fixbug::LoginRequest* request,
               ::fixbug::LoginResponse* response,
               google::protobuf::Closure* done)
    {
        // 根据请求参数, 调用本地业务进行处理
        std::string username = request->username();
        std::string password = request->password();
        bool loginRet = login(username, password);

        // 将处理结果通过响应参数传出
        auto* result = response->mutable_result();
        if (loginRet)
        {
            result->set_code(0);
            result->set_message("ok");
            response->set_success(true);
        }
        else
        {
            result->set_code(1);
            result->set_message("error");
            response->set_success(false);
        }

        // 调用回调函数, 告知调用方, RPC执行完成
        // 里面进行了响应参数序列化，然后通过网络发送
        done->Run();
    }
};

int main(int argc, char **argv)
{
    // 初始化mprpc框架
    MPRpcApplication::Init(argc, argv);

    // 创建服务发布者对象
    MPRpcProvider provider;

    // 将UserService对象发布到rpc框架中,供远程调用
    provider.NotifyService(new UserService());

    // 启动rpc服务发布者
    provider.Run();

    return 0;
}
