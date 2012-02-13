#ifndef net_io_event_h
#define net_io_event_h

#include "net_def.h"
#include "net_connection.h"
#include "net_log.h"

typedef struct net_io_event_ net_io_event_t; 

enum io_event_type
{
	select,
	epoll,
};

//根据io_event_type_创建对应的io模型的方式，如select指其后面的实现方式采用select 
//而epool则采用epool（只可在linux下使用）
net_io_event_t* nio_event_create(enum io_event_type io_event_type_, net_log_t* net_log_ ,net_connection_t* conn_array);

//启动io线程进行事件监听
void nio_event_dispatch(net_io_event_t*self);

//如果添加不成功，返回-1
int  nio_event_add_fd(net_io_event_t*self,net_connection_t* conn_ptr);

int  nio_event_remove_fd(net_io_event_t*self,net_connection_t* conn_ptr);

//析构net_io_event_t，关闭io模型中启动的io线程
void nio_event_destory(net_io_event_t*self);

#endif