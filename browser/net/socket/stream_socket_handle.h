#ifndef NET_SOCKET_STREAM_SOCKET_HANDLE_H_
#define NET_SOCKET_STREAM_SOCKET_HANDLE_H_

#include <memory>
#include "net/socket/stream_socket.h"

namespace net {

class StreamSocketHandle {
public:
  StreamSocket* socket() { return socket_.get(); }
  StreamSocket* socket() const { return socket_.get(); }

  // Sets `socket` to `this`.
  void SetSocket(std::unique_ptr<StreamSocket> socket);

private:
  std::unique_ptr<StreamSocket> socket_;
};


}

#endif