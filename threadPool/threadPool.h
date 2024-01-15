#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <pthread.h>

typedef struct threadpool_t
{
    pthread_t * threadIds;

    /* 最小的线程数 */
    int minThreadSize;
    /* 最大的线程数 */
    int maxThreadSize;

} threadpool_t;

/* 线程池初始化*/
int threadPoolInit(threadpool_t * pool, int minThreadSize, int maxThreadSize);

/* 线程池的销毁 */
int threadPoolDstory(threadpool_t * pool);
#endif
