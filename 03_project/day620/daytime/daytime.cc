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
using namespace std::placeholders;

class daytimeServer
{
    public:
        // 构造函数
        // 创建TcpServer对象
        daytimeServer(muduo::net::EventLoop *loop,
            const muduo::net::InetAddress &listenAddr)
        :_server(loop, listenAddr, "daytimeServer")
        {
            _server.setConnectionCallback(std::bind(&daytimeServer::onConnection,this,_1));
            _server.setMessageCallback(std::bind(&daytimeServer::onMessage,this,_1,_2,_3));
        }
        void start()
        {
            _server.start();
        }

        ~daytimeServer() {}

    private:
        void onConnection(const muduo::net::TcpConnectionPtr &conn)
        {
            // 打印日志：客户端地址 -> 服务器地址，连接状态
            LOG_INFO << "DaytimeServer - " << conn->peerAddress().toIpPort() << " -> "
            << conn->localAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");

            if (conn->connected()) {
                // 发送当前时间字符串
                conn->send(Timestamp::now().toFormattedString() + "\n");
                // 主动断开连接（符合 Daytime 协议规范）
                conn->forceClose();
            }
        }
        void onMessage(const muduo::net::TcpConnectionPtr &conn,
            muduo::net::Buffer *buf,muduo::Timestamp time)
        {
            // 无论客户端发来什么，全部从接收缓冲区中取出来并丢弃
            string msg(buf->retrieveAllAsString());

            LOG_INFO << conn->name() << " discards " << msg.size()
            << " bytes received at " << time.toString();
        }
        muduo::net::TcpServer _server;
};
