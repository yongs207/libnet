#include "net_err.h"
#ifdef _WIN32
const char * wsa_error()
{
  int no = WSAGetLastError ();
  //  TODO: This is not a generic way to handle this...
  if (no == WSAEWOULDBLOCK)
    return NULL;

  return wsa_error_no (no);
}

const char * wsa_error_no( int no_ )
{
  return
    (no_ == WSABASEERR) ?
    "No Error" : 
  (no_ == WSAEINTR) ?
    "Interrupted system call" : 
  (no_ == WSAEBADF) ?
    "Bad file number" : 
  (no_ == WSAEACCES) ?
    "Permission denied" : 
  (no_ == WSAEFAULT) ?
    "Bad address" : 
  (no_ == WSAEINVAL) ?
    "Invalid argument" : 
  (no_ == WSAEMFILE) ?
    "Too many open files" : 
  (no_ == WSAEWOULDBLOCK) ?
    "Operation would block" : 
  (no_ == WSAEINPROGRESS) ?
    "Operation now in progress" : 
  (no_ == WSAEALREADY) ?
    "Operation already in progress" : 
  (no_ == WSAENOTSOCK) ?
    "Socket operation on non-socket" : 
  (no_ == WSAEDESTADDRREQ) ?
    "Destination address required" : 
  (no_ == WSAEMSGSIZE) ?
    "Message too long" : 
  (no_ == WSAEPROTOTYPE) ?
    "Protocol wrong type for socket" : 
  (no_ == WSAENOPROTOOPT) ?
    "Bad protocol option" : 
  (no_ == WSAEPROTONOSUPPORT) ?
    "Protocol not supported" : 
  (no_ == WSAESOCKTNOSUPPORT) ?
    "Socket type not supported" : 
  (no_ == WSAEOPNOTSUPP) ?
    "Operation not supported on socket" : 
  (no_ == WSAEPFNOSUPPORT) ?
    "Protocol family not supported" : 
  (no_ == WSAEAFNOSUPPORT) ?
    "Address family not supported by protocol family" : 
  (no_ == WSAEADDRINUSE) ?
    "Address already in use" : 
  (no_ == WSAEADDRNOTAVAIL) ?
    "Can't assign requested address" : 
  (no_ == WSAENETDOWN) ?
    "Network is down" : 
  (no_ == WSAENETUNREACH) ?
    "Network is unreachable" : 
  (no_ == WSAENETRESET) ?
    "Net dropped connection or reset" : 
  (no_ == WSAECONNABORTED) ?
    "Software caused connection abort" : 
  (no_ == WSAECONNRESET) ?
    "Connection reset by peer" : 
  (no_ == WSAENOBUFS) ?
    "No buffer space available" : 
  (no_ == WSAEISCONN) ?
    "Socket is already connected" : 
  (no_ == WSAENOTCONN) ?
    "Socket is not connected" : 
  (no_ == WSAESHUTDOWN) ?
    "Can't send after socket shutdown" : 
  (no_ == WSAETOOMANYREFS) ?
    "Too many references can't splice" : 
  (no_ == WSAETIMEDOUT) ?
    "Connection timed out" : 
  (no_ == WSAECONNREFUSED) ?
    "Connection refused" : 
  (no_ == WSAELOOP) ?
    "Too many levels of symbolic links" : 
  (no_ == WSAENAMETOOLONG) ?
    "File name too long" : 
  (no_ == WSAEHOSTDOWN) ?
    "Host is down" : 
  (no_ == WSAEHOSTUNREACH) ?
    "No Route to Host" : 
  (no_ == WSAENOTEMPTY) ?
    "Directory not empty" : 
  (no_ == WSAEPROCLIM) ?
    "Too many processes" : 
  (no_ == WSAEUSERS) ?
    "Too many users" : 
  (no_ == WSAEDQUOT) ?
    "Disc Quota Exceeded" : 
  (no_ == WSAESTALE) ?
    "Stale NFS file handle" : 
  (no_ == WSAEREMOTE) ?
    "Too many levels of remote in path" : 
  (no_ == WSASYSNOTREADY) ?
    "Network SubSystem is unavailable" : 
  (no_ == WSAVERNOTSUPPORTED) ?
    "WINSOCK DLL Version out of range" : 
  (no_ == WSANOTINITIALISED) ?
    "Successful WSASTARTUP not yet performed" : 
  (no_ == WSAHOST_NOT_FOUND) ?
    "Host not found" : 
  (no_ == WSATRY_AGAIN) ?
    "Non-Authoritative Host not found" : 
  (no_ == WSANO_RECOVERY) ?
    "Non-Recoverable errors: FORMERR REFUSED NOTIMP" : 
  (no_ == WSANO_DATA) ?
    "Valid name no data record of requested" :
  (no_ == 0)?
    "remote client normal close":
  "error not defined"; 
}

void win_error( char *buffer_, size_t buffer_size_ )
{
  DWORD errcode = GetLastError ();
  DWORD rc = FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errcode, MAKELANGID(LANG_NEUTRAL,
    SUBLANG_DEFAULT), buffer_, (DWORD) buffer_size_, NULL );
  net_assert (rc);
}

void wsa_error_to_errno()
{
  int errcode = WSAGetLastError ();
  switch (errcode) {
  case WSAEINPROGRESS:
    errno = EAGAIN;
    return;
  case WSAEBADF:
    errno = EBADF;
    return;
  case WSAECONNREFUSED:
    errno = ECONNREFUSED;
    return;
  case WSAEINVAL:
    errno = EINVAL;
    return;
  case WSAEMFILE:
    errno = EMFILE;
    return;
  case WSAEFAULT:
    errno = EFAULT;
    return;
  case WSAEPROTONOSUPPORT:
    errno = EPROTONOSUPPORT;
    return;
  case WSAENOBUFS:
    errno = ENOBUFS;
    return;
  case WSAENETDOWN:
    errno = ENETDOWN;
    return;
  case WSAEADDRINUSE:
    errno = EADDRINUSE;
    return;
  case WSAEADDRNOTAVAIL:
    errno = EADDRNOTAVAIL;
    return;
  case WSAETIMEDOUT:
    errno = ETIMEDOUT;
    return ;
  default:
    wsa_assert (0);
  }
}
#endif
const char * errno_to_string( int errno_ )
{
  switch (errno_) {
#if defined _WIN32
  case ENOTSUP:
    return "Not supported";
  case EPROTONOSUPPORT:
    return "Protocol not supported";
  case ENOBUFS:
    return "No buffer space available";
  case ENETDOWN:
    return "Network is down";
  case EADDRINUSE:
    return "Address in use";
  case EADDRNOTAVAIL:
    return "Address not available";
  case ECONNREFUSED:
    return "Connection refused";
  case EINPROGRESS:
    return "Operation in progress";
#endif
  case 0:
    return "Connection reset by peer";
  default:
#if defined _MSC_VER
#pragma warning (push)
#pragma warning (disable:4996)
#endif
    return strerror (errno_);
#if defined _MSC_VER
#pragma warning (pop)
#endif
  }
}


#ifdef _WIN32

int wsa_error_to_errid (int no_)
{
  switch(no_)
  {
  case WSAECONNRESET:
    return ERR_CONNECT_CLOSE;
  case  WSABASEERR :
  case  WSAEINTR :
  case  WSAEBADF :
  case  WSAEACCES :
  case  WSAEFAULT :
  case  WSAEINVAL :
  case  WSAEMFILE :
  case  WSAEWOULDBLOCK :
  case  WSAEINPROGRESS :
  case  WSAEALREADY :
    return ERR_UNKNOWN;
  case  WSAENOTSOCK :
    return ERR_CONNECT_CLOSE;
  case  WSAEDESTADDRREQ :
  case  WSAEMSGSIZE :
  case  WSAEPROTOTYPE :
  case  WSAENOPROTOOPT :
  case  WSAEPROTONOSUPPORT :
  case  WSAESOCKTNOSUPPORT :
  case  WSAEOPNOTSUPP :
  case  WSAEPFNOSUPPORT :
  case  WSAEAFNOSUPPORT :
    return ERR_UNKNOWN;
  case  WSAEADDRINUSE :
    return ERR_BINDING_REFUSED;
  case  WSAEADDRNOTAVAIL :
  case  WSAENETDOWN :
  case  WSAENETUNREACH :
  case  WSAENETRESET :
    return ERR_UNKNOWN;
  case  WSAECONNABORTED :
    return ERR_SERVER_ABORT;
  case  WSAENOBUFS :
  case  WSAEISCONN :
  case  WSAENOTCONN :
  case  WSAESHUTDOWN :
  case  WSAETOOMANYREFS :
  case  WSAETIMEDOUT :
    return ERR_UNKNOWN;
  case  WSAECONNREFUSED :
    return ERR_CONNECT_REFUSED;
  case  WSAELOOP :
  case  WSAENAMETOOLONG :
  case  WSAEHOSTDOWN :
  case  WSAEHOSTUNREACH :
  case  WSAENOTEMPTY :
  case  WSAEPROCLIM :
  case  WSAEUSERS :
  case  WSAEDQUOT :
  case  WSAESTALE :
  case  WSAEREMOTE :
  case  WSASYSNOTREADY :
  case  WSAVERNOTSUPPORTED :
  case  WSANOTINITIALISED :
  case  WSAHOST_NOT_FOUND :
  case  WSATRY_AGAIN :
  case  WSANO_RECOVERY :
  case  WSANO_DATA :
    return ERR_UNKNOWN;
  case 0:
    return ERR_REMOTE_NORMAL_CLOSE;
  default:
    net_assert(0);
  }
  return 0;
}
#else
int errno_to_errid(int no_)
{
  switch(no_)
  {
  case EADDRINUSE:
    return ERR_BINDING_REFUSED;
  case ECONNREFUSED:
    return ERR_CONNECT_REFUSED;
  case ETIMEDOUT :
    return ERR_CONNECT_TIMEOUT;
  case ECONNRESET :
    return ERR_CONNECT_CLOSE;
  case ENOTCONN:
    return ERR_CONNECT_CLOSE;
  case 0:
    return ERR_CONNECT_CLOSE;
  }
  return ERR_UNKNOWN;
}
#endif