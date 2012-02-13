#ifndef net_connector_h
#define net_connector_h

#include "net_def.h"

fd_t sock_connect(const char *ip_addr,unsigned short port);

#endif