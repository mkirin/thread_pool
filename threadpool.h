
/***************************************************
 * filename:thread_pool.h
 * author:mkirin
 * e-mail:lingfengtengfei@163.com
 * description: the thread pool's header
***************************************************/

#ifndef _THREADPOOL_HEAD_
#define _THREADPOOL_HEAD_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>
#include "readConfig.h"

/**********************宏定义区********************/

/* 配置文件名  */
#define CONFIGFILENAME "thread_pool_config.conf"

/* 线程池最小线程数 */
#define MIN_THREAD_NUM "MIN_THREAD_NUM"

/* 线程池最大线程数 */
#define MAX_THREAD_NUM "MAX_THREAD_NUM"

/* 线程池默认线程数 */
#define DEF_THREAD_NUM "DEF_THREAD_NUM"

/* 管理线程动态调节时间间隔（s） */
#define MANAGE_ADJUST_INTERVAL "MANAGE_ADJUST_INTERVAL"

/* 线程数与工作峰值比例 */
#define THREAD_WORKER_HIGH_RATIO "THREAD_WORKER_HIGH_RATIO"

/* 任务与线程数低谷比例 */
#define THREAD_WORKER_LOW_RATIO "THREAD_WORKER_LOW_RATIO"





/************************结构体声明区*************************/

/*
 *线程池里所有运行和等待的任务都是 一个thread_worker
 *由于所有任务都在链表中，所以是一个链表结构
 */
typedef struct _worker{
	void *(*process)(void *arg);   	/* 工作的处理函数指针 */
	void *arg;					   	/* 处理函数的参数  */
	struct _worker *next;		  	/* 下一个工作  */
}thread_worker;

/*  线程队列节点结构  */
typedef struct _thread_queue_node{
	pthread_t 	thread_id;
	struct _thread_queue_node *next;
}thread_queue_node,*p_thread_queue_node;

/* 线程池结构  */
typedef struct {
	int					shutdown;			/* 是否销毁线程池  */
	pthread_mutex_t 	queue_lock;			/* 线程锁  */
	pthread_mutex_t 	remove_queue_lock;			/* 线程锁  */
	pthread_cond_t		queue_ready;		/* 通知等待队列有新任务条件变量  */
	thread_queue_node 	*thread_queue;		/* 线程池的线程队列 */
	thread_queue_node 	*thread_idle_queue;
	int 				idle_queue_num;
	int 				max_thread_num; 	/* 线程池中允许开启的最大线程数  */
	int 				cur_queue_size;		/* 当前等待队列的任务数目  */
	thread_worker 		*queue_head;		/* 线程池所有等待任务  */
}thread_pool;

/* 线程取消  */
typedef struct {
	int 				is_revoke; 			/*是否需要撤销线程*/
	int 				revoke_count;		/* 已经撤销的线程数  */
	int 				revoke_num;			/* 需要撤销的总数  */
	pthread_mutex_t 	revoke_mutex; 		/* 撤销线程加锁  */
	thread_queue_node 	*thread_revoke_queue;	/* 线程撤销队列 */
}thread_revoke;

/**************************功能函数声明区************************/

/** 向线程池中添加任务  **/
int pool_add_worker(void *(*process)(void *arg), void *arg);

/** 线程池中的线程  **/
void *thread_routine(void *arg);

/** 初始化线程池  **/
void pool_init(int max_thread_num);

/**  销毁线程池  **/
int pool_destroy();

/** 向线程池中追加线程 **/
void pool_add_thread(int thread_num);

/** 向线程队列中追加线程 **/
int thread_queue_add_node(p_thread_queue_node *thread_queue, pthread_t thread_id,int * count);

/**  撤销线程  **/
void pool_revoke_thread(int thread_num);

/**  从线程队列删除线程  **/
int thread_queue_remove_node(p_thread_queue_node *thread_queue, pthread_t thread_id,int *count);

/** 获取配置文件中某一项的值  **/
int get_config_value(char * item);

/*****************全局变量声明区******************/
/* 线程池  */
extern thread_pool 		*g_pool;

/* 线程取消  */
extern thread_revoke 	*g_thread_revoke;

/* 线程池最大线程数  */
extern int 				g_max_thread_num;

/* 线程池最小线程数  */
extern int 				g_min_thread_num;

/* 默认线程池线程数  */
extern int 				g_def_thread_num;	

/* 管理线程调整时间间隔  */
extern int 				g_manage_adjust_interval;

/* 线程任务峰值比率：衡量负载 */
extern int 				g_thread_worker_high_ratio;	

/* 线程任务低谷比率：衡量负载 */
extern int 				g_thread_worker_low_ratio;	

#endif
