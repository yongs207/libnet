#ifndef net_rwlock_h
#define net_rwlock_h

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <pthread.h>
#endif

struct net_rwlock_ 
{
#ifdef WINDOWS_NEWER
  SRWLOCK write_lock;
#elif _WIN32
  CRITICAL_SECTION write_lock;
  CRITICAL_SECTION reader_count_lock;
  HANDLE no_reader;
  volatile int reader_count;
#else
  pthread_rwlock_t rw_mutex;
#endif
};

struct net_mutex_
{
#if _WIN32
  CRITICAL_SECTION mutex_lock;
#else
  pthread_mutex_t mutex_lock;
#endif
};

typedef struct net_rwlock_ net_rwlock_t;
typedef struct net_mutex_ net_mutex_t;

int net_mutex_init(net_mutex_t* mutex_ptr);

void net_mutex_destory(net_mutex_t* mutex_ptr);

void net_mutex_lock(net_mutex_t* mutex_ptr);

void net_mutex_unlock(net_mutex_t* mutex_ptr);

int net_mutex_trylock(net_mutex_t* mutex_ptr);

int net_rw_init(net_rwlock_t* self);

int net_rw_destory(net_rwlock_t* self);

int net_rw_lock(net_rwlock_t* self);

int net_rw_unlock(net_rwlock_t* self);

int net_rw_lock_shared(net_rwlock_t* self);

int net_rw_unlock_shared(net_rwlock_t* self);

#endif