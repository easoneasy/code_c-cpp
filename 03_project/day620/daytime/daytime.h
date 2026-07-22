#pragma once

#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>

using namespace std;
using namespace muduo;
using namespace muduo::net;

class daytimeServer
{
    public:
        // 构造函数
        // 创建TcpServer对象
        daytimeServer(muduo::net::EventLoop *loop,
            const muduo::net::InetAddress &listenAddr);
        // 启动服务器
        void start();

    private:
        // 连接事件回调函数
        void onConnection(const muduo::net::TcpConnectionPtr &conn);
        // 消息事件回调函数
        void onMessage(const muduo::net::TcpConnectionPtr &conn,
            muduo::net::Buffer *buf,muduo::Timestamp time);

        muduo::net::TcpServer _server;
};
