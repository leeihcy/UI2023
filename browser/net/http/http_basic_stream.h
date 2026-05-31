#ifndef NET_HTTP_HTTP_BASIC_STREAM_H_
#define NET_HTTP_HTTP_BASIC_STREAM_H_

#include "net/http/http_stream.h"
#include "net/socket/stream_socket_handle.h"
#include <memory>

namespace net {

class HttpBasicState {
public:
  HttpBasicState(std::unique_ptr<StreamSocketHandle> connection);

private:
  std::unique_ptr<StreamSocketHandle> connection_;
};

class HttpBasicStream : public HttpStream {
public:
  HttpBasicStream(std::unique_ptr<StreamSocketHandle> connection);

private:
  HttpBasicState state_;
};

}

#endif