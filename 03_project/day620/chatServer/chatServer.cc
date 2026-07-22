#include "chatServer.h"
#include "lengthHeaderCodec.h"
#include <functional>
#include <iostream>
#include <muduo/base/Logging.h>
#include <string>

using namespace std;
using namespace placeholders;


// 构造函数
// 把onEntireMessage绑定注册给_codec
chatServer::chatServer(muduo::net::EventLoop *loop,const muduo::net::InetAddress listenAddr)
:_server(loop, listenAddr, "chatServer")
,_codec(std::bind(&chatServer::onEntireMessage,this,_1,_2,_3))
{
    // 注册连接回调
    _server.setConnectionCallback(std::bind(&chatServer::onConnection,this,_1));
    // 注册消息回调
    _server.setMessageCallback(std::bind(&lengthHeaderCodec::onMessage,_codec,_1,_2,_3));
    // 设置I/O线程数为4
    _server.setThreadNum(4);

}
// 启动服务器
void chatServer::start()
{
    _server.start();
    LOG_INFO << "ChatServer started on port " << _server.ipPort();
}

// 处理连接和断开事件
void chatServer::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if(conn->connected())
    {
        // 新客户端连接加入连接集合
        _connections.insert(conn);
        LOG_INFO << conn->peerAddress().toIpPort()
            << " -> " << conn->localAddress().toIpPort()
            << " is UP , total connections : " << _connections.size();
        // 发送欢迎消息，告知当前在线人数
        string welcome = "Welcome to chat room! Current online users: "
            + std::to_string(_connections.size()) + "\n";
        _codec.send(conn,welcome);
    }
}
// 广播给其他连接
void chatServer::onEntireMessage(const muduo::net::TcpConnectionPtr &conn,
    const std::string &msg,muduo::Timestamp receiveTime)
{

}
