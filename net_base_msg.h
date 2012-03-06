#ifndef net_base_msg_h
#define net_base_msg_h

//消息需要引用计数


 
#define COMMON_HEADER 	unsigned int msg_len;/*包含基本头长的msg长度*/                                          \
	                    unsigned short msg_type;/*其中规定60000-65536的消息为系统分配的消息*/					 \
	                    unsigned short dest_addr;																\
						unsigned short source_addr;																\
						unsigned short app_addr;																\
						unsigned short flag;/*保留站位使用*/													  \
						char more:1;/*由于大消息的发送时，需要进行拆分（消息必须小于消息通信限制的最大值），*/	 \
						/*其中为1是表示还有消息没有接收完成*/														\
						char compress:1;/*数据包的data域是否被压缩*/											  \
						char debug:1/*debug状态的消息将全部记录*/                                                \
                        char log:1/*log 状态的消息将被记录，用于回放或数据恢复*/                                 \
                        /*其他的位域用于保留使用*/																 \
						unsigned char data_offset/*数据域的偏移位置，16+data_offset*/						   \
	//需要保证消息包的顺序性	

typedef struct net_base_msg_
{
	COMMON_HEADER;
}net_base_msg_t;

enum base_msg_type 
{
	HEARTBEAT = 60000,
	OPEN,
	CLOSE
};

typedef struct net_heartbeat_msg_
{
	COMMON_HEADER;
	short cpu_usage;
	short memery_usage;
	unsigned int request_delay;
	unsigned int wait_task;
}net_heartbeat_msg_t;

typedef struct net_open_msg_
{
	COMMON_HEADER;
	short max_instance;//最大实例个数，如果max_instance 为1此势力为唯一实例
	short max_load;//最大负载值，当大于这个值时会试着创建新的实例
	short app_id;
	unsigned char app_server_name_len;//如果长度为0表示，匿名实例
	char app_server_name[1];//app 服务名以'\0'结尾

}net_open_msg_t;

typedef struct net_close_msg_
{
	COMMON_HEADER;
}net_close_msg_t;

typedef struct net_app_id_msg_
{
	COMMON_HEADER;
	short app_id;//app_id 为 0 表示没有找到对应的服务
}net_app_name_msg_t;

typedef struct net_dest_error_msg_
{
	COMMON_HEADER;
	short dest;
}net_app_name_msg_t;

//获得msg的数据域
#define nmsg_data(self) (((char*)(self))+((net_base_msg_t*)(self)->data_offset)+16)

//获得msg的数据域的长度
#define nmsg_len (self) (((net_base_msg_t*)(self)->msg_len)-(net_base_msg_t*)(self)->data_offset-16)


#endif