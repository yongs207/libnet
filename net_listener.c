#include "net_listener.h"
#include "net_err.h"

static int resolve_ip_interface (struct sockaddr_storage *addr_, socklen_t *addr_len_,char const *ip_address,int port)
{
    struct sockaddr_in ip4_addr;
    struct addrinfo *res = NULL;
    struct addrinfo req;
    struct sockaddr *out_addr;
    socklen_t out_addrlen;
    char service[10];
    int rc;
    //  Initialize the output parameter.
    memset (addr_, 0, sizeof (*addr_));

    //  Initialise IPv4-format family/port.
    memset (&ip4_addr, 0, sizeof (ip4_addr));
    ip4_addr.sin_family = AF_INET;
    ip4_addr.sin_port = htons ((unsigned short) port);

    //  Initialize temporary output pointers with ip4_addr
    out_addr = (struct sockaddr *) &ip4_addr;
    out_addrlen = sizeof (ip4_addr);

    //  0 is not a valid port.
    if (!ip4_addr.sin_port)
    {
        errno = EINVAL;
        return -1;
    }

    //  * resolves to INADDR_ANY.
    if (strcmp(ip_address,"*")==0)
    {
        ip4_addr.sin_addr.s_addr = htonl (INADDR_ANY);
        net_assert (out_addrlen <= sizeof (*addr_));
        memcpy (addr_, out_addr, out_addrlen);
        *addr_len_ = out_addrlen;
        return 0;
    }

    //  There's no such interface name. Assume literal address.
    memset (&req, 0, sizeof (req));

    //  We only support IPv4 addresses for now.
    req.ai_family = AF_INET;

    //  Arbitrary, not used in the output, but avoids duplicate results.
    req.ai_socktype = SOCK_STREAM;

    //  Restrict hostname/service to literals to avoid any DNS lookups or
    //  service-name irregularity due to indeterminate socktype.
    req.ai_flags = AI_PASSIVE | AI_NUMERICHOST | AI_NUMERICSERV;

    //  Resolve the literal address. Some of the error info is lost in case
    //  of error, however, there's no way to report EAI errors via errno.
    sprintf(service,"%d",port);
    rc = getaddrinfo (ip_address, service, &req, &res);
    if (rc)
    {
        errno = ENODEV;
        return -1;
    }

    //  Use the first result.
    net_assert ((size_t) (res->ai_addrlen) <= sizeof (*addr_));
    memcpy (addr_, res->ai_addr, res->ai_addrlen);
    *addr_len_ = (socklen_t)res->ai_addrlen;

    //  Cleanup getaddrinfo after copying the possibly referenced result.
    if (res)
        freeaddrinfo (res);

    return 0;
}

#ifdef _WIN32

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



//  Start listening on the interface.
fd_t  net_listen (const char *ip_addr,unsigned short port,int backlog_)
{
    fd_t fd_ = retired_fd;
    int flag = 1;
    u_long uflag = 1;
    struct sockaddr_storage addr;
    socklen_t addr_len;
    int rc;
    socket_init();
    //  Convert the interface into sockaddr_in structure.
    rc = resolve_ip_interface (&addr, &addr_len, ip_addr,port);
    if (rc != 0)
        return fd_;

    //  Create a listening socket.
    fd_ = socket (addr.ss_family, SOCK_STREAM, IPPROTO_TCP);

    if (fd_ == INVALID_SOCKET)
        return fd_;
    //  Allow reusing of the address.

    rc = setsockopt (fd_, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                     (const char*) &flag, sizeof (int));
    wsa_assert (rc != SOCKET_ERROR);

    //  Bind the socket to the network interface and port.
    rc = bind (fd_, (struct sockaddr*) &addr, addr_len);
    if (rc == SOCKET_ERROR)
        return retired_fd;

    //  Listen for incomming connections.
    rc = listen (fd_, backlog_);
    if (rc == SOCKET_ERROR)
        return retired_fd;

    return fd_;
}

//  Accept the new connection. Returns the file descriptor of the
//  newly created connection. The function may return retired_fd
//  if the connection was dropped while waiting in the listen backlog.
fd_t net_accept (fd_t fd_)
{
    fd_t sock;
    int rc;
    int flag = 1;
    unsigned long argp = 1;
    //struct sockaddr sockaddr_ ;
    //int len ;
    //int error ;
    net_assert (fd_ != retired_fd);
    //  Accept one incoming connection.
    sock = accept (fd_, NULL, NULL);

    if (sock == INVALID_SOCKET &&
            (WSAGetLastError () == WSAEWOULDBLOCK ||
             WSAGetLastError () == WSAECONNRESET))
    {
        return retired_fd;
    }
    net_assert (sock != INVALID_SOCKET);
    // Set to non-blocking mode.
    rc = ioctlsocket (sock, FIONBIO, &argp);
    wsa_assert (rc != SOCKET_ERROR);

    //  Disable Nagle's algorithm.
    rc = setsockopt (sock, IPPROTO_TCP, TCP_NODELAY, (char*) &flag,sizeof (int));
    wsa_assert (rc != SOCKET_ERROR);
    return sock;
}
#else

session_t* sock_listen (session_t* session,const char *ip_addr,unsigned short port,int backlog_)
{
    //  Resolve the sockaddr to bind to.
    fd_t fd_ = retired_fd;
    int flag = 1;
    socklen_t addr_len;
    int rc = resolve_ip_interface (&addr, &addr_len,ip_addr,port);
    session->fd =fd_;
    session->rcv_callback = s_rcv_callback;
    session->event_callback = s_event_callback;
    if (rc != 0)
        return session;

    //  Create a listening socket.
    fd_ = socket (addr.ss_family, SOCK_STREAM, IPPROTO_TCP);
    if (fd_ == -1)
    {
        remove_session(session->id);
        return session;
    }
    session->fd=fd_;
    //  Allow reusing of the address.
    rc = setsockopt (fd_, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof (int));
    errno_assert (rc == 0);

    //  Set the non-blocking flag.
    //flag = fcntl (fd_, F_GETFL, 0);
    //if (flag == -1)
    //    flag = 0;
    //rc = fcntl (s, F_SETFL, flag | O_NONBLOCK);
    //assert (rc != -1);

    //  Bind the socket to the network interface and port.
    rc = bind (fd_, (struct sockaddr*) &addr, addr_len);
    if (rc != 0)
    {
        int err = errno;
        session->fd =retired_fd;
        if (close (fd_) != 0)
            return session;
        errno = err;
        return session;
    }

    //  Listen for incomming connections.
    rc = listen (fd_, backlog_);
    if (rc != 0)
    {
        int err = errno;
        session->fd =retired_fd;
        if (close (fd_) != 0)
            return session;
        errno = err;
        return session;
    }

    return session;
}

static void set_keepalive(int fd, int keep_alive, int keep_idle, int keep_interval, int keep_count)
{
    if(keep_alive)
    {
        if(setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keep_alive, sizeof(keep_alive)) == -1)
            errno_assert(0);
        if(setsockopt(fd, SOL_TCP, TCP_KEEPIDLE,(void *)&keep_idle,sizeof(keep_idle)) == -1)
            errno_assert(0);
        if(setsockopt(fd,SOL_TCP,TCP_KEEPINTVL,(void *)&keep_interval, sizeof(keep_interval)) == -1)
            errno_assert(0);
        if(setsockopt(fd,SOL_TCP,TCP_KEEPCNT, (void *)&keep_count,sizeof(keep_count)) == -1)
            errno_assert(0);
    }
}

session_t* sock_accept (fd_t fd_)
{
    //  Accept one incoming connection.
    fd_t sock = accept (fd_, NULL, NULL);

    session->fd = sock;
    session->session_mode = M_SERVER;
    session->rcv_callback = s_rcv_callback;
    session->event_callback = s_event_callback;

    if (sock == -1 &&
            (errno == EAGAIN || errno == EWOULDBLOCK ||
             errno == EINTR || errno == ECONNABORTED))
    {
        session->fd = retired_fd;
        return session;
    }

    errno_assert (sock != -1);


    struct sockaddr *sa = (struct sockaddr*) &addr;
    if (AF_UNIX != sa->sa_family)
    {
        //  Disable Nagle's algorithm.
        int flag = 1;
        int rc = setsockopt (sock, IPPROTO_TCP, TCP_NODELAY, (char*) &flag,
                             sizeof (int));
        errno_assert (rc == 0);

    }

    return session;
}

#endif