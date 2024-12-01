//
// Created by Y2000 on 2024/11/27.
//
#include "test.pb.h"
#include <iostream>

using namespace std;
using namespace test;

int main()
{
//    LoginRequest request;
//
//    // 封装对象的数据
//    request.set_username("Y2000");
//    request.set_password("123456");
//    cout << "request: " << request.username() << endl;
//    cout << "request: " << request.password() << endl;
//
//    // 序列化对象: 将对象数据序列化成字符串
//    string str;
//    if (request.SerializeToString(&str))
//    {
//        cout << str << endl;
//    }
//
//    // 反序列化对象: 将字符串反序列化成对象
//    LoginRequest request2;
//    if (request2.ParseFromString(str))
//    {
//        cout << "request2: " << request2.username() << endl;
//        cout << "request2: " << request2.password() << endl;
//    }

//    LoginResponse loginResponse;
//    ResultCode* resultcode = loginResponse.mutable_code(); // mutable返回指针
//    resultcode->set_code(200);
//    resultcode->set_message("success");
//    cout << "code: " << loginResponse.code().code() << endl;
//    cout << "message: " << loginResponse.code().message() << endl;
//
//    GetFriendListsResponse response;
//
//    //---------------------添加第一个元素------------------------------
//    ResultCode* result1 = response.add_code();
//    result1->set_code(200);
//    result1->set_message("success");
//    //---------------------添加第二个元素------------------------------
//    ResultCode* result2 = response.add_code();
//    result2->set_code(404);
//    result2->set_message("failed");
//    //---------------------遍历数组------------------------------
//    cout << response.code_size() << endl;
//    for (auto code : response.code())
//    {
//        cout << code.code() << " " << code.message() << endl;
//    }
//
//    //---------------------添加第一个元素------------------------------
//    User* user1 = response.add_user();
//    user1->set_name("Tom");
//    user1->set_age(20);
//    user1->set_sex(User_Sex_MALE);
//    //---------------------添加第二个元素------------------------------
//    User* user2 = response.add_user();
//    user2->set_name("Marry");
//    user2->set_age(18);
//    user2->set_sex(User_Sex_FEMALE);
//    //---------------------遍历数组------------------------------
//    cout << response.user_size() << endl;
//    for (auto user : response.user())
//    {
//      cout << user.name() << " " << user.age() << " " << user.sex() << endl;
//    }
//
//    // 序列化对象: 将对象数据序列化成字符串
//    string str;
//    if (response.SerializeToString(&str))
//    {
//        cout << str << endl;
//    }
//
//    // 反序列化对象: 将字符串反序列化成对象
//    GetFriendListsResponse response2;
//    if (response2.ParseFromString(str))
//    {
//        for (auto code : response2.code())
//        {
//            cout << code.code() << " " << code.message() << endl;
//        }
//        for (auto user : response2.user())
//        {
//            cout << user.name() << " " << user.age() << " " << user.sex() << endl;
//        }
//    }

    TestMap test;

    // 获取map
    auto* map1 = test.mutable_map1();

    // 插入元素
    map1->insert({"id", 1001});

    // 返回大小
    cout << map1->size() << endl;

    // 遍历map
    for (auto& [key, value] : *map1)
    {
        cout << key << " " << value << endl;
    }

    // 修改元素
    map1->at("id") = 1002;

    // 获取元素
    if (map1->contains("id")) // 判断该元素是否存在
    {
        cout << map1->at("id") << endl;
    }

    // 删除元素
    map1->erase("id");
    cout << map1->size() << endl;

    // 清空map
    map1->clear();

    //-----------------------------------------------------------------

    auto* map2 = test.mutable_map2();

    // 插入自定义类型元素
    User user1;
    user1.set_name("Tom");
    user1.set_age(20);
    user1.set_sex(User_Sex_MALE);
    map2->insert({1, user1});

    // 获取自定义类型元素
    if (map2->contains(1))
    {
        User user2 = map2->at(1);
        cout << user2.name() << " " << user2.age() << " " << user2.sex() << endl;
    }

    // 修改自定义元素
    User user3;
    user3.set_name("Marry");
    user3.set_age(18);
    user3.set_sex(User_Sex_FEMALE);
    map2->at(1) = user3;
    cout << map2->at(1).name() << " " << map2->at(1).age() << " " << map2->at(1).sex() << endl;

    // 遍历自定义元素
    for (auto& [key, value] : *map2)
    {
        cout << key << " " << value.name() << " " << value.age() << " " << value.sex() << endl;
    }

    // 删除自定义元素
    map2->erase(1);

    // 清空map
    map2->clear();
    cout << map2->size() << endl;



    return 0;
}


