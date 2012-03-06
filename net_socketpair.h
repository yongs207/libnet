#ifndef net_socketpair_h
#define net_socketpair_h

#include "net_def.h"
#include "net_err.h"

//创建socketpair，没有返回值，如果出现错误将直接触发assert
void nsocketpair(fd_t* pair);



#endif