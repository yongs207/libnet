#include "net_connector.h"
#include "net_err.h"

static int resolve_ip_hostname (struct sockaddr_storage *addr_, socklen_t *addr_len_,const char *ip_address,int port)
{
    char service[10];
    int rc;
    struct addrinfo *res;
    //  Set up the query.
    struct addrinfo req;
    memset (&req, 0, sizeof (req));

    //  We only support IPv4 addresses for now.
    req.ai_family = AF_INET;

    //  Need to choose one to avoid duplicate results from getaddrinfo() - this
    //  doesn't really matter, since it's not included in the addr-output.
    req.ai_socktype = SOCK_STREAM;

    //  Avoid named services due to unclear socktype.
    req.ai_flags = AI_NUMERICSERV;

    //  Resolve host name. Some of the error info is lost in case of error,
    //  however, there's no way to report EAI errors via errno.
    sprintf(service,"%d",port);
    rc = getaddrinfo (ip_address, service, &req, &res);
    if (rc)
    {
        errno = EINVAL;
        return -1;
    }


    //  Copy first result to output addr with hostname and service.
    net_assert ((size_t) (res->ai_addrlen) <= sizeof (*addr_));
    memcpy (addr_, res->ai_addr, res->ai_addrlen);
    *addr_len_ = (socklen_t)res->ai_addrlen;

    freeaddrinfo (res);

    return 0;
}

#ifdef _WIN32

fd_t sock_connect(const char *ip_addr,unsigned short port)
{
    int flag = 1,rc;
    fd_t fd_;
    unsigned long argp = 1;
    struct sockaddr_storage addr;
    socklen_t addr_len;
    //if(!wasstartup) init();
    if(resolve_ip_hostname (&addr, &addr_len, ip_addr,port))
        return retired_fd;
    //  Create the socket.
    fd_ = socket (addr.ss_family, SOCK_STREAM, IPPROTO_TCP);
    if (fd_ == INVALID_SOCKET)
    {
      wsa_error_to_errno ();
      return retired_fd;
    }
    //  Disable Nagle's algorithm.

    rc = setsockopt (fd_, IPPROTO_TCP, TCP_NODELAY, (char*) &flag,
                     sizeof (int));
    net_assert (rc != SOCKET_ERROR);

    //  Connect to the remote peer.
    rc = connect (fd_, (struct sockaddr*) &addr, addr_len);

    //rc = ioctlsocket (fd_, FIONBIO, &argp);
    //wsa_assert (rc != SOCKET_ERROR);
    //  Connect was successfull immediately.
    if (rc == 0)
        return fd_;

    //  Asynchronous connect was launched.
    if (rc == SOCKET_ERROR)
        return retired_fd;
}
#else


#endif