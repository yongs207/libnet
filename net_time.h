#ifndef net_time_h
#define net_time_h

#include <stdint.h>

typedef struct net_timer_ net_timer_t;
typedef struct timer_task_ timer_task_t;

typedef void (*timer_callback)(void* arg);
typedef void (*free_callback)(void* arg);

net_timer_t* ntimer_create();

//添加一个定时器，在定时器的回调只会被执行一次
//参数：arg 不要调用者在回调函数中释放内存
//返回值：timer_task_t,如果返回为NULL表示添加定时器失败
timer_task_t* ntimer_expires_at(net_timer_t* self,int millisecond,timer_callback callback,void* arg);

//取消task_id对应的任务
//如果设定的定时器已被执行则返回-1，否则返回0
int ntimer_cannel(net_timer_t* self,timer_task_t*  task_ptr,free_callback callback);


void ntimer_destory(net_timer_t** self_ptr,free_callback callback);

int64_t ntime();

#endif