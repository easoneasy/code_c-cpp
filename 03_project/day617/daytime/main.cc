#include "daytime.h"
#include <iostream>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/EventLoop.h>

using namespace std;
using namespace muduo::net;

int main(int argc,char *argv[])
{
    // 创建事件循环
    // 管理所有网络事件
    EventLoop loop;
    // 创建监听地址，监听所有网卡的2013端口
    InetAddress listenAddr(2013);
    // 创建daytime服务器，传入事件循环和监听地址
    daytimeServer server(&loop,listenAddr);
    // 启动服务器（开始监听）
    server.start();
    // 进入事件循环（阻塞运行，直到loop被停止）
    loop.loop();

    return 0;
}
