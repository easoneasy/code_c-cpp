#include "echo.h"
#include <iostream>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <netdb.h>
#include <muduo/base/Logging.h>

using namespace std;
using namespace muduo::net;
using namespace muduo;

// 构造函数
echoServer::echoServer(muduo::net::EventLoop *loop,
    const muduo::net::InetAddress &listenAddr)
:_server(loop,listenAddr,"echoServer")
{
    _server.setConnectionCallback(std::bind(&echoServer::onConnection,this,_1));
    _server.setMessageCallback(std::bind(&echoServer::onMessage,this,_1,_2,_3));
}

// 启动服务器
void echoServer::start()
{
    _server.start();
}

// 连接事件回调函数
void echoServer::onConnection(const TcpConnectionPtr &conn)
{
    // 输出连接信息：客户端地址 -> 本地地址 的状态（UP/DOWN）
    LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
    << conn->localAddress().toIpPort() << " is "
    << (conn->connected() ? "UP" : "DOWN");
}
// 消息回调函数
void echoServer::onMessage(const TcpConnectionPtr &conn,Buffer *buf,Timestamp time)
{
    // 将缓冲区中的所有数据取出为一个字符串
    string msg(buf->retrieveAllAsString());
    // 日志记录：连接名、字节数、接收时间
    LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
    << "data received at " << time.toString();
    // 原样发送回客户端
    conn->send(msg);
}
