#include "thread_pool.h"
#include "queue.h"
#include "worker.h"
#include <my_header.h>

//初始化线程池中的数据
void init_thread_pool(thread_pool_t *pool, int num)
{
    //退出的标志位，默认不退出，那就是0，如果退出就是1
    pool->exitFlag = 0;

    //初始化子线程的数目
    pool->thread_num = num;

    //互斥锁的初始化
    pthread_mutex_init(&pool->lock, NULL);

    //条件变量的初始化
    pthread_cond_init(&pool->cond, NULL);

    //初始化队列
    memset(&pool->queue,0, sizeof(queue_t));

    //给线程id对应的指针分配空间
    pool->thread_id_arr = (pthread_t *)malloc(num * sizeof(pthread_t));

    for(int idx = 0; idx < num; ++idx)
    {
        //一定要注意：需要将pool变量传入到thread_func中，因为会用到互斥锁、条件变量等
        int ret = pthread_create(&pool->thread_id_arr[idx], NULL, thread_func, (void *)pool);
        ERROR_CHECK(ret, -1, "pthread_create");
    }
}
