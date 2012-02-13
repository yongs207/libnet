#ifndef net_buffer_h
#define net_buffer_h

#include <stdlib.h>
#include "net_err.h"

//由于关于buffer的函数使用的频率比较高，所以将其实现放入头文件中这样有利于编译器将其内联
typedef struct net_buffer_
{
	char* data;
	int len;
	char* tmp_data;
	int tmp_len;
	int begin_pos;
	int end_pos;

}net_buffer_t;

net_buffer_t* nbuf_create(int size)
{
	net_buffer_t* self = (net_buffer_t*)malloc(sizeof(net_buffer_t));
	self->begin_pos = self->end_pos = 0;
	self->len = 0;
	self->tmp_data = NULL;
	self->data = (char*)malloc(sizeof(char)*size);
	return self;
}

char* nbuf_data(net_buffer_t* self)
{
	return self->data+self->begin_pos;
}

int nbuf_size(net_buffer_t* self)
{
	return self->end_pos-self->end_pos;
}
//如果消息体长度大于buf剩余大小，则缓存区需要重置缓存区的大小
//并且下一次需要获取的缓冲区长度为（消息体长度-nbuf_size）
//当使用完消息后需要调用nbuf_clear,将缓冲区还原为原来缺省data
int nbuf_remain_size(net_buffer_t* self)
{
	return self->len-self->end_pos;
}
//由于创建的data直接作为一个新的msg，所以内存不需要释放
void nbuf_clear(net_buffer_t* self)
{
	char* tmp;
	self->begin_pos = self->end_pos = 0;
	self->data = self->tmp_data;
}

char* nbuf_reset(net_buffer_t* self,int size)
{
	char* tmp;
	int len = self->end_pos-self->begin_pos;
	self->tmp_len = self->len;
	self->tmp_data = (char*)malloc(sizeof(char)*size);
	memcpy(self->tmp_data,self->data+self->begin_pos,len);
	self->begin_pos = 0;
	self->end_pos = len;
	self->len = size;
	tmp = self->data;
	self->data = self->tmp_data;
	self->tmp_data = tmp;
}

void nbuf_product(net_buffer_t* self,int size)
{
	net_assert(nbuf_remain_size(self)>=size);
	self->end_pos +=size;
}

void nbuf_consume(net_buffer_t* self,int size)
{
	net_assert(nbuf_size(self)<=size);
	self->begin_pos +=size;
	if(self->begin_pos == self->end_pos)
		self->begin_pos = self->end_pos = 0;
}

void nbuf_destory(net_buffer_t** self)
{
	free((*self)->data);
	free(*self);
	*self = NULL;
}

#endif