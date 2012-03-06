#include "../net_thread.h"
#include "../net_time.h"

#include <stdio.h>
#include <stdlib.h>
typedef struct net_timer_ net_timer_t;
typedef struct timer_task_ timer_task_t;
static int call_count = 0;
void callback(void* arg)
{
  printf("%d:%s\n",call_count++,(char*)arg);
  if(call_count%5==0)
    printf("...............................\n");
  free(arg);
}

void freecallback(void* arg)
{
  printf("free-%d:%s\n",call_count++,(char*)arg);
  free(arg);
}
int timetest()
{
  net_timer_t* timer_ptr = ntimer_create();
  int i=0,j=0;
  timer_task_t* timer_tasks[5];
  timer_task_t* tmp_task;
  for(i=0;i<10;i++)
  {
    for(j=0;j<5;j++)
    {
      char* buf = malloc(10);
      itoa(i,buf,10);
      tmp_task=ntimer_expires_at(timer_ptr,3000*(i)+j*500,callback,(void*)buf);
      if(i==5)
        timer_tasks[j]=tmp_task;
    }
  }
  //cannel
  for(i=0;i<5;i++)
  {
    ntimer_cannel(timer_ptr,timer_tasks[i],freecallback);
  }
  system("pause");
  ntimer_destory(&timer_ptr,freecallback);
  system("pause");
  return 0;
}

int main()
{
  timetest();
  return 0;
}