#include "error.h"
#include <stdio.h>
#include <WinSock2.h>



char* err_to_name(int err) {

  char* cErrName;
  switch (err) {
  case WSA_INVALID_HANDLE:
    cErrName = "Specified event object handle is invalid";
    break;
  case WSA_NOT_ENOUGH_MEMORY:
    cErrName = "Insufficient memory available.";
    break;
  case WSA_INVALID_PARAMETER:
    cErrName = "One or more parameters are invalid.";
    break;
  case WSA_OPERATION_ABORTED:
    cErrName = "Overlapped operation aborted.";
    break;
  case WSA_IO_INCOMPLETE:
    cErrName = "Overlapped I/O event object not in signaled state.";
    break;
  case WSA_IO_PENDING:
    cErrName = "Overlapped operations will complete later.";
    break;
  case WSAEINTR:
    cErrName = "Interrupted function call.";
    break;
  case WSAEBADF:
    cErrName = "File handle is not valid.";
    break;
  case WSAEACCES:
    cErrName = "Permission denied.";
    break;
  case WSAEFAULT:
    cErrName = "Bad address.";
    break;
  case WSAEINVAL:
    cErrName = "Invalid argument.";
    break;
  case WSAEMFILE:
    cErrName = "Too many open sockets.";
    break;
  case WSAEWOULDBLOCK:
    cErrName = "Resource temporarily unavailable.";
    break;
  case WSAEINPROGRESS:
    cErrName = "Operation now in progress.";
    break;
  case WSAEALREADY:
    cErrName = "Operation already in progress.";
    break;
  case WSAENOTSOCK:
    cErrName = "Socket operation on nonsocket.";
    break;
  case WSAEDESTADDRREQ:
    cErrName = "Destination address required.";
    break;
  case WSAEMSGSIZE:
    cErrName = "Message too long.";
    break;
  case WSAEPROTOTYPE:
    cErrName = "Protocol wrong type for socket.";
    break;
  case WSAENOPROTOOPT:
    cErrName = "Bad protocol option.";
    break;
  case WSAEPROTONOSUPPORT:
    cErrName = "Protocol not supported.";
    break;
  case WSAESOCKTNOSUPPORT:
    cErrName = "Socket type not supported.";
    break;
  case WSAEOPNOTSUPP:
    cErrName = "Operation not supported.";
    break;
  case WSAEPFNOSUPPORT:
    cErrName = "Protocol family not supported.";
    break;
  case WSAEAFNOSUPPORT:
    cErrName = "Address family not supported by protocol family.";
    break;
  case WSAEADDRINUSE:
    cErrName = "Address already in use.";
    break;
  case WSAEADDRNOTAVAIL:
    cErrName = "Cannot assign requested address.";
    break;
  case WSAENETDOWN:
    cErrName = "Network is down.";
    break;
  case WSAENETUNREACH:
    cErrName = "Network is unreachable.";
    break;
  case WSAENETRESET:
    cErrName = "Network dropped connection on reset.";
    break;
  case WSAECONNABORTED:
    cErrName = "Software caused connection abort.";
    break;
  case WSAECONNRESET:
    cErrName = "Connection reset by peer.";
    break;
  case WSAENOBUFS:
    cErrName = "No buffer space available.";
    break;
  case WSAEISCONN:
    cErrName = "Socket is already connected.";
    break;
  case WSAENOTCONN:
    cErrName = "Socket is not connected.";
    break;
  case WSAESHUTDOWN:
    cErrName = "Cannot send after socket shutdown.";
    break;
  case WSAETOOMANYREFS:
    cErrName = "Too many references.";
    break;
  case WSAETIMEDOUT:
    cErrName = "Connection timed out.";
    break;
  case WSAECONNREFUSED:
    cErrName = "Connection refused.";
    break;
  case WSAELOOP:
    cErrName = "Cannot translate name.";
    break;
  case WSAENAMETOOLONG:
    cErrName = "Name too long.";
    break;
  case WSAEHOSTDOWN:
    cErrName = "Host is down.";
    break;
  case WSAEHOSTUNREACH:
    cErrName = "No route to host.";
    break;
  case WSAENOTEMPTY:
    cErrName = "Directory not empty.";
    break;
  case WSAEPROCLIM:
    cErrName = "Too many processes.";
    break;
  case WSAEUSERS:
    cErrName = "User quota exceeded.";
    break;
  case WSANOTINITIALISED:
    cErrName = "Successful WSAStartup not yet performed.";
    break;
  case WSAHOST_NOT_FOUND:
    cErrName = "Host not found.";
    break;
  case ERR_NULL_POINTER:
    cErrName = "Passed null pointer";
    break;
  case ERR_NO_MEMORY:
    cErrName = "Memory allocation didn't work";
    break;
  default:
    cErrName = "not found error name";
    break;
  }

  return cErrName;
}