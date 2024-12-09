### 介绍
1. 这是一个基于 muduo、protobuf、zookeeper 的RPC(远程调用)框架，用于实现分布式系统中的通信。
2. 目前实现了简单的 RPC 调用(只包含了同步调用，以及单向调用) 和向 zookeeper上注册rpc服务。
3. 使用时需要安装 protobuf、muduo 和 zookeeper-client-c。

### 构建
1. 安装 protobuf
    ```
    sudo apt-get install protobuf-compiler libprotobuf-dev
    ```
2. 安装 muduo
    ```
    git clone https://github.com/chenshuo/muduo.git
    cd muduo
    mkdir build
    cd build
    cmake ..
    make
    sudo make install
    ```
3. 安装 zookeeper-client-c
    ```
    git clone https://github.com/apache/zookeeper.git
    cd zookeeper
    git checkout release-3.4.13
    cd zookeeper-3.4.13/zookeeper-client/zookeeper-client-c
    ./configure
    make
    sudo make install
    ```
4. 编译
    ```
    mkdir build
    cd build
    cmake ..
    make
    sudo make install
    ```
   
### protobuf使用
1. 创建proto文件，定义 rpc 服务，例如：
    ```
    syntax = "proto3";

    package fixbug;
    
    // 生成rpc服务描述消息
    option cc_generic_services = true;
    
    // 定义请求消息类型
    message LoginRequest
    {
        bytes username = 1;
        bytes password = 2;
    }
    
    // 定义响应消息类型
    message LoginResponse
    {
        int32 code = 1;
        string message = 2;
        bool success = 2;
    }
    
    // 定义rpc服务
    service UserServiceRpc
    {
        // rpc方法
        rpc Login(LoginRequest) returns (LoginResponse);
    }
    ```
2. 使用 protobuf 生成服务接口(服务提供者类) 和 stub/桩(服务消费者类)
    ```
    protoc test.proto --cpp_out=./
    ```
3. 被调用方实现服务提供者类rpc接口，例如：
   ```
   #include "user.pb.h"
   #include <iostream>

   class UsererviceImpl : public ::fixbug::UserServiceRpc {
   public:
      void Login(::google::protobuf::RpcController* controller,
               const ::fixbug::LoginRequest* request,
               ::fixbug::LoginResponse* response,
               google::protobuf::Closure* done) 
      {
          std::cout << "username: " << request->username() << std::endl;
          response->set_message("User " + request->username());
          done->Run();
      }
   };
   ```
4. 调用方使用stub/桩(服务消费者类)调用rpc方法
   ```
   #include "user.pb.h"
   #include <iostream>
   
   int main(int argc, char **argv)
   {
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
   
       return 0;
   }
   ```
   
### 框架使用
1. 配置文件
   ```
   # rpc节点ip地址
   rpcserverip=192.168.116.128

   # rpc节点端口
   rpcserverport=8332

   # zk的ip地址
   zkserverip=192.168.116.128

   # zk节点端口
   zkserverport=2181
   ```
2. 服务发布者(被调用方)
   ```
   #include <iostream>
   #include "user.pb.h"
   #include "mprpcapplication.h"

   class UserService : public fixbug::UserServiceRpc
   {
   public:
       // 本地登录业务
       bool login(const std::string &username, const std::string &password)
       {
           std::cout << "UserService::login " << username << " " << password << std::endl;
           return true;
       }

       // 重写基类 UserServiceRpc 的虚函数，实现rpc登录服务
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
   ```
3. 服务调用者(调用方)
   ```
   #include "mprpcapplication.h"
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
       MPRpcController controller; // rpc控制器: 存储rpc调用的状态信息
       stub.Login(&controller, &request, &response, nullptr);

       // 判断rpc调用是否成功
       if (controller.Failed())
       {
           // rpc调用失败
           std::cout << "rpc login response failed: " << controller.ErrorText() << std::endl;
       }
       else 
       {
           // 调用成功
           std::cout << "login rpc response success" << std::endl;
       }
   
       return 0;
   }
   ```
   
### 注意事项 
1. 服务端和客户端的protobuf文件必须一致
2. 服务端和客户端的protobuf文件必须和protobuf编译器一起编译
3. muduo库必须编译为动态库