#ifndef net_thread_h
#define net_thread_h


typedef void (threadfunc)(void*);

void* nthread_create (threadfunc *func, void* args);

void nthread_join (void *handle);

void nthread_sleep (int millisecond);


#endif
