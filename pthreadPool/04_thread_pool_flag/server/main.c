#include "epoll.h"
#include "queue.h"
#include "socket.h"
#include "thread_pool.h"
#include <bits/pthreadtypes.h>
#include <my_header.h>
#include <pthread.h>
#include <signal.h>

int pipe_fd[2];

void func(int num)
{
    printf("num : %d\n", num);
    write(pipe_fd[1], "1", 1);
}

int main(int argc, char *argv[])
{
    //创建管道
    pipe(pipe_fd);

    if(fork() != 0)
    {
        //父进程
        signal(2, func);
        wait(NULL);//父进程等着子进程
        exit(0);
    }

    //让子进程脱离进程组，并且成为新的进程组的组长
    //新的进程组的id就是刚刚的子进程的id
    //因为父进程是前台进程组，而一个会话中最多只有
    //一个前台进程组，所以此时子进程对应的进程组会
    //自动变为后台进程组，后台进程组在一个会话中
    //可以有多个
    setpgid(0, 0);

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
    //监听管道的读端
    add_epoll_fd(epfd, pipe_fd[0]);

    while(1)
    {
        struct epoll_event lst[10];
        int nready = epoll_wait(epfd, lst, 10, -1);
        ERROR_CHECK(nready, -1, "epoll_wait");
        printf("nready:%d\n", nready);

        for(int idx = 0; idx < nready; ++idx)
        {
            int fd = lst[idx].data.fd;

            if(fd == pipe_fd[0])
            {
                char buf[10] = {0};
                read(fd, buf, sizeof(buf));
                /* printf("子进程的main线程收到了父进程的信号\n"); */
                //------具体的事情还没有写-------
                //具体的事情就是让子线程死

                pthread_mutex_lock(&pool.lock);
                pool.exitFlag = 1;//将退出标志位设置为1，表明子进程中的工作线程
                                  //在做完任务之后，可以退出来了
                pthread_cond_broadcast(&pool.cond);
                pthread_mutex_unlock(&pool.lock);

                for(int idx = 0; idx < pool.thread_num; ++idx)
                {
                    pthread_join(pool.thread_id_arr[idx], NULL);
                }
                pthread_exit((void *)NULL);
            }

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

