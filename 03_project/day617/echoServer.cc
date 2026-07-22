#include <arpa/inet.h>
#include <cstdint>
#include <cstdio>
#include <dirent.h>
#include <iostream>
#include <my_header.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;
/*
 * TCP编程
 * Daytime服务器（短连接，tcp连接创建和断开是一瞬间）
 * Echo服务器  （长连接，tcp连接复用）
 * Chatroom服务器（长连接，连接之间有交互，把一个方向传过来的数据转发给其他连接）
 *
 * Echo服务器： 把客户端发送过来的字母转换成大写，再返回给客户端
 */

int tcp_listen(uint16_t port)
{

}

int main(int argc,char *argv[])
{
    // 绑定通配符地址，监听8888
    int listenfd = tcp_listen(8888);
    // 创建epoll实例（红黑树、双向链表）
    int epfd = epoll_create(1);
    if(epfd == -1)
    {
        perror("epoll_create()");
        exit(-1);
    }
    // 将listenfd添加到epoll实例中监听
    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = listenfd;
    int err = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &evt);
    if(err)
    {
        perror("Add listenfd to epoll instance");
        exit(-1);
    }

    // 事件循环
    for(;;)
    {
        // 存放就绪事件
        struct epoll_event events[MAXNAMLEN];
        // -1 一直等待，直到有事件就绪或发生错误
        int nready = epoll_wait(epfd, events, MAXNAMLEN, -1);
        if(nready < 0)
        {
            perror("epoll_wait()");
            break;
        }
        // 遍历所有已就绪的事件
        for(int i = 0; i < nready ; ++i)
        {
            if((events[i].events &EPOLLIN) == 0)
            {
                continue;
            }
            // listenfd 就绪：有新连接
            if(events[i].data.fd == listenfd)
            {
                // 用于接收客户端地址
                struct sockaddr_in cliaddr;
                socklen_t length = sizeof(cliaddr);
                int connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &length);
                if(connfd == -1)
                {
                    perror("accept()");
                    continue;
                }
                // 将connfd添加到epoll实例中
                evt.events = EPOLLIN;
                evt.data.fd = connfd;
                epoll_ctl(epfd, EPOLLIN, connfd, &evt);
                // 打印日志信息
                char ipstr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &cliaddr.sin_addr, ipstr, INET_ADDRSTRLEN);
                uint16_t port = ntohs(cliaddr.sin_port);
                printf("新连接建立，客户端地址为：%s:%hu\n",ipstr,port);
            }else{
                char buf[4096];
                int connfd = events[i].data.fd;
                int nbytes = recv(connfd, buf, sizeof(buf), 0);
                if(nbytes < 0)
                {
                    perror("read()");
                    epoll_ctl(epfd,EPOLL_CTL_DEL,connfd,NULL);
                }
            }
        }
    }


    return 0;
}
