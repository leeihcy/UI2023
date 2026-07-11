#include "net/socket/stream_socket_handle.h"

namespace net {

void StreamSocketHandle::SetSocket(std::unique_ptr<StreamSocket> socket) {
  socket_ = std::move(socket);
}

}