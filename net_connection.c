#include "net_connection.h"

#include <assert.h>


struct net_conn_manager_
{
  int max_conn_count;
  int free_conn_count;
  net_connection_t* conn_free_list;
  net_connection_t* alive_list;
  net_log_t* log_ptr;
};

////初始化connection数组，并将connection连接起来，形成双向链表，其中net_connection数组
////的第一个为预留free connection 链表的头指针 
net_conn_manager_t* nconnection_create(int max_connection_count,net_log_t* net_log_ptr)
{
  int i;
  net_conn_manager_t* conn_manager_ptr = (net_conn_manager_t* )malloc(sizeof(*conn_manager_ptr));
  net_connection_t* conn = (net_connection_t*)malloc(sizeof(net_connection_t)*max_connection_count);
  assert(max_connection_count>2);
  conn_manager_ptr->max_conn_count = max_connection_count;
  conn_manager_ptr->log_ptr = net_log_ptr;
  conn_manager_ptr->free_conn_count = max_connection_count-1;//第0个为free的头指针
  conn_manager_ptr->conn_free_list = &conn[0];

  for(i=0;i<max_connection_count-1;i++)
  {
    //self[i].burden = 0;
    conn[i].connection_status = STATUS_IDLE ;
    conn[i].fd = retired_fd;
    conn[i].id = i;
    conn[i].read_buf = NULL;
    conn[i].next = &conn[i+1];
    conn[i+1].prev = &conn[i];
  }
  //self[i].burden = 0;
  conn[max_connection_count-1].connection_status = STATUS_IDLE ;
  conn[max_connection_count-1].fd = retired_fd;
  conn[max_connection_count-1].id = max_connection_count-1;
  conn[max_connection_count-1].read_buf = NULL;
  conn[max_connection_count-1].next = &conn[0];
  conn[0].prev = &conn[max_connection_count-1];
  return conn_manager_ptr;
}




//从free connection链表的标头处取出一个，并初始化一些该item的成员变量
//如果已经没有可连接的conn则返回null
net_connection_t* nconnection_get(net_conn_manager_t* self)
{
  net_connection_t* item;
  if(self->free_conn_count==0)
  {
    nlog_exception_log(self->log_ptr,"not enough connection ");
    return NULL;
  }
  item=self->conn_free_list->next;
  item->connection_status = STATUS_OPEN;
  item->read_buf = nbuf_create(NET_DEFAULT_READ_BUF_SIZE);
  self->conn_free_list->next = item->next;
  item->next->prev = self->conn_free_list;
  item->net_log = self->log_ptr;
  self->free_conn_count--;
	return item;
}

//将item放回到free connection的链表尾部，并将item的一些成员变量还原
void nconnection_free(net_conn_manager_t* self,net_connection_t* item)
{
	item->fd = retired_fd;
	//assert(item->connection_status==STATUS_CLOSE);
	item->connection_status = STATUS_IDLE;
	nbuf_destory(&item->read_buf);
  item->next = self->conn_free_list;
  self->conn_free_list->prev->next = item;
  item->prev = self->conn_free_list->prev;
  self->conn_free_list->prev = item;
  self->free_conn_count++;
}

void nconnections_destory(net_conn_manager_t* self)
{
	free(self);
  assert(self->free_conn_count+1==self->max_conn_count);
}
//for debug
void nconnections_dump(net_conn_manager_t* self)
{
  int i = 0;
  net_connection_t* item =self->conn_free_list->next;
  printf("free count： %d\n",self->free_conn_count);
  while(item!=self->conn_free_list)
  {
    printf("%d,\t",item->id);
    item=item->next;
    if(i++%11==12) printf("\n");
  }
  printf("\n");
}


