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
    // 打印日志
    LOG_INFO << "echoServer" << conn->peerAddress().toIpPort()
        << conn->localAddress().toIpPort() << " is "
        << (conn->connected() ? " UP " : " DOWN ");
}

void onMessage(const TcpConnectionPtr &conn,Buffer *buf,muduo::Timestamp time)
{
    // 当前连接被分配到哪个子线程，这段代码就会在哪个子线程执行
    // 从连接的输入缓冲区一次性读取所有数据
    string msg(buf->retrieveAllAsString());
    // 打印日志，记录收到数据的线程环境、大小、时间
    LOG_INFO << conn->name() << " echo " << msg.size() << " byte, "
        << " data received at " << time.toString();

    // 将数据原样发回给客户端
    conn->send(msg);
}

int main(int argc,char *argv[])
{
    // 创建主线程,专门用来监听新的连接
    muduo::net::EventLoop loop;
    // 确定要监听的ip+port
    muduo::net::InetAddress listenAddr(2007);
    // 创建tcp服务器对象
    muduo::net::TcpServer server(&loop,listenAddr,"echoServer");
    // 注册连接事件回调函数、消息事件回调函数
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    // 创建4个子线程
    server.setThreadNum(4);

    // 启动服务器
    server.start();

    loop.loop();

    return 0;
}
