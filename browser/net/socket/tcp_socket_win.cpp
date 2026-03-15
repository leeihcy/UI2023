#include "net/socket/tcp_socket_win.h"

namespace net {

int TCPSocketWin::Open(AddressFamily family) {
  // DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  // DCHECK_EQ(socket_, INVALID_SOCKET);

  // socket_ = CreatePlatformSocket(ConvertAddressFamily(family), SOCK_STREAM,
  //                                IPPROTO_TCP);
  // int os_error = WSAGetLastError();
  // if (socket_ == INVALID_SOCKET) {
  //   // PLOG(ERROR) << "CreatePlatformSocket() returned an error";
  //   return MapSystemError(os_error);
  // }

  // if (!SetNonBlockingAndGetError(socket_, &os_error)) {
  //   int result = MapSystemError(os_error);
  //   Close();
  //   return result;
  // }

  // return OK;
  return 0;
}


}