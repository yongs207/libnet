#ifndef net_queue_h
#define net_queue_h

typedef struct queue_ queue_t;

queue_t* nqueue_create(int sync);

void nqueue_release(queue_t* queue);

void* nqueue_pop(queue_t* queue);

void nqueue_push(queue_t* queue,void* item);

int nqueue_size(queue_t* queue);

void nqueue_sync_clear(queue_t* queue);

void* nqueue_sync_pop(queue_t* queue);

void nqueue_sync_push(queue_t* queue,void* item);

#endif