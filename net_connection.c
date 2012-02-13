#include "net_connection.h"
#include "net_buffer.h"

#include <assert.h>
//初始化connection数组，并将connection连接起来，形成链表，其中net_connection数组
//的第一个为预留free connection 链表的头指针 第2个的next指向尾指针
net_connection_t* nconnection_create(int max_connection_count)
{
	int i;
	net_connection_t* self = (net_connection_t*)malloc(sizeof(net_connection_t)*max_connection_count);
	assert(max_connection_count>2);
	for(i=2;i<max_connection_count-1;i++)
	{
		self[i].burden = 0;
		self[i].connection_status = STATUS_IDLE ;
		self[i].fd = retired_fd;
		self[i].read_buf = NULL;
		self[i].next = &self[i+1];
	}
	self[i].burden = 0;
	self[i].connection_status = STATUS_IDLE ;
	self[i].fd = retired_fd;
	self[i].read_buf = NULL;
	self[i].next = NULL;
	self[0].next = &self[2];
	self[1].next = &self[i];
	return self;
}


//从free connection链表的标头处取出一个，并初始化一些该item的成员变量
net_connection_t* nconnection_get(net_connection_t* self)
{
	net_connection_t* item =NULL;
	if(self[0].next!=NULL)//self[0].next为空表示已经没有可用的connection
	{
		item = self[0].next;
		item->connection_status = STATUS_OPEN;
		item->read_buf = nbuf_create(NET_DEFAULT_READ_BUF_SIZE);
		self[0].next = item->next;
	}
	return item;
}

//将item放回到free connection的链表尾部，并将item的一些成员变量还原
void nconnection_free(net_connection_t* self,net_connection_t* item)
{
	item->fd = retired_fd;
	assert(item->connection_status==STATUS_CLOSE);
	item->connection_status = STATUS_IDLE;
	nbuf_destory(&item->read_buf);
	self[1].next->next = item;
	self[1].next = item;
}
//需要释放所以alive的recv_buf
void nconnections_destory(net_connection_t* self)
{
	net_connection_t* item;
	while(item=self[0].next)
	{
		nbuf_destory(&item->read_buf);
		item=item->next;
	}
	free(self);
}


