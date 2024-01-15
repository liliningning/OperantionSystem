#include <pthread.h>
#include "threadPool.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEAULET_MIN_THREADS 5
#define DEAULET_MAX_THREADS 10
enum STATUS_CODE
{
    ON_SUCESS,
    PTR_NULL,
    MALLOC_ERROR,
    UNKONE_ERROR,

};

void * threadHander(void *arg)
{

}

/* 线程池初始化*/
int threadPoolInit(threadpool_t *pool, int minThreadSize, int maxThreadSize)
{
    if (pool == NULL)
    {
        return PTR_NULL;
    }

    do
    {
        /* 判断容量是否合法 */
        if (minThreadSize < 0 || maxThreadSize < 0 || minThreadSize >= maxThreadSize)
        {
            minThreadSize = DEAULET_MIN_THREADS;
            maxThreadSize = DEAULET_MAX_THREADS;
        }
        /* 更新大小 */
        pool->minThreadSize = minThreadSize;
        pool->maxThreadSize = maxThreadSize;

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
            ret = pthread_create(&(pool->threadIds[idx]), NULL, threadHander, NULL);
            if (ret != 0)
            {
                perror("thread  create error");
                break;
            }
        }

        return ON_SUCESS;
    } while (0);

    /* 回收线程资源*/
    for (int idx = 0; idx < pool->minThreadSize; idx++)
    {
        if (pool->threadIds[idx] != 0)
        {
            pthread_join(pool->threadIds[idx],NULL);
        }
    }

    if (pool->threadIds != NULL)
    {
        free(pool->threadIds);
        pool->threadIds = NULL;
    }

    return  UNKONE_ERROR;
}

/* 线程池的销毁 */
int threadPoolDstory(threadpool_t *pool)
{
}