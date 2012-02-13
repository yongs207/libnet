#ifndef net_vector_h
#define net_vector_h

#include <stdlib.h>

typedef struct _nvector nvector_t;

//  @interface
//  Create a new vector container
nvector_t * nvector_new (int size);

//  Destroy a vector container
void nvector_destroy (nvector_t **self_p);

//  Return first item in the vector, or null
void * nvector_at (nvector_t *self,int index);

//  Return next item in the vector, or null.it's iteraor
void *nvector_next (nvector_t *self);

//  Append an item to the end of the vector
void nvector_append (nvector_t *self, void *item);

//  Push an item to the start of the vector
void  nvector_push (nvector_t *self, void *item);


//  Remove the specified item from the vector if present
void  nvector_remove (nvector_t *self, void *item);

void  nvector_remove_at (nvector_t *self, int index);

//  Copy the entire vector, return the copy
nvector_t *  nvector_copy (nvector_t *self);

//  Return number of items in the vector
size_t  nvector_size (nvector_t *self);

//  Return capicity of items in the vector
size_t  nvector_capicity (nvector_t *self);

void* nvector_data(nvector_t *self);

#endif