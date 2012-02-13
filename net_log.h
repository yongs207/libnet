#ifndef net_log_h
#define net_log_h

#include <stdlib.h>

typedef enum log_level_ 
{
	LOG_WARN,
	LOG_DEBUG,
	LOG_INFO,
	LOG_EXCEPTION,
	LOG_ERROR
}log_level_t;


typedef struct net_log_ net_log_t;

typedef void (*callback_t)(net_log_t* self,log_level_t log_level,char* format,va_list arg_ptr);

net_log_t* nlog_create();

//打开文件句柄，默认的日志输出stderr，打开失败返回-1
int nlog_open(net_log_t* self,const char* file_path);

//如果打开了文件句柄，关闭文件句柄，失败返回-1，如果没有打开文件而调用关闭函数将调用函数失败返回-1
int nlog_close(net_log_t* self);

void nlog_destory(net_log_t** self_ptr);

//设置log处理的级别，产生的日志级别小于设置的级别时，日志将其忽略。默认级别为处理所有的日志信息
void nlog_set_level(net_log_t* self,log_level_t log_level);

void nlog_log(net_log_t* self,log_level_t log_level,char* format,...);

//设置log的回调函数，在有日志产生时将回调此函数，当没有设置回调函数时采用系统默认的函数进行处理
void nlog_set_callback(net_log_t* self,callback_t log);



#endif