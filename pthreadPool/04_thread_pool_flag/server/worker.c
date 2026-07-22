#include "worker.h"
#include "queue.h"
#include "thread_pool.h"
#include "send_file.h"
#include <pthread.h>

void *thread_func(void *arg)
{
    //通过pthread_create函数的第三个参数传递过来之后，在
    //线程入口函数中将其反着解出来
    thread_pool_t *pool = (thread_pool_t *)arg;

    //就应该写线程需要执行的逻辑
    while(1)
    {
        int fd = 0;
        //从队列中拿数据，因为队列是共享资源，就要上锁
        pthread_mutex_lock(&pool->lock);

        //如果队列是空的，工作线程只能睡眠
        //面试常考题：什么是虚假唤醒，如何解决虚假唤醒的问题
        while(pool->queue.size == 0 && pool->exitFlag == 0)
        {
            pthread_cond_wait(&pool->cond, &pool->lock);
        }

        if(pool->exitFlag == 1)
        {
            pthread_mutex_unlock(&pool->lock);
            pthread_exit((void* )NULL);
        }

        //只有队列不空的时候，才能从队列中取数据
        //获取文件描述符
        fd = pool->queue.head->fd;
        //出队
        deQueue(&pool->queue);

        //后续要解锁
        pthread_mutex_unlock(&pool->lock);

        //利用文件描述符将数据发给客户端
        send_file(fd);

        close(fd);
    }

    return NULL;
}
