#include "epoll.h"
#include <my_header.h>
#include <sys/epoll.h>

//将fd放在红黑树上进行监听
void add_epoll_fd(int epfd, int fd)
{
    struct epoll_event evt;
    evt.events = EPOLLIN;//读事件
    evt.data.fd = fd;//监听的文件描述符

    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &evt);
    ERROR_CHECK(ret, -1, "epoll_ctl add");
}

//将fd从红黑树上取消监听
void del_epoll_fd(int epfd, int fd)
{
    struct epoll_event evt;
    evt.events = EPOLLIN;//读事件
    evt.data.fd = fd;//监听的文件描述符

    int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &evt);
    ERROR_CHECK(ret, -1, "epoll_ctl del");
}
