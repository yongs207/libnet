#ifndef net_buffer_h
#define net_buffer_h

#include <stdlib.h>
#include "net_err.h"

//由于关于buffer的函数使用的频率比较高，所以将其实现放入头文件中这样有利于编译器将其内联
typedef struct net_buffer_
{
	char* data;
	int base_len;
	char* tmp_data;
	int len;
	int begin_pos;
	int end_pos;

}net_buffer_t;

net_buffer_t* nbuf_create(int size);

#define nbuf_data(self) ((self)->data+(self)->begin_pos)
#define nbuf_size(self) ((self)->end_pos-(self)->begin_pos)
#define nbuf_tailer(self) ((self)->data+(self)->end_pos)
#define nbuf_space_size(self) ((self)->len-(self)->end_pos)

//由于创建的data直接作为一个新的msg，所以内存不需要释放,直接由外部释放
void nbuf_clear(net_buffer_t* self);

char* nbuf_reset(net_buffer_t* self,int size);

void nbuf_product(net_buffer_t* self,int size);

void nbuf_consume(net_buffer_t* self,int size);

void nbuf_destory(net_buffer_t** self);

static void s_dump(char *buffer, int sz) ;

void nbuf_dump(net_buffer_t* self);



#endif