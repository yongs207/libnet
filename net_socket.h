#ifndef net_socket_h
#define net_socket_h

#include "net_def.h"
//  Associates a socket with a native socket descriptor.
int nsock_setsockbuf (fd_t fd_, int sndbuf_, int rcvbuf_);

//  Closes the underlying socket.
int nsock_close (fd_t fd_);

//  Reads data from the socket (up to 'size' bytes). Returns the number
//  of bytes actually read (even zero is to be considered to be
//  a success). In case of error or orderly shutdown by the other
//  peer -1 is returned.
int nsock_read (fd_t fd_,void *data, int size);

//  Writes data to the socket. Returns the number of bytes actually
//  written (even zero is to be considered to be a success). In case
//  of error or orderly shutdown by the other peer -1 is returned.
int nsock_write (fd_t fd_,const void *data, int size);

//全部发送完size长度的数据，如果返回-1则表示发送失败
int nsock_full_write(fd_t fd_,void *data, int size);

#endif