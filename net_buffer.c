#include "net_buffer.h"


net_buffer_t* nbuf_create(int size)
{
  net_buffer_t* self = (net_buffer_t*)malloc(sizeof(net_buffer_t));
  net_assert(self);
  self->begin_pos = self->end_pos = 0;
  self->base_len = size;
  self->len = size;
  self->tmp_data = NULL;
  self->data = (char*)malloc(sizeof(char)*size);
  net_assert(self->data);
  return self;
}

//由于创建的data直接作为一个新的msg，所以内存不需要释放,直接由外部释放
void nbuf_clear(net_buffer_t* self)
{
  self->begin_pos = self->end_pos = 0;
  self->data = self->tmp_data;
  self->tmp_data = NULL;
}

char* nbuf_reset(net_buffer_t* self,int size)
{
  char* tmp;
  int len = self->end_pos-self->begin_pos;
  self->tmp_data = (char*)malloc(sizeof(char)*size);
  memcpy(self->tmp_data,self->data+self->begin_pos,len);
  self->begin_pos = 0;
  self->end_pos = len;
  self->len = size;
  tmp = self->data;
  self->data = self->tmp_data;
  self->tmp_data = tmp;
  return self->data ;
}

void nbuf_product(net_buffer_t* self,int size)
{
  net_assert(nbuf_space_size(self)>=size);
  self->end_pos +=size;
}

void nbuf_consume(net_buffer_t* self,int size)
{
  net_assert(nbuf_size(self)>=size);
  self->begin_pos +=size;
  if(self->begin_pos == self->end_pos)
    self->begin_pos = self->end_pos = 0;
}

void nbuf_destory(net_buffer_t** self)
{
  free((*self)->data);
  if((*self)->tmp_data!=NULL)
    free((*self)->tmp_data);
  free(*self);
  *self = NULL;
}

static void s_dump(char *buffer, int sz) 
{
  int i , j;
  for (i=0;i<sz;i++) 
  {
    printf("%02X ",buffer[i]);
    if (i % 16 == 15) 
    {
      for (j = 0 ;j <16 ;j++) {
        char c = buffer[i/16 * 16+j];
        if (c>=32 && c<127) {
          printf("%c",c);
        } else {
          printf(".");
        }
      }
      printf("\n");
    }
  }

  for(i=sz%16;i<16;i++)//打印最后一行
    printf("   ");
  for (i=sz/16*16;i<sz;i++)
  {
    char c = buffer[i];
    if (c>=32 && c<127) {
      printf("%c",c);
    } else {
      printf(".");
    }
  }

  printf("\n");
}


void nbuf_dump(net_buffer_t* self)
{
  printf(self->tmp_data?"big_buf\t":"small_buf\t");
  printf("begin: %d\tend: %d\n",self->begin_pos,self->end_pos);
  printf(".....................................................\n");
  s_dump(nbuf_data(self),nbuf_size(self));
}