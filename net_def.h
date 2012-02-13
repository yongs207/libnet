#ifndef net_def_h
#define net_def_h
#ifdef _WIN32
#if(_WIN32_WINNT >= 0x0400)
#include <winsock2.h>
#include <mswsock.h>
#else
#include <winsock.h>
#endif
#include <ws2tcpip.h>
#include <ipexport.h>
#include <process.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#endif


#ifdef _WIN32
#if defined _MSC_VER &&_MSC_VER <= 1400
typedef UINT_PTR fd_t;
enum {retired_fd = (fd_t)(~0)};
#else
typedef SOCKET fd_t;
enum {retired_fd = INVALID_SOCKET};
#endif
#else
typedef int fd_t;
enum {retired_fd = -1};
#endif


#define NET_MAX_MSG_SIZE 1024*1024*2 //(2M)

#define NET_DEFAULT_READ_BUF_SIZE 1024*8

#define NET_MAX_CONNECTION_COUNT 10000
#define NET_MAX_APP_ID NET_MAX_CONNECTION_COUNT;




#endif