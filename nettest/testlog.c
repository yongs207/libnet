#include "../net_log.h"

void default_log()
{
  net_log_t* log = nlog_create();
  nlog_log(log,LOG_DEBUG,"debug test");
  nlog_log(log,LOG_WARN,"warn test");
  nlog_log(log,LOG_INFO,"info test");
  nlog_log(log,LOG_EXCEPTION,"exception test");
  nlog_log(log,LOG_ERROR,"error test");
  nlog_set_level(log,LOG_INFO);
  nlog_log(log,LOG_DEBUG,"debug test");
  nlog_log(log,LOG_WARN,"warn test");
  nlog_log(log,LOG_INFO,"info test");
  nlog_log(log,LOG_EXCEPTION,"exception test");
  nlog_log(log,LOG_ERROR,"error test");
  nlog_destory(&log);
}

void file_log()
{
  net_log_t* log = nlog_create();
  nlog_open(log,"gava.log");
  nlog_log(log,LOG_DEBUG,"debug test");
  nlog_log(log,LOG_WARN,"warn test");
  nlog_log(log,LOG_INFO,"info test");
  nlog_log(log,LOG_EXCEPTION,"exception test");
  nlog_log(log,LOG_ERROR,"error test");
  nlog_set_level(log,LOG_INFO);
  nlog_log(log,LOG_DEBUG,"debug test");
  nlog_log(log,LOG_WARN,"warn test");
  nlog_log(log,LOG_INFO,"info test");
  nlog_log(log,LOG_EXCEPTION,"exception test");
  nlog_log(log,LOG_ERROR,"error test");
  nlog_destory(&log);
}

static void nlog_log_(net_log_t* self,log_level_t log_level,char* format,va_list arg_ptr)
{
   vprintf (format, arg_ptr);
}

void callback_log()
{
  net_log_t* log = nlog_create();
  nlog_open(log,"gava.log");
  nlog_log(log,LOG_DEBUG,"debug test");
  nlog_log(log,LOG_WARN,"warn test");
  nlog_log(log,LOG_INFO,"info test");
  nlog_log(log,LOG_EXCEPTION,"exception test");
  nlog_log(log,LOG_ERROR,"error test");
  nlog_set_level(log,LOG_INFO);
  nlog_set_callback(log,nlog_log_);
  nlog_log(log,LOG_DEBUG,"debug test");
  nlog_log(log,LOG_WARN,"warn test");
  nlog_log(log,LOG_INFO,"info test");
  nlog_log(log,LOG_EXCEPTION,"exception test");
  nlog_log(log,LOG_ERROR,"error test");
  nlog_destory(&log);
}

//int main()
//{
//  default_log();
//  file_log();
//  callback_log();
//  system("pause");
//  return 0;
//}