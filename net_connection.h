
#pragma once
//#ifndef net_connection_h
//#define net_connection_h

#include "net_log.h"
#include "net_lock.h"

#include "net_def.h"
#include "net_buffer.h"

typedef struct net_connection_   net_connection_t;
typedef struct net_conn_manager_ net_conn_manager_t;

typedef void (*nconn_event_in)(net_connection_t* self);
typedef void (*nconn_event_out)(net_connection_t* self);

//connection 需要包含session 和linker相关的元素

//需要包含appid 管理的相关功能模块，使其具备记录当前势力是否为单例，对非单例每个app_id 下一个专有的优先队列，对负载均衡进行管理
//
struct net_connection_ 
{
  int connection_status;
  struct net_connection_* next;
  struct net_connection_* prev;
  net_log_t* net_log;
  fd_t fd;
  net_buffer_t* read_buf;//default size is NET_DEFAULT_READ_BUF_SIZE
  int id;
  nconn_event_in read_fun;
  nconn_event_out write_fun;
  //int burden;//用于有负载均衡的客户端的负载压力记录
  //nconn_event_in on_event_in;
  //void* key;//通过外部设置同时也需要外部释放，绑定的key可以方便的用于回调函数中

  // read_buf
  //read callback
  //write callback
  //close callback
  //connect notify
  
};

#define STATUS_IDLE        0x00
#define STATUS_OPEN        0x01
#define STATUS_REGISTE     0x02
#define STATUS_CLOSE       0x80
#define STATUS_ALIVE       0x7f //与当前状态与运算，如果不为0 则为alive状态




//初始化connection数组，并将connection连接起来，形成链表，其中net_connection数组
//的第一个为预留free connection 链表的头指针
net_conn_manager_t* nconnection_create(int max_connection_count,net_log_t* net_log_ptr);

//从free connection链表的标头处取出一个，并初始化一些该item的成员变量
net_connection_t* nconnection_get(net_conn_manager_t* self);

//将item放回到free connection的链表尾部，并将item的一些成员变量还原
void nconnection_free(net_conn_manager_t* self,net_connection_t* item);

void nconnections_destory(net_conn_manager_t* self);

void nconnections_dump(net_conn_manager_t* self);

//#endif