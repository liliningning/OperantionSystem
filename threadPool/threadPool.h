#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <pthread.h>

typedef struct threadpool_t
{
    pthread_t *threadId;

    /* 最小的线程数 */
    int minThreadSize;
    /* 最大的线程数 */
    int maxThreadSize;

} threadpool_t;

#endif
