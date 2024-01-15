#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <pthread.h>

typedef struct task_t
{
    /* 钩子函数 */
    void *(work_hander)(void *arg);
    /* 参数 */
    void *arg;
} task_t;

typedef struct threadpool_t
{
    /* 任务队列 */
    task_t *taskQueue;
    /* 容量 */
    int queueCapacity;
    /* 队列里面的任务数 */
    int queueSize;
    /* 队头 */
    int queuehead;
    /* 队尾 */
    int queuetail;



    pthread_t *threadIds;
    /* 最小的线程数 */
    int minThreadSize;
    /* 最大的线程数 */
    int maxThreadSize;

} threadpool_t;

/* 线程池初始化*/
int threadPoolInit(threadpool_t *pool, int minThreadSize, int maxThreadSize);

/* 线程池的销毁 */
int threadPoolDstory(threadpool_t *pool);
#endif
