#include "net_thread.h"

#include <assert.h>
#ifdef _WIN32
#include <windows.h>
#include <process.h>


void* nthread_create( threadfunc *func, void* args )
{
  HANDLE descriptor = (HANDLE) _beginthread (func,0,args);
  return descriptor;
}

void nthread_join( void *handle )
{
  DWORD rc = WaitForSingleObject (handle, INFINITE);
  assert (rc != WAIT_FAILED);
}

void nthread_sleep( int millisecond )
{
  Sleep(millisecond);
}

#else

#include <pthread.h>

void* nthread_create( threadfunc *func, void* args )
{
  pthread_t id;
  int rc = pthread_create (&id, NULL, func, args);
  assert (rc == 0);
  return (void*)id;
}

void nthread_join( void *handle )
{
  pthread_t id = (pthread_t)handle;
  int rc = pthread_join (id, NULL);
  assert (rc == 0);
}

void nthread_sleep( int millisecond )
{
  usleep(1000*millisecond);
}
#endif