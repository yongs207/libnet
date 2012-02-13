#include "net_signaler.h"
#include "sock_def.h"
#include "err.h"

#ifdef _WIN32

#else
#include <sys/select.h>
#include <fcntl.h>
#endif

#include <stdlib.h>
typedef struct pair_t
{
  fd_t w;
  fd_t r;
}pair_t;

static pair_t* make_fdpair ()
{
  pair_t* pair_ptr = (pair_t*) malloc(sizeof(pair_t));
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
  pair_ptr->w = INVALID_SOCKET;
  pair_ptr->r = INVALID_SOCKET;
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
  pair_ptr->w = WSASocket (AF_INET, SOCK_STREAM, 0, NULL, 0,  0);
  wsa_assert (pair_ptr->w != INVALID_SOCKET);

  //  Set TCP_NODELAY on writer socket.
  rc = setsockopt (pair_ptr->w, IPPROTO_TCP, TCP_NODELAY,
    (char *)&tcp_nodelay, sizeof (tcp_nodelay));
  wsa_assert (rc != SOCKET_ERROR);

  //  Connect writer to the listener.
  rc = connect (pair_ptr->w, (struct sockaddr *) &addr, sizeof (addr));
  wsa_assert (rc != SOCKET_ERROR);

  //  Accept connection from writer.
  pair_ptr->r = accept (listener, NULL, NULL);
  wsa_assert (pair_ptr->r != INVALID_SOCKET);

  //  We don't need the listening socket anymore. Close it.
  rc = closesocket (listener);
  wsa_assert (rc != SOCKET_ERROR);

#else
  int sv [2];
  int rc = socketpair (AF_UNIX, SOCK_STREAM, 0, sv);
  errno_assert (rc == 0);
  pair_ptr->w = sv [0];
  pair_ptr->r = sv [1];
#endif
   return pair_ptr;
}


void* net_signal_create()
{
  pair_t* pair_ptr = make_fdpair();
  return pair_ptr;
}

void net_signal_destory(void* signal)
{
  pair_t* pair_ptr = (pair_t*)signal;
#if defined _WIN32
  int rc = closesocket (pair_ptr->w);
  wsa_assert (rc != SOCKET_ERROR);
  rc = closesocket (pair_ptr->r);
  wsa_assert (rc != SOCKET_ERROR);
#else
  int rc = close (pair_ptr->w);
  errno_assert (rc == 0);
  rc = close (pair_ptr->r);
  errno_assert (rc == 0);
#endif
}

void net_signal_send(void* signal)
{
    pair_t* pair_ptr = (pair_t*)signal;
#if defined _WIN32
  unsigned char dummy = 0;
  int nbytes = send (pair_ptr->w, (char*) &dummy, sizeof (dummy), 0);
  wsa_assert (nbytes != SOCKET_ERROR);
  net_assert (nbytes == sizeof (dummy));
#else
  unsigned char dummy = 0;
  while (1) {
    ssize_t nbytes =send (pair_ptr->w, &dummy, sizeof (dummy), 0);
    if (unlikely (nbytes == -1 && errno == EINTR))
      continue;
    net_assert (nbytes == sizeof (dummy));
    break;
  }
#endif
}

void net_signal_recv(void* signal)
{
  unsigned char dummy;
  pair_t* pair_ptr = (pair_t*)signal;
#if defined _WIN32
  int nbytes = recv (pair_ptr->r, (char*) &dummy, sizeof (dummy), 0);
  wsa_assert (nbytes != SOCKET_ERROR);
#else
  ssize_t nbytes = recv ( pair_ptr->r, &dummy, sizeof (dummy), 0);
  errno_assert (nbytes >= 0);
#endif
  net_assert (nbytes == sizeof (dummy));
  net_assert (dummy == 0);
}

int net_signal_wait (void* signal,int timeout_)
{
  fd_set fds;
  pair_t* pair_ptr = (pair_t*)signal;
  int rc;
  struct timeval timeout;
  FD_ZERO (&fds);
  FD_SET (pair_ptr->r, &fds);
  if (timeout_ >= 0) {
    timeout.tv_sec = timeout_ / 1000;
    timeout.tv_usec = timeout_ % 1000 * 1000;
  }
#ifdef _WIN32
  rc = select (0, &fds, NULL, NULL,
    timeout_ >= 0 ? &timeout : NULL);
  wsa_assert (rc != SOCKET_ERROR);
#else
  rc = select (pair_ptr->r + 1, &fds, NULL, NULL,
    timeout_ >= 0 ? &timeout : NULL);
  if (unlikely (rc < 0)) {
    net_assert (errno == EINTR);
    return -1;
  }
#endif
  if (unlikely (rc == 0)) {
    errno = EAGAIN;
    return -1;
  }
  net_assert (rc == 1);
  return 0;
}
