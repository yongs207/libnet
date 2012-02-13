#include "net_io_select.h"
#include "net_vector.h"
#include "net_thread.h"
#include "net_socket.h"

struct net_io_select_ 
{
	fd_t maxfd;
	fd_set source_set_in;
	fd_set readfds;
    int retired; //用于标记其否存在已废弃的socket
	net_log_t* net_log_;
	net_connection_t* conn_array;
	int work_started;
	void* thread_handle;
	nvector_t* connector_vector;
	//为心跳包预留
	int heart_beat_time;
};

static void remove_retired_socket(net_io_select_t* own)
{
	int i,conn_size = nvector_size(own->connector_vector);
	//  Destroy retired event sources.
	for(i=conn_size;i>=conn_size;i--)
	{
		net_connection_t* item = (net_connection_t*)(net_connection_t*)nvector_at(own->connector_vector,i);
		if(item->connection_status&STATUS_ALIVE&&item->fd ==retired_fd);
			nvector_remove_at(own->connector_vector,i);
	}
	own->retired = 0;
}


static void work_loop(void* arg)
{
	net_io_select_t* own =(net_io_select_t*)(arg);
	struct timeval tv = {2,0};
	while(own->work_started)
	{
		int rc ;
		//  Intialise the pollsets.
		tv.tv_sec=2-tv.tv_sec;// on linux the value update to 0
		memcpy (&own->readfds, &own->source_set_in, sizeof own->source_set_in);
		rc = select (own->maxfd + 1, &own->readfds, NULL, NULL,&tv);
#ifdef _WIN32
		if(rc == SOCKET_ERROR)
		{
		  FD_ZERO (&own->source_set_in);
		  nlog_log(own->net_log_,LOG_ERROR,"select listen error");
		  break;
		}
#else
		if (rc == -1 && errno == EINTR)
		  continue;
		if (rc == -1)
		{
		  nlog_log(own->net_log_,LOG_ERROR,"select listen error");
		  break;
		}
#endif
		if(rc!=0)
		{
			int i,conn_size = nvector_size(own->connector_vector) ;//先获得conn_size ,在循环中这个值有可能会增加
			for ( i = 0; i < conn_size ; i ++) 
			{
				net_connection_t* item = (net_connection_t*)(net_connection_t*)nvector_at(own->connector_vector,i);
				if(item->connection_status&STATUS_ALIVE)
				{
					fd_t fd = item->fd ;
					if (fd == retired_fd)
					continue;
					if (FD_ISSET (fd, &own->readfds))
					{
						// conn->callback
					}
						//do_receive(socket_ctx_ptr,&socket_ctx_ptr->sessions[i]);
				}
			}
		}

		//check_heartbeat(socket_ctx_ptr);
		if(own->retired)
			remove_retired_socket(own);
	}
}

net_io_select_t* nio_select_create( net_log_t* net_log_ ,net_connection_t* conn_array)
{
	net_io_select_t* own = (net_io_select_t*)malloc(sizeof(net_io_select_t));
	own->net_log_ = net_log_;
	own->conn_array = conn_array;
	own->connector_vector = nvector_new(FD_SETSIZE);
	FD_ZERO (&own->source_set_in);
	own->maxfd=retired_fd;
	own->retired=0;
	own->heart_beat_time=0;
}

//启动io线程进行事件监听
void nio_select_dispatch(void*self)
{
	net_io_select_t* own =(net_io_select_t*)(self);
	own->work_started =1;
	own->thread_handle = nthread_create(work_loop,own);
}

int nio_select_add_fd(void* self,net_connection_t* conn_ptr)
{
	net_io_select_t* own =(net_io_select_t*)(self);
	fd_t fd = conn_ptr->fd;
	if(nvector_size(own->connector_vector)>=FD_SETSIZE)
	{
		nlog_log(own->net_log_,LOG_ERROR,"Not enough to handle the select distribution");
		return -1;
	}
	FD_SET (fd, &own->source_set_in);
	if (fd > own->maxfd)
		own->maxfd = fd;
	nvector_append(own->connector_vector,conn_ptr);
	return 0;
}

int  nio_select_remove_fd(void* self,net_connection_t* conn_ptr)
{
	net_io_select_t* own =(net_io_select_t*)(self);
	fd_t fd=conn_ptr->fd;
	//  Mark the descriptor as retired.
	conn_ptr->fd = retired_fd;

	//  Stop polling on the descriptor.
	FD_CLR (fd, &own->source_set_in);
	//  Discard all events generated on this file descriptor.
	FD_CLR (fd, &own->readfds);

	//  Adjust the maxfd attribute if we have removed the
	//  highest-numbered file descriptor.
	if (fd == own->maxfd) {
		int i,conn_count;
		nvector_t* conns = own->connector_vector;
		own->maxfd = retired_fd;
		conn_count = nvector_size(conns);
		for (i=0;i<conn_count;++i)
		{
			net_connection_t* item = (net_connection_t*)nvector_at(conns,i);
			if(item->fd >own->maxfd )
				own->maxfd = item->fd;
		}
	}
	own->retired=1;
}

//析构net_io_event_t，关闭io模型中启动的io线程
void nio_select_destory(void* self)
{
	net_io_select_t* own =(net_io_select_t*)(self);
}