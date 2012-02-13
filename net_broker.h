#ifndef net_broker_h
#define net_broker_h

typedef struct net_broker_ net_broker_t;

net_broker_t* nbroker_create(int thread_count,int max_connection_count);

void nbroker_start(net_broker_t* self,int port);

void nbroker_close(net_broker_t* self);

void nbroker_destory(net_broker_t** self_ptr);

void nbroker_send(net_broker_t* self,void* data);

//收到消息的回调函数

//需要进行登录验证
//需要查看appid的列表
//需要查看当前各个app 服务的状态
//对数据服务日志的处理


//拦截所有来自app_id 的消息
//栏架所有发往app_id 的消息
//拦截所有来自source 的消息
void set_filter



#endif