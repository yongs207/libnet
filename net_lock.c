#include "net_lock.h"
#ifdef _WIN32
#include "windowdef.h"
#else
#include <pthread.h>
#endif

#include <stdlib.h>
#include <assert.h>

#ifdef _WIN32

int net_mutex_init(net_mutex_t* mutex_ptr)
{
  InitializeCriticalSection(&mutex_ptr->mutex_lock);
  return 1;
}

void net_mutex_destory(net_mutex_t* mutex_ptr)
{
  DeleteCriticalSection(&mutex_ptr->mutex_lock);
}

void net_mutex_lock(net_mutex_t* mutex_ptr)
{
  EnterCriticalSection(&mutex_ptr->mutex_lock);
}

void net_mutex_unlock(net_mutex_t* mutex_ptr)
{
  LeaveCriticalSection(&mutex_ptr->mutex_lock);
}

int net_mutex_trylock(net_mutex_t* mutex_ptr)
{
  return TryEnterCriticalSection(&mutex_ptr->mutex_lock);
}

#else
int net_mutex_init(net_mutex_t* mutex_ptr)
{
  return pthread_mutex_init(&mutex_ptr->mutex_lock,NULL);
}

void net_mutex_destory(net_mutex_t* mutex_ptr)
{
  pthread_mutex_destroy(&mutex_ptr->mutex_lock);
}

void net_mutex_lock(net_mutex_t* mutex_ptr)
{
  pthread_mutex_lock(&mutex_ptr->mutex_lock);
}

void net_mutex_unlock(net_mutex_t* mutex_ptr)
{
  pthread_mutex_unlock(&mutex_ptr->mutex_lock)
}

int net_mutex_trylock(net_mutex_t* mutex_ptr)
{
  return pthread_mutex_trylock(&mutex_ptr->mutex_lock);
}
#endif




#ifdef WINDOWS_NEWER


int net_rw_init(net_rwlock_t* rwlock)
{
  InitializeSRWLock(&(rwlock->write_lock));
  return 1;
}

int net_rw_destory(net_rwlock_t* self)
{
  return 1;
}

int net_rw_lock(net_rwlock_t* self)
{
  AcquireSRWLockExclusive(&self->write_lock);
  return 1;
}

int net_rw_unlock(net_rwlock_t* self)
{
  ReleaseSRWLockExclusive(&self->write_lock);
  return 1;  
}

int net_rw_lock_shared(net_rwlock_t* self)
{
  AcquireSRWLockShared(&self->write_lock);
  return 1;
}

int net_rw_unlock_shared(net_rwlock_t* self)
{
  ReleaseSRWLockShared(&self->write_lock);
  return 1;
}
#elif _WIN32

int net_rw_init(net_rwlock_t* rwlock)
{
  InitializeCriticalSection(&(rwlock->write_lock));
  InitializeCriticalSection(&(rwlock->reader_count_lock));
  rwlock->no_reader = CreateEvent (NULL, TRUE, TRUE, NULL);
  rwlock->reader_count = 0;
  return 1;
}

int net_rw_destory(net_rwlock_t* self)
{
  CloseHandle(self->no_reader);
  DeleteCriticalSection(&(self)->write_lock);
  DeleteCriticalSection(&(self)->reader_count_lock);
  return 1;
}

int net_rw_lock(net_rwlock_t* self)
{
  EnterCriticalSection(&self->write_lock);
  if (self->reader_count > 0) {
    WaitForSingleObject(self->no_reader, INFINITE);
  }
  return 1;
}

int net_rw_unlock(net_rwlock_t* self)
{
  LeaveCriticalSection(&self->write_lock);
  return 1;
}

int net_rw_lock_shared(net_rwlock_t* self)
{
  EnterCriticalSection(&self->write_lock);
  EnterCriticalSection(&self->reader_count_lock);
  if (++self->reader_count == 1) {
    ResetEvent(self->no_reader);
  }
  LeaveCriticalSection(&self->reader_count_lock);
  LeaveCriticalSection(&self->write_lock);
  return 1;
}

int net_rw_unlock_shared(net_rwlock_t* self)
{
  EnterCriticalSection(&self->reader_count_lock);
  assert (self->reader_count > 0);
  if (--self->reader_count == 0) {
    SetEvent(self->no_reader);
  }
  LeaveCriticalSection(&self->reader_count_lock);
  return 1;
}

#else
int net_rw_init(net_rwlock_t* rwlock)
{
  return pthread_rwlock_init(&(rwlock->rw_mutex),NULL);
}

int net_rw_destory(net_rwlock_t* rwlock)
{
  return pthread_rwlock_destroy(&(rwlock->rw_mutex));
}

int net_rw_lock(net_rwlock_t* rwlock)
{
  return pthread_rwlock_wrlock(&(rwlock->rw_mutex));
}

int net_rw_unlock(net_rwlock_t* rwlock)
{
  return pthread_rwlock_unlock(&(rwlock->rw_mutex));
}

int net_rw_lock_shared(net_rwlock_t* rwlock)
{
  return pthread_rwlock_rdlock(&(rwlock->rw_mutex));
}

int net_rw_unlock_shared(net_rwlock_t* rwlock)
{
  return pthread_rwlock_unlock(&(rwlock->rw_mutex));
}

#endif