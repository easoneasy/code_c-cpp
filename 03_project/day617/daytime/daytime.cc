#include "daytime.h"
#include <iostream>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>

using namespace std;
using namespace muduo::net;
using namespace std::placeholders;

// 构造函数实现
daytimeServer::daytimeServer(EventLoop *loop,const InetAddress &listenAddr)
:_server(loop, listenAddr, "DaytimeServer")
{
    // 注册连接回调函数，当连接建立或关闭时，调用onConnection
    _server.setConnectionCallback(std::bind(&daytimeServer::onConnection,this,_1));

    // 注册消息回调函数，当收到数据时，调用onMessage
    _server.setMessageCallback(std::bind(&daytimeServer::onMessage,this,_1,_2,_3));
}

// 启动服务器（内部调用TcpServer的start方法
void daytimeServer::start()
{
    _server.start();
}

// 连接回调函数的实现
void daytimeServer::onConnection(const TcpConnectionPtr &conn)
{
    // 记录连接信息： 客户端地址 -> 服务器地址， 状态up或down
    LOG_INFO << "DaytimeServer - " << conn->peerAddress().toIpPort()
    << " -> " << conn->localAddress().toIpPort() << " is "
    << (conn->connected() ? "UP" : "DOWN");

    // 如果连接已经建立（UP状态）
    if(conn->connected())
    {
        // 发送当前时间的字符串
        conn->send(muduo::Timestamp::now().toFormattedString()+ "\n");
        // 只关闭写端，服务器进入半连接状态，依然可以读取客户端的数据
        conn->shutdown();

        // 主动关闭连接（daytime协议：发送完时间后服务器关闭连接）
        // conn->forceClose();
    }
}

// 消息回调函数的实现
void daytimeServer::onMessage(const TcpConnectionPtr &conn,Buffer *buf,
    muduo::Timestamp time)
{
    // 读取缓冲区中的所有数据
    string msg(buf->retrieveAllAsString());

    // 记录日志，丢弃了多少字节的数据，以及收到数据的时间
    LOG_INFO << conn->name() << " discards " << msg.size()
        << "bytes receive at " << time.toString();
}
