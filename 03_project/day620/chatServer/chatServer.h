#pragma once

#include "lengthHeaderCodec.h"
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
#include <set>

class chatServer
{
public:
    // 构造函数
    // 把onEntireMessage绑定注册给_codec
    chatServer(muduo::net::EventLoop *loop,const muduo::net::InetAddress listenAddr);
    // 启动服务器
    void start();
private:
    // 处理连接和断开事件
    void onConnection(const muduo::net::TcpConnectionPtr &conn);
    // 广播给其他连接
    void onEntireMessage(const muduo::net::TcpConnectionPtr &conn,
        const std::string &msg,muduo::Timestamp receiveTime);

    // tcp服务器
    muduo::net::TcpServer _server;
    // 编解码器
    lengthHeaderCodec _codec;
    // 活跃的连接集合
    std::set<muduo::net::TcpConnectionPtr> _connections;
};
