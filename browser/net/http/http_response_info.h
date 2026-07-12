#ifndef NET_HTTP_HTTP_RESPONSE_INFO_H_
#define NET_HTTP_HTTP_RESPONSE_INFO_H_

#include "net/http/http_response_headers.h"

namespace net {

class HttpResponseInfo {
public:

  // The parsed response headers and status line.
  HttpResponseHeaders* headers;

};

}

#endif