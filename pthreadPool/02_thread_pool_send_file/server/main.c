#include "epoll.h"
#include "queue.h"
#include "socket.h"
#include "thread_pool.h"
#include <my_header.h>

int main(int argc, char *argv[])
{
    //线程池中的线程创建出来、然后互斥锁、条件变量等
    //都要初始化（也就是初始化线程）
    thread_pool_t pool;
    init_thread_pool(&pool, 4);

    //初始化socket相关的操作
    int listen_fd = 0;
    init_socket(&listen_fd, "192.168.100.128", "12345");

    //IO多路复用
    int epfd = epoll_create(1);
    ERROR_CHECK(epfd, -1, "epoll_create");

    //监听listen_fd
    add_epoll_fd(epfd, listen_fd);

    while(1)
    {
        struct epoll_event lst[10];
        int nready = epoll_wait(epfd, lst, 10, -1);
        ERROR_CHECK(nready, -1, "epoll_wait");
        printf("nready:%d\n", nready);

        for(int idx = 0; idx < nready; ++idx)
        {
            int fd = lst[idx].data.fd;

            if(fd == listen_fd)
            {
                int conn_fd = accept(listen_fd, NULL, NULL);
                ERROR_CHECK(conn_fd, -1, "accept");

                //-----接下来的基本就是在与队列打交道-------
                pthread_mutex_lock(&pool.lock);
                //核心操作就是入队
                enQueue(&pool.queue, conn_fd);
                //只要向队列中放了文件描述符conn_fd，就可以让工作线程
                //取出文件描述符
                pthread_cond_signal(&pool.cond);
                pthread_mutex_unlock(&pool.lock);
                //---------------结束与队列打交道----------
            }
        }
    }

    return 0;
}

