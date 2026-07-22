#pragma once

#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/TcpServer.h>
class daytimeServer
{
public:
    // 构造函数
    daytimeServer(muduo::net::EventLoop *loop,
        const muduo::net::InetAddress &listenAddr);

    // 启动服务（开始监听）
    void start();

private:

    // 连接回调函数，当新连接建立或断开时被调用
    void onConnection(const muduo::net::TcpConnectionPtr &conn);

    // 消息回调函数，当收到客户端数据时被调用
    void onMessage(const muduo::net::TcpConnectionPtr &conn,
        muduo::net::Buffer *buf,muduo::Timestamp time);
private:
    // TCP 服务器对象，管理监听和连接
    muduo::net::TcpServer _server;

};
