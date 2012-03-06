#ifndef net_listener_h
#define net_listener_h

#include "net_def.h"

fd_t net_listen (const char *ip_addr,unsigned short port,int backlog_);

fd_t net_accept (fd_t fd_);

#endif