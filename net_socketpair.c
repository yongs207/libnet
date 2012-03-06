#include "net_socketpair.h"

#ifndef WSA_START
void static socket_init()
{
  WORD version_requested = MAKEWORD (2, 2);
  WSADATA wsa_data;
  int rc = WSAStartup (version_requested, &wsa_data);
  wsa_assert (rc == 0);
  net_assert (LOBYTE (wsa_data.wVersion) == 2 &&HIBYTE (wsa_data.wVersion) == 2);
}
#else
void static socket_init()
{

}
#endif


void nsocketpair(fd_t* pair)
{
#ifdef _WIN32
  //  Create listening socket.
  SOCKET listener;
  BOOL so_reuseaddr = 1;
  int rc;
  BOOL tcp_nodelay = 1;
  struct sockaddr_in addr;
  int addrlen = sizeof (addr);
  //  Windows has no 'socketpair' function. CreatePipe is no good as pipe
  //  handles cannot be polled on. Here we create the socketpair by hand.
  pair[0] = INVALID_SOCKET;
  pair[1] = INVALID_SOCKET;
  socket_init();
  listener = socket (AF_INET, SOCK_STREAM, 0);
  wsa_assert (listener != INVALID_SOCKET);

  //  Set SO_REUSEADDR and TCP_NODELAY on listening socket.

  rc = setsockopt (listener, SOL_SOCKET, SO_REUSEADDR,
    (char *)&so_reuseaddr, sizeof (so_reuseaddr));
  wsa_assert (rc != SOCKET_ERROR);
  rc = setsockopt (listener, IPPROTO_TCP, TCP_NODELAY,
    (char *)&tcp_nodelay, sizeof (tcp_nodelay));
  wsa_assert (rc != SOCKET_ERROR);

  //  Bind listening socket to any free local port.

  memset (&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);
  addr.sin_port = 0;
  rc = bind (listener, (const struct sockaddr*) &addr, sizeof (addr));
  wsa_assert (rc != SOCKET_ERROR);

  //  Retrieve local port listener is bound to (into addr).

  rc = getsockname (listener, (struct sockaddr*) &addr, &addrlen);
  wsa_assert (rc != SOCKET_ERROR);

  //  Listen for incomming connections.
  rc = listen (listener, 1);
  wsa_assert (rc != SOCKET_ERROR);

  //  Create the writer socket.
  pair[0] = WSASocket (AF_INET, SOCK_STREAM, 0, NULL, 0,  0);
  wsa_assert (pair[0] != INVALID_SOCKET);

  //  Set TCP_NODELAY on writer socket.
  rc = setsockopt (pair[0], IPPROTO_TCP, TCP_NODELAY,
    (char *)&tcp_nodelay, sizeof (tcp_nodelay));
  wsa_assert (rc != SOCKET_ERROR);

  //  Connect writer to the listener.
  rc = connect (pair[0], (struct sockaddr *) &addr, sizeof (addr));
  wsa_assert (rc != SOCKET_ERROR);

  //  Accept connection from writer.
  pair[1] = accept (listener, NULL, NULL);
  wsa_assert (pair[1] != INVALID_SOCKET);

  //  We don't need the listening socket anymore. Close it.
  rc = closesocket (listener);
  wsa_assert (rc != SOCKET_ERROR);

#else
  int rc = socketpair (AF_UNIX, SOCK_STREAM, 0, pair);
  errno_assert (rc == 0);
#endif

}