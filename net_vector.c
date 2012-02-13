#include "net_vector.h"
#include <string.h>

typedef struct _nvector
{
  int capicity;
  int size;
  int iter;
  void** data;
};

#define MIN_VECTOR_SIZE 4
//  @interface
//  Create a new vector container
nvector_t * nvector_new (int size)
{
  nvector_t *self = (nvector_t *)malloc(sizeof(nvector_t));
  size = size<MIN_VECTOR_SIZE?MIN_VECTOR_SIZE:size;
  self->data = (void**)malloc(sizeof(void*)*size);
  self->iter = 0 ;
  self->size = 0;
  self->capicity =4;
  return self;
}

//  Destroy a vector container
void nvector_destroy (nvector_t **self_p)
{
  nvector_t *self = *self_p;
  free(self->data);
  free(self);
  *self_p = NULL;
}

//  Return first item in the vector, or null
void * nvector_at (nvector_t *self,int index)
{
  if(index<self->capicity&&index>=0)
    return self->data[index];
  return NULL;
}

//  Return next item in the vector, or null.it's iteraor
void *nvector_next (nvector_t *self)
{
  if(self->size==0)
    return NULL;
  if(self->iter==self->size)
    self->iter =0;
  return self->data[self->iter++];
}

//  Append an item to the end of the vector
void nvector_append (nvector_t *self, void *item)
{
  if(self->size==self->capicity){
    self->data = (void**)realloc(self->data,sizeof(void*)*self->capicity*2);
    self->capicity *=2;
  }
  self->data[self->size++] = item;
}

//  Push an item to the start of the vector
void  nvector_push (nvector_t *self, void *item)
{
  int index ;
  if(self->size==self->capicity){
    self->data = (void**)realloc(self->data,sizeof(void*)*self->capicity*2);
    self->capicity *=2;
  }
  self->size++;
  for(index=self->size;index>0;index--)
    self->data[index] = self->data[index-1];
  self->data[0] = item;
}


//  Remove the specified item from the vector if present
void  nvector_remove (nvector_t *self, void *item)
{
  int index;
  for (index =0;index<self->size;index++)
  {
    if(self->data[index]==item)
      break;
  }
  nvector_remove_at(self,index);
}

void  nvector_remove_at (nvector_t *self, int index)
{
  int i;
  if(index<self->capicity&&index>=0)
  {
    for(i=self->size;i>index;i--)
      self->data[i] = self->data[i-1];
    self->size--;
  }

}

//  Copy the entire vector, return the copy
nvector_t *  nvector_copy (nvector_t *self)
{
  nvector_t* pcopy = (nvector_t*)malloc(sizeof(nvector_t));
  memcpy(pcopy->data,self->data,sizeof(void*)*self->capicity);
  pcopy->iter = self->iter;
  pcopy->size = self->size;
  pcopy->capicity = self->capicity;
  return pcopy;
}

//  Return number of items in the vector
size_t  nvector_size (nvector_t *self)
{
  return self->size;
}

//  Return capicity of items in the vector
size_t  nvector_capicity (nvector_t *self)
{
  return self->capicity;
}


void* nvector_data(nvector_t *self)
{
  return self->data[0];
}