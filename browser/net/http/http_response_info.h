#ifndef NET_HTTP_HTTP_RESPONSE_INFO_H_
#define NET_HTTP_HTTP_RESPONSE_INFO_H_

#include "net/http/http_response_headers.h"
#include "net/http/http_connection_info.h"

namespace net {

class HttpResponseInfo {
public:

  // The parsed response headers and status line.
  std::shared_ptr<HttpResponseHeaders> headers;

  // The type of connection used for this response.
  HttpConnectionInfo connection_info = HttpConnectionInfo::kUNKNOWN;

};

}

#endif