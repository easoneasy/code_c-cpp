#include <iostream>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>

using namespace std;
using namespace muduo::net;
using namespace muduo;

void onConnection(const TcpConnectionPtr &conn)
{
    // 打印日志：客户端地址 -> 服务器地址 ，连接状态
    LOG_INFO << "daytimeServer - "
        << conn->peerAddress().toIp() << " -> "
        << conn->peerAddress().toIpPort() << " is "
        << (conn->connected() ? "UP" : "DOWN");
    if(conn->connected())
    {
        // 发送当前时间字符串
        conn->send(Timestamp::now().toFormattedString()+"\n");
        // 主动断开连接
        conn->forceClose();
    }
}

void onMessage(const TcpConnectionPtr &conn,Buffer *buf,Timestamp time)
{
    // 无论客户端发来什么，全部从缓冲区取出来并丢弃
    string msg(buf->retrieveAllAsString());

    LOG_INFO << conn->name() << " discards " << msg.size()
        << " bytes received at " << time.toString();
}

int main(int argc,char *argv[])
{
    // 创建核心事件循环
    EventLoop loop;
    // 创建服务器要监听的ip+port
    InetAddress listenAddr(2013);
    // 实例化muduo库自带的TcpServer对象
    TcpServer server(&loop,listenAddr,"daytimeServer");
    // 把全局函数的名称传给服务器，回调函数
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    // 启动服务器并进入事件循环
    server.start();
    loop.loop();

    return 0;
}
