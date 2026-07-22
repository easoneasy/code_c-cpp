#include "daytime.h"
#include <iostream>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

using namespace std;


int main(int argc,char *argv[])
{
    // 创建事件循环
    muduo::net::EventLoop loop;
    // 创建监听地址
    muduo::net::InetAddress listenAddr(2013);
    // 创建daytime服务器
    daytimeServer server(&loop,listenAddr);
    // 启动服务器
    server.start();
    // 进入事件循环
    loop.loop();


    return 0;
}
