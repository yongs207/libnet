#ifndef net_h
#define net_h 

typedef struct net_context_ net_context_t;

net_context_t* net_create_context();

void net_destory_context(net_context_t** context_ptr_ptr);

//建立连接

//创建不同的chanel进行数据处理

/////////////////////////net msg 相关函数的处理////////////////////////////////////////////////
//size为消息体的长度，不包括消息头长，但包含扩展消息头的头长
void* net_msg_create(int size);


void* net_msg_data(void* msg);

//获得消息体的长度，不包括消息头长和消息的扩展头长
int   net_msg_size();

//将msg的引用计数-1，如果当前的引用计数为0 则析构msg
void  net_msg_close(void** msg);

//将消息的引用计数加count(由于当前使用中消息没有出现在不同线程中的场景所以没有对引用计数上加锁)
void  net_msg_increase_ref(void** msg,int count);

void  net_msg_set_exheader(void* msg,void* data,int len);

//返回扩展消息头，len为扩展消息头的长度
void* net_msg_exheader(void* msg,int* len);

/////////////////////属性设置的相关操作//////////////////////////////////////////////






#endif