#include <cstdint>
#include <endian.h>
#include <iostream>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
#include <string>

using namespace std;
using namespace muduo;
using namespace muduo::net;

// 同一管理所有在线客户端的连接池（使用只能指针保证多线程安全）
std::set<TcpConnectionPtr> g_connections;

void onEntireMessage(const TcpConnectionPtr &conn,const string &msg,Timestamp receiveTime)
{
    // 格式化消息，加上当前收到消息的时间戳前缀
    string formattedMsg = receiveTime.toFormattedString() + " : " + msg + "\n";
    LOG_INFO << "Broadcasting message : " << msg;
    // 环形广播：遍历在线的所有人
    for(auto it = g_connections.begin();it!=g_connections.end();++it)
    {
        if(*it != conn)
        {
            // 编码并发送
            Buffer writeBuf;
            // 计算长度并转为网络字节序
            uint32_t length_be = htobe32(formattedMsg.size());
            // 写上4字节头
            writeBuf.append(&length_be,sizeof(length_be));
            // 放入消息体
            writeBuf.append(formattedMsg.data(),formattedMsg.size());
            (*it)->send(&writeBuf);
        }
    }
}

// 解决粘包
void onMessage(const TcpConnectionPtr &conn,Buffer *buf,Timestamp receiveTime)
{
    // 只要输入缓冲区的可读字节数 >= 4字节，至少可以读取长度头
    while(buf->readableBytes() >= 4)
    {
        // 窥探前4字节，不着急从缓冲区取出
        const void *data = buf->peek();
        // 将4字节从网络字节序->主机字节序，得到后面消息体的真正长度
        uint32_t length = be32toh(*static_cast<const uint32_t*>(data));
        // 安全性检查
        // 如果消息体特别长或不合法可以断开，避免恶意攻击
        if(length > 65536)
        {
            LOG_ERROR << "Invalid length" << length;
            conn->shutdown();
            break;
        }
        if(buf->readableBytes() - 4 < length)
        {
            // 如果不够就跳出循环，继续epoll_wait，等待下一次的数据到达
            break;
        }
        // 一句话完整地到齐了
        // 确认把这4字节的长度头从缓冲区删掉
        buf->retrieve(4);
        // 提取真正的明文消息体
        string msg(buf->peek(),length);
        // 将明文消息体从缓冲区清空
        buf->retrieve(length);
        // 解包完毕，接力传给业务层进行广播
        onEntireMessage(conn,msg,receiveTime);
    }
}

void onConnection(const TcpConnectionPtr &conn)
{
    // 客户端连接成功
    if(conn->connected())
    {
        // 塞入全局在线集合
        g_connections.insert(conn);
        LOG_INFO << conn->peerAddress().toIpPort() << "is UP,total_suer :" <<g_connections.size();
        // 准备一句欢迎词，告知当前聊天室的在线人数
        string welcome = "Welcome to chat room! Current online users: "
            + std::to_string(g_connections.size()) +"\n";
        // 包装成4字节+消息体的格式单独发送给
        Buffer writeBuf;
        uint32_t length_be = htobe32(welcome.size());
        writeBuf.append(&length_be,sizeof(length_be));
        writeBuf.append(welcome.data(),welcome.size());
        conn->send(&writeBuf);
    }else{
        // 客户端主动断开
        // 从在线集合中抹去
        g_connections.erase(conn);
        LOG_INFO << conn->peerAddress().toIpPort() << "is DOWN. Remaining users: "
        << g_connections.size();
    }
}
int main(int argc,char *argv[])
{
    if(argc < 2)
    {
        cout << "Usage : " << argv[0] << "<port>" << endl;
        return 1;
    }

    // 将日志输出级别设置为INFO
    Logger::setLogLevel(muduo::Logger::INFO);

    // 创建主线程
    EventLoop loop;
    // 监听ip+port
    uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
    InetAddress listenAddr(port);
    // 初始化Server
    TcpServer server(&loop,listenAddr,"charServer");
    // 设置回调函数
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    // 开启4个线程池
    server.setThreadNum(4);
    // 启动服务器
    server.start();
    LOG_INFO << "charServer started on port " << port;
    // 阻塞
    loop.loop();
    return 0;
}
