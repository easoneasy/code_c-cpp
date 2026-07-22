#pragma once

#include <functional>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>
class lengthHeaderCodec
{
public:
    using MessageCallback = std::function<void(const muduo::net::TcpConnectionPtr &,
        const std::string&,muduo::Timestamp)>;
    // 构造函数，保存消息处理回调
    explicit lengthHeaderCodec(const MessageCallback &cb)
    :_messageCallback(cb)
    {}

    // 将消息打包成【长度+内容】格式发送
    void send(const muduo::net::TcpConnectionPtr &conn, const std::string &val);

    // 解码：从接收缓冲区中解析出一条完整的消息
    void onMessage(const muduo::net::TcpConnectionPtr &conn,muduo::net::Buffer *buf,muduo::Timestamp receiveTime);
private:
    MessageCallback _messageCallback;
};
