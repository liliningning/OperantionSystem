#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <pthread.h>

typedef struct task_t
{
    /* 钩子函数 */
    void *( *work_hander)(void *arg);
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
    /*忙碌的线程数 */
    int busyThreadNums;
    /* 存活的线程数*/
    int liveThreadNums;



    /* 线程的锁*/
    pthread_mutex_t  mutexpool;

    /* */
    pthread_mutex_t  busymutexpool;


    /* 条件变量： 任务队列有东西可以消费*/
    pthread_cond_t notEmpty;

    /* 条件变量： 消费者没有东西可以拿 */
    pthread_cond_t notFull ;


} threadpool_t;

/* 线程池初始化*/
int threadPoolInit(threadpool_t *pool, int minThreadSize, int maxThreadSize, int queueCapacity);

/* 线程添加任务 */
int threadPoolAddTask(threadpool_t *pool, void * (worker_hander)(void *), void *arg);


/* 线程池的销毁 */
int threadPoolDstory(threadpool_t *pool);
#endif
