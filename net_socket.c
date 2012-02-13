#include "net_socket.h"
#include "net_err.h"

#ifdef _WIN32

int nsock_full_write(fd_t fd_,void *data, int size)
{
	int nbytes = nsock_write(fd_,data,size);
	int tmp_size = size;
	while(nbytes>=0&&nbytes<tmp_size)
	{
		data = (char*)data+nbytes;
		tmp_size -= nbytes;
		nbytes = nsock_write(fd_,data,tmp_size);
	}
	if(size==nbytes)
		return tmp_size;
	return -1;
}

//  Associates a socket with a native socket descriptor.
int nsock_setsockbuf (fd_t fd_, int sndbuf_, int rcvbuf_)
{
    if (sndbuf_)
    {
        int sz = (int) sndbuf_;
        int rc = setsockopt (fd_, SOL_SOCKET, SO_SNDBUF,
                             (char*) &sz, sizeof (int));
        errno_assert (rc == 0);
    }

    if (rcvbuf_)
    {
        int sz = (int) rcvbuf_;
        int rc = setsockopt (fd_, SOL_SOCKET, SO_RCVBUF,
                             (char*) &sz, sizeof (int));
        errno_assert (rc == 0);
    }

    return 0;
}

//  Closes the underlying socket.
int nsock_close (fd_t fd_)
{
    int rc;
    net_assert (fd_ != retired_fd);
    rc = closesocket (fd_);
    wsa_assert (rc != SOCKET_ERROR);
    return 0;
}


//  Writes data to the socket. Returns the number of bytes actually
//  written (even zero is to be considered to be a success). In case
//  of error or orderly shutdown by the other peer -1 is returned.
int nsock_write (fd_t fd_,const void *data, int size)
{
    int nbytes = send (fd_, (char*) data, size, 0);
    //  If not a single byte can be written to the socket in non-blocking mode
    //  we'll get an error (this may happen during the speculative write).
    if (nbytes == SOCKET_ERROR && WSAGetLastError () == WSAEWOULDBLOCK)
        return 0;

    //  Signalise peer failure.
    if (nbytes == SOCKET_ERROR )
        return -1;

    return (size_t) nbytes;
}



//  Reads data from the socket (up to 'size' bytes). Returns the number
//  of bytes actually read (even zero is to be considered to be
//  a success). In case of error or orderly shutdown by the other
//  peer -1 is returned.
int nsock_read (fd_t fd_,void *data, int size)
{
    int nbytes = recv (fd_, (char*) data, size, 0);

    //  Connection failure.
    if (nbytes ==SOCKET_ERROR )
        return -1;

    //  Orderly shutdown by the other peer.
    if (nbytes == 0)
        return -1;

    return (size_t) nbytes;
}
#else

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

//  Associates a socket with a native socket descriptor.
int nsock_setsockbuf (fd_t fd_, int sndbuf_, int rcvbuf_)
{
    if (sndbuf_)
    {
        int sz = (int) sndbuf_;
        int rc = setsockopt (fd_, SOL_SOCKET, SO_SNDBUF, &sz, sizeof (int));
        errno_assert (rc == 0);
    }

    if (rcvbuf_)
    {
        int sz = (int) rcvbuf_;
        int rc = setsockopt (fd_, SOL_SOCKET, SO_RCVBUF, &sz, sizeof (int));
        errno_assert (rc == 0);
    }

    return 0;
}

//  Closes the underlying socket.
int nsock_close (fd_t fd_)
{
    int rc ;
    net_assert (fd_ != retired_fd);
    rc = close (fd_);
    if (rc != 0)
        return -1;
    return 0;
}


//  Writes data to the socket. Returns the number of bytes actually
//  written (even zero is to be considered to be a success). In case
//  of error or orderly shutdown by the other peer -1 is returned.
int nsock_write (fd_t fd_,const void *data, int size)
{
    ssize_t nbytes = send (fd_, data, size, 0);

    //  Several errors are OK. When speculative write is being done we may not
    //  be able to write a single byte to the socket. Also, SIGSTOP issued
    //  by a debugging tool can result in EINTR error.
    if (nbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK ||
                         errno == EINTR))
        return 0;

    //  Signalise peer failure.
    if (nbytes == -1 )
        return -1;

    return (size_t) nbytes;
}

//  Reads data from the socket (up to 'size' bytes). Returns the number
//  of bytes actually read (even zero is to be considered to be
//  a success). In case of error or orderly shutdown by the other
//  peer -1 is returned.
int nsock_read (fd_t fd_,void *data, int size)
{
    ssize_t nbytes = recv (fd_, data, size, 0);

    //  Several errors are OK. When speculative read is being done we may not
    //  be able to read a single byte to the socket. Also, SIGSTOP issued
    //  by a debugging tool can result in EINTR error.
    if (nbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK ||
                         errno == EINTR))
        return 0;

    //  Signalise peer failure.
    if (nbytes == -1)
        return -1;

    //  Orderly shutdown by the other peer.
    if (nbytes == 0)
        return -1;

    return (size_t) nbytes;
}

#endif