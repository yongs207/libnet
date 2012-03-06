#include "net_signaler.h"
#include "net_def.h"
#include "net_err.h"
#include "net_socketpair.h"

#ifdef _WIN32

#else
#include <sys/select.h>
#include <fcntl.h>
#endif

#include <stdlib.h>



void* net_signal_create()
{
  fd_t* pair_ptr = (fd_t*)malloc(sizeof(fd_t)*2);
  nsocketpair(pair_ptr);
  return pair_ptr;
}

void net_signal_destory(void* signal)
{
  fd_t* pair_ptr = (fd_t*)signal;
#if defined _WIN32
  int rc = closesocket (pair_ptr[0]);
  wsa_assert (rc != SOCKET_ERROR);
  rc = closesocket (pair_ptr[1]);
  wsa_assert (rc != SOCKET_ERROR);
#else
  int rc = close (pair_ptr->w);
  errno_assert (rc == 0);
  rc = close (pair_ptr->r);
  errno_assert (rc == 0);
#endif
  free(pair_ptr);
}

void net_signal_send(void* signal)
{
  fd_t* pair_ptr = (fd_t*)signal;
#if defined _WIN32
  unsigned char dummy = 0;
  int nbytes = send (pair_ptr[0], (char*) &dummy, sizeof (dummy), 0);
  wsa_assert (nbytes != SOCKET_ERROR);
  net_assert (nbytes == sizeof (dummy));
#else
  unsigned char dummy = 0;
  while (1) {
    ssize_t nbytes =send (pair_ptr[0], &dummy, sizeof (dummy), 0);
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
  fd_t* pair_ptr = (fd_t*)signal;
#if defined _WIN32
  int nbytes = recv (pair_ptr[1], (char*) &dummy, sizeof (dummy), 0);
  wsa_assert (nbytes != SOCKET_ERROR);
#else
  ssize_t nbytes = recv ( pair_ptr[1], &dummy, sizeof (dummy), 0);
  errno_assert (nbytes >= 0);
#endif
  net_assert (nbytes == sizeof (dummy));
  net_assert (dummy == 0);
}

int net_signal_wait (void* signal,int timeout_)
{
  fd_set fds;
  fd_t* pair_ptr = (fd_t*)signal;
  int rc;
  struct timeval timeout;
  FD_ZERO (&fds);
  FD_SET (pair_ptr[1], &fds);
  if (timeout_ >= 0) {
    timeout.tv_sec = timeout_ / 1000;
    timeout.tv_usec = timeout_ % 1000 * 1000;
  }
#ifdef _WIN32
  rc = select (0, &fds, NULL, NULL,
    timeout_ >= 0 ? &timeout : NULL);
  wsa_assert (rc != SOCKET_ERROR);
#else
  rc = select (pair_ptr[1] + 1, &fds, NULL, NULL,
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
