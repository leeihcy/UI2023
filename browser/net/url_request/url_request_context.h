#ifndef NET_URL_REQUEST_URL_REQUEST_CONTEXT_H_
#define NET_URL_REQUEST_URL_REQUEST_CONTEXT_H_

#include <memory>
#include "net/url_request/url_request.h"

namespace net {

class URLRequestContext {
public:
  std::unique_ptr<URLRequest> CreateRequest();
};

}

#endif