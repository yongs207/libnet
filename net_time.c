#include "net_time.h"

#include "net_thread.h"
#include "net_socketpair.h"
#include "net_pqueue.h"
#include "net_lock.h"

#include <stdlib.h>
#include <string.h>



#define NET_TIME_MAX_COUNT 1024*2

struct timer_task_ 
{
  int alive;
  int64_t expires_time;//超时的时刻
  int pos;// for pqueue
  timer_callback callback;
  void* arg;
  struct timer_task_* next; 
} ;

struct net_timer_ 
{
  pqueue_t * time_queue;
  net_mutex_t queue_lock;
  timer_task_t task_array[NET_TIME_MAX_COUNT];
  timer_task_t* freelist;
  int64_t timeout;//超时时间间隔
  volatile int terminate;
  char signal;//用于信号发送时使用
  void* thread_handle;
  fd_t pair[2];
};

static int s_cmp_expires_time(double next, double curr)
{
  return (next > curr);
}


static double s_get_expires_time(void *a)
{
  return (double) ((timer_task_t *) a)->expires_time;
}


static void s_set_expires_time(void *a, double expires_time)
{
  ((timer_task_t *) a)->expires_time = expires_time;
}


static size_t s_get_pos(void *a)
{
  return ((timer_task_t *) a)->pos;
}


static void s_set_pos(void *a, size_t pos)
{
  ((timer_task_t *) a)->pos = pos;
}

static timer_task_t* s_get_free_task(net_timer_t* self)
{
  if (self->freelist)
  {
    timer_task_t* res = self->freelist->next;
    self->freelist->next = res->next;
    return res;
  }
  return NULL;
}

static void s_set_free_task(net_timer_t* self,timer_task_t* task)
{
  task->next = self->freelist->next;
  self->freelist->next = task;
  task->alive = 0;
}

static void s_work_loop(void* arg)
{
  net_timer_t* timer_ptr = (net_timer_t *)arg;
  unsigned long timeout_ ;
  fd_set fds;
  int rc;
  struct timeval timeout;
  FD_ZERO (&fds);
  
  while (!timer_ptr->terminate)
  {
    FD_SET (timer_ptr->pair[1], &fds);
    timeout_ = timer_ptr->timeout;
    if (timeout_ >= 0) {
      timeout.tv_sec = timeout_ / 1000;
      timeout.tv_usec = timeout_ % 1000 * 1000;
    }
#ifdef _WIN32
    rc = select (0, &fds, NULL, NULL,
      timeout_ >= 0 ? &timeout : NULL);
    wsa_assert (rc != SOCKET_ERROR);
#else
    rc = select (pair_ptr[1] + 1, &fds, NULL, NULL,
      timeout_ >= 0 ? &timeout : NULL);
    if (unlikely (rc < 0)) {
      net_assert (errno == EINTR);
      return -1;
    }
#endif
    if(rc==0&&timer_ptr->time_queue>0)
    {
      timer_task_t* task_item ;
      do 
      {
        net_mutex_lock(&timer_ptr->queue_lock);
        task_item = (timer_task_t*)pqueue_pop(timer_ptr->time_queue);
        task_item->callback(task_item->arg);
        task_item->alive = 0;
        s_set_free_task(timer_ptr,task_item);

        if(pqueue_size(timer_ptr->time_queue)==0)
        {
          timer_ptr->timeout=-1;
          net_mutex_unlock(&timer_ptr->queue_lock);
          break;
        }

        task_item = (timer_task_t*)pqueue_peek(timer_ptr->time_queue);
        timer_ptr->timeout = task_item->expires_time -ntime();
        net_mutex_unlock(&timer_ptr->queue_lock);
      } while (timer_ptr->timeout<=0);
    }
    if(rc==1)
    {
      //激活超时，调整超时时间
      int nbytes = recv (timer_ptr->pair[1],  &timer_ptr->signal, 1, 0);
      net_assert (nbytes == 1);
    }
  }
}

net_timer_t* ntimer_create()
{
  int i;
  net_timer_t* self = (net_timer_t*)malloc(sizeof(net_timer_t));
  memset(self,0,sizeof(*self));
  self->time_queue=pqueue_init(NET_TIME_MAX_COUNT,s_cmp_expires_time,s_get_expires_time,s_set_expires_time,s_get_pos,s_set_pos);
  net_mutex_init(&self->queue_lock);
  for (i=0;i<NET_TIME_MAX_COUNT-1;i++)
  {
    self->task_array[i].alive =0;
    self->task_array[i].next =&self->task_array[i+1];
  }
  self->task_array[NET_TIME_MAX_COUNT-1].alive =0;
  self->task_array[NET_TIME_MAX_COUNT-1].next =NULL;
  self->freelist = &self->task_array[0];
  self->timeout = -1;
  self->terminate = 0;
  nsocketpair(self->pair);
  self->thread_handle = nthread_create(s_work_loop,self);
  return self;
}

static void s_update_time(net_timer_t* self,timer_task_t* task,int millisecond)
{
  int nbytes;
  pqueue_insert(self->time_queue,task);
  nbytes = send (self->pair[0], &self->signal, 1, 0);
  net_assert (nbytes == 1);
  self->timeout = millisecond;
}


//添加一个定时器，在定时器的回调只会被执行一次
//参数：arg 不要调用者在回调函数中释放内存
//返回值：task_id,如果返回为-1表示添加定时器失败
timer_task_t* ntimer_expires_at(net_timer_t* self,int millisecond,timer_callback callback,void* arg)
{
  pqueue_t * time_queue = self->time_queue;
  int64_t now_time = ntime();
  timer_task_t* curr_task = s_get_free_task(self);

  if(curr_task==NULL) return NULL;
  curr_task->alive = 1;
  curr_task->arg = arg;
  curr_task->callback = callback;
  curr_task->expires_time = now_time+millisecond;
  //获得当前的时间，计算最小时间间隔
  net_mutex_lock(&self->queue_lock);
  if (pqueue_size(time_queue))
  {
    timer_task_t* task_item = (timer_task_t*)pqueue_peek(time_queue);
    if(task_item->expires_time>curr_task->expires_time)//最小时间间隔更新，发送事件修改超时间隔
      s_update_time(self,curr_task,millisecond);
    else
      pqueue_insert(time_queue,curr_task);
  }
  else
    s_update_time(self,curr_task,millisecond);
  net_mutex_unlock(&self->queue_lock);
  return curr_task;
}

//取消task_id对应的任务
//如果设定的定时器已被执行则返回-1，否则返回0
int  ntimer_cannel(net_timer_t* self,timer_task_t* task_ptr,free_callback callback)
{
  int nbytes;
  if(task_ptr->alive==0) return -1;
  net_mutex_lock(&self->queue_lock);
  if(pqueue_peek(self->time_queue)==task_ptr)
  {
    pqueue_remove(self->time_queue,task_ptr);
    self->timeout = task_ptr->expires_time-ntime();
    self->timeout = self->timeout>0?self->timeout:1;
    nbytes = send (self->pair[0], &self->signal, 1, 0);
    net_assert (nbytes == 1);
  }else
    pqueue_remove(self->time_queue,task_ptr);
  s_set_free_task(self,task_ptr);
  callback(task_ptr->arg);
  net_mutex_unlock(&self->queue_lock);
  return 0;
}


void ntimer_destory(net_timer_t** self_ptr,free_callback callback)
{
  int nbytes,i,size;
  net_timer_t* timer_ptr =* self_ptr;
  timer_ptr->terminate=1;
  nbytes = send (timer_ptr->pair[0], &timer_ptr->signal, 1, 0);
  net_assert (nbytes == 1);
  nthread_join(timer_ptr->thread_handle);
  if(callback!=NULL)
  {
    size = pqueue_size(timer_ptr->time_queue);
    for (i=0;i<size;i++)
    {
      timer_task_t* tmp=(timer_task_t*)pqueue_pop(timer_ptr->time_queue);
      callback(tmp->arg);
    }
  }

  pqueue_free(timer_ptr->time_queue);
  net_mutex_destory(&timer_ptr->queue_lock);
  free(timer_ptr);
  * self_ptr=NULL;
}

#ifdef _WIN32
static int64_t s_filetime_to_msec (const FILETIME *ft)
{
  return (int64_t) (*((int64_t *) ft) / 10000);
}

#endif

int64_t ntime()
{
#if defined (__linux)
  struct timeval tv;
  gettimeofday (&tv, NULL);
  return (int64_t) (tv.tv_sec * 1000 + tv.tv_usec / 1000);
#elif (defined (_WIN32))
  FILETIME ft;
  GetSystemTimeAsFileTime (&ft);
  return s_filetime_to_msec (&ft);
#endif
}