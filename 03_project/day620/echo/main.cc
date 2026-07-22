#include "echo.h"
#include <iostream>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

using namespace std;
using namespace muduo::net;
using namespace muduo;


int main(int argc,char *argv[])
{

    // 创建事件循环对象
    EventLoop loop;
    // 监听地址
    InetAddress listenAddr(2007);
    // 创建server
    echoServer server(&loop,listenAddr);
    // 启动服务器
    server.start();
    // 进入事件循环
    loop.loop();

    return 0;
}
