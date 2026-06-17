#include <iostream>


using namespace std;
/*
 * TCP编程
 * Daytime服务器（短连接，tcp连接创建和断开是一瞬间）
 * Echo服务器  （长连接，tcp连接复用）
 * Chatroom服务器（长连接，连接之间有交互，把一个方向传过来的数据转发给其他连接）
 *
 * Echo服务器： 把客户端发送过来的字母转换成大写，再返回给客户端
 */
int tcp_listen(unsigned int port)
{
    return 0;
}

int main(int argc,char *argv[])
{
    // 绑定通配符地址,监听端口
    int listenfd = tcp_listen(9527);
    // 创建epoll实例,返回文件描述符
    // 底层是红黑树+双向链表
    int epfd = epoll_create(1);
    // 将listenfd 添加到 epoll实例中
    struct epoll_event ev;
    ev.envs = EPOLLIN;
    ev.data.fd = listenfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);

    // 时间循环
    for(;;)
    {
        // 等待事件发生，处理事件
        // 处理事件
        struct epoll_event events[MAX_EVENTS];
        int nready = epoll_wait(epfd,events,MAX_EVENTS,-1);

        // 处理事件
        for(int i = 0; i<nready;++i)
        {

        }
    }
    return 0;
}
