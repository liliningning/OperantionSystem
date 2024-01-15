#include <pthread.h>
#include "threadPool.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEAULET_MIN_THREADS 5
#define DEAULET_MAX_THREADS 10

#define DEAULET_QUEUE_CAPACITY 100

enum STATUS_CODE
{
    ON_SUCESS,
    PTR_NULL,
    MALLOC_ERROR,
    UNKNOWN_ERROR,

};

/* 本质是消费者 */
void *threadHander(void *arg)
{
    threadpool_t *pool = (threadpool_t *)arg;

    while (1)
    {

        pthread_mutex_lock(&(pool->mutexpool));
        while (pool->queueSize == 0)
        {
            /* 等待一个生产者条件变量 */
            pthread_cond_wait(&(pool->notEmpty), &(pool->mutexpool));
        }
        /* 任务队列有任务 */
        task_t tempTask = pool->taskQueue[pool->queuehead];
        pool->queuehead = (pool->queuehead + 1) % pool->queueCapacity;
        /* 任务数--*/
        (pool->queueSize)--;

        pthread_mutex_unlock(&(pool->mutexpool));
        /*给生产者发送信号 */
        pthread_cond_signal(&(pool->notFull));


        pthread_mutex_lock(&(pool->busymutexpool));
        /* 提升性能 创建一把只维护忙碌的线程 */
        pool->busyThreadNums++;
        pthread_mutex_unlock(&(pool->busymutexpool));

        /* 执行钩子函数 */
        tempTask.work_hander(tempTask.arg);

        pthread_mutex_lock(&(pool->busymutexpool));
        pool->busyThreadNums--;
         pthread_mutex_unlock(&(pool->busymutexpool));


    }
    pthread_exit(NULL);
}

/* 线程池初始化*/
int threadPoolInit(threadpool_t *pool, int minThreadSize, int maxThreadSize, int queueCapacity)
{
    if (pool == NULL)
    {
        return PTR_NULL;
    }

    do
    {
        /* 判断线程容量是否合法 */
        if (minThreadSize <= 0 || maxThreadSize <= 0 || minThreadSize >= maxThreadSize)
        {
            minThreadSize = DEAULET_MIN_THREADS;
            maxThreadSize = DEAULET_MAX_THREADS;
        }
        /* 更新大小 */
        pool->minThreadSize = minThreadSize;
        pool->maxThreadSize = maxThreadSize;

        /* 忙碌的线程数 */
        pool->busyThreadNums = 0;



        /* 判断合法性 任务队列的容量 */
        if (queueCapacity <= 0)
        {
            queueCapacity = DEAULET_QUEUE_CAPACITY;
        }
        /* 任务队列属性 */
        pool->queueCapacity = queueCapacity;
        pool->queuehead = 0;
        pool->queuetail = 0;
        pool->queueSize = 0;

        pool->taskQueue = (task_t *)malloc(sizeof(task_t) * pool->queueCapacity);
        if (pool->taskQueue == NULL)
        {
            perror("  pool->taskQueue malloc error ");
            break;
        }
        memset(pool->taskQueue, 0, sizeof(task_t) * pool->queueCapacity);

        /* 为id分配空间 */
        pool->threadIds = (pthread_t *)malloc(sizeof(pthread_t) * pool->maxThreadSize);
        if (pool->threadIds == NULL)
        {
            perror("malloc error");
            exit(-1);
        }
        /* 清除脏数据 */
        memset(pool->threadIds, 0, sizeof(pthread_t) * pool->maxThreadSize);

        /* 创建线程 */
        int ret = 0;
        for (int idx = 0; idx < pool->minThreadSize; idx++)
        {
            ret = pthread_create(&(pool->threadIds[idx]), NULL, threadHander, pool);
            if (ret != 0)
            {
                perror("thread  create error");
                break;
            }
        }
        /* 创建线程函数的返回值 */
        if (ret != 0)
        {
            break;
        }

        /* 存活的线程数 */
        pool->liveThreadNums = pool->minThreadSize;

        /* 线程池的锁的初始化 */
        pthread_mutex_init(&(pool->mutexpool), NULL);
        /* 只维护忙碌线程的锁的初始化 */
        pthread_mutex_init(&(pool->busymutexpool), NULL);


    if(pthread_cond_init(&(pool->notEmpty), NULL) != 0 ||  pthread_cond_init(&(pool->notFull), NULL) != 0)
       {
            perror("thread cond error");
            break;
        }

    return ON_SUCESS;
    } while (0);

    /* 回收任务队列的资源 */
    if (pool->taskQueue != NULL)
    {
        free(pool->taskQueue);
        pool->taskQueue = NULL;
    }

    /* 回收线程资源*/
    for (int idx = 0; idx < pool->minThreadSize; idx++)
    {
        if (pool->threadIds[idx] != 0)
        {
            pthread_join(pool->threadIds[idx], NULL);
        }
    }

    if (pool->threadIds != NULL)
    {
        free(pool->threadIds);
        pool->threadIds = NULL;
    }

    /* 释放锁资源 */
    pthread_mutex_destroy(&(pool->mutexpool));
    pthread_mutex_destroy(&(pool->busymutexpool));

    /* 释放条件变量资源 */
    pthread_cond_destroy(&(pool->notEmpty));
    pthread_cond_destroy(&(pool->notFull));

    return UNKNOWN_ERROR;
}


/* 线程添加任务 */
int threadPoolAddTask(threadpool_t *pool, void * (worker_hander)(void *), void *arg)
{
    if(pool == NULL)
    {
        return PTR_NULL;
    }

    pthread_mutex_lock(&(pool->mutexpool));
    /* 任务队列满 */
    while(pool->queueSize == pool->queueCapacity)
    {
        pthread_cond_wait(&(pool->notFull), &(pool->mutexpool));
    }

    /* 程序一定有位置 可以有任务  */
    /* 往队列的队尾 里面添加任务 */
    pool->taskQueue[pool->queuetail].work_hander = worker_hander;
    pool->taskQueue[pool->queuetail].arg = arg;

    /* 队尾往后移动*/
    pool->queuetail = (pool->queuetail + 1) % pool->queueCapacity;
    /* 任务数++*/
    pool->queueSize++;

    pthread_mutex_unlock(&(pool->mutexpool));
    pthread_cond_signal(&(pool->notEmpty));

    return ON_SUCESS;



}




/* 线程池的销毁 */
int threadPoolDstory(threadpool_t *pool)
{
}