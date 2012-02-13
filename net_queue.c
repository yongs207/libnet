#include "net_queue.h"
#include "net_lock.h"
#include "net_signaler.h"

#include <stdlib.h>

#define N 256

typedef struct chunk_
{
  void* values [N];
  struct chunk_ *prev;
  struct chunk_ *next;
}chunk_t;

typedef struct queue_
{
  //  Back position may point to invalid memory if the queue is empty,
  //  while begin & end positions are always valid. Begin position is
  //  accessed exclusively be queue reader (front/pop), while back and
  //  end positions are accessed exclusively by queue writer (back/push).
  chunk_t *begin_chunk;
  int begin_pos;
  chunk_t *back_chunk;
  int back_pos;
  chunk_t *end_chunk;
  int end_pos;
  int size_;
  //  People are likely to produce and consume at similar rates.  In
  //  this scenario holding onto the most recently freed chunk saves
  //  us from having to call malloc/free.
  //atomic_ptr_t<chunk_t> spare_chunk;
  chunk_t * spare_chunk;
  net_mutex_t mutex;
  void* signal;
  int sync;
};

queue_t* nqueue_create(int sync)
{
  queue_t* queue = (queue_t*)malloc(sizeof(queue_t));
  queue->begin_chunk = (chunk_t*) malloc (sizeof (chunk_t));
  queue->begin_pos = 0;
  queue->back_chunk = NULL;
  queue->back_pos = 0;
  queue->end_chunk = queue->begin_chunk;
  queue->end_pos = 0;
  queue->size_=0;
  queue->spare_chunk=NULL;
 
  queue->sync = sync;
  if(sync)
  {
    net_mutex_init(&queue->mutex);
    queue->signal = net_signal_create();
  }
  return queue;
}

void nqueue_release(queue_t* queue)
{
  while (1) {
    chunk_t *o;
    if (queue->begin_chunk == queue->end_chunk) {
      free (queue->begin_chunk);
      break;
    } 
    o = queue->begin_chunk;
    queue->begin_chunk = queue->begin_chunk->next;
    free (o);
  }
  free(queue);
  queue=NULL;
}



void* nqueue_pop(queue_t* queue)
{
  chunk_t *cs;
  void* item = queue->begin_chunk->values[queue->begin_pos];
  if (++ queue->begin_pos == N) {
    chunk_t *o = queue->begin_chunk;
    queue->begin_chunk = queue->begin_chunk->next;
    queue->begin_chunk->prev = NULL;
    queue->begin_pos = 0;
    if (o)
      free (o);
  }
  --queue->size_;
  return item;
}


void nqueue_push(queue_t* queue,void* item)
{
  chunk_t *sc;
  queue->back_chunk = queue->end_chunk;
  queue->back_pos = queue->end_pos;

  if(++queue->end_pos == N){//添加一个新的chunk
    queue->end_chunk->next = (chunk_t*) malloc (sizeof (chunk_t));
    queue->end_chunk->next->prev = queue->end_chunk;
    queue->end_chunk = queue->end_chunk->next;
    queue->end_pos = 0;
  }
  queue->back_chunk->values [queue->back_pos] = item;
  ++queue->size_;
}

int nqueue_size(queue_t* queue)
{
  return queue->size_;
}

void* nqueue_sync_pop(queue_t* queue)
{
  void* item;
  while(queue->size_==0)
    net_signal_recv(queue->signal);
  net_mutex_lock(&queue->mutex);
  item = nqueue_pop(queue);
  net_mutex_unlock(&queue->mutex);
  return item;
}

void nqueue_sync_push(queue_t* queue,void* item)
{
  net_mutex_lock(&queue->mutex);
  nqueue_push(queue,item);
  if(queue->size_==1)
    net_signal_send(queue->signal);
  net_mutex_unlock(&queue->mutex);
}

void nqueue_sync_clear(queue_t* queue)
{
  net_mutex_lock(&queue->mutex);
  while (queue->begin_chunk != queue->end_chunk) {
    chunk_t *o;
    o = queue->begin_chunk;
    queue->begin_chunk = queue->begin_chunk->next;
    free (o);
  }
  queue->begin_pos = queue->back_pos = 0;
  queue->back_chunk = NULL;
  queue->size_ = 0 ;
  net_mutex_unlock(&queue->mutex);
}