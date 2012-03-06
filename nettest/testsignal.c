#include "../net_thread.h"
#include "../net_signaler.h"

#include <stdio.h>

void* signal;
int i = 0;

void read(void* arg)
{
  while(1)
  {
    printf("%d\n",i++);
    nthread_sleep(100);
    net_signal_send(signal);
  }
}

void write(void* arg)
{
  while(1)
  {
    net_signal_recv(signal);
    printf("%d\n",i--);
  }
}


int testsignal()
{
  int i;
  signal = net_signal_create();
  nthread_create(read,NULL);
  nthread_create(write,NULL);
  system("pause");
  return 0 ;
}