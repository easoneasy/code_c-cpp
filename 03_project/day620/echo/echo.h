#pragma once

#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
class echoServer
{
public:
    echoServer(muduo::net::EventLoop *loop,
                const muduo::net::InetAddress &listenAddr);

    void start();

private:
    // 连接建立或关闭的回调函数
    void onConnection(const muduo::net::TcpConnectionPtr &conn);
    // 消息回调函数
    void onMessage(const muduo::net::TcpConnectionPtr &conn,
            muduo::net::Buffer *buf,muduo::Timestamp time);
    muduo::net::TcpServer _server;
};
