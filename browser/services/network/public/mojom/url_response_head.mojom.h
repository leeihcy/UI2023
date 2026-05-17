#ifndef SERVICES_NETWORK_PUBLIC_MOJOM_URL_RESPONSE_HEAD_MOJOM_H_
#define SERVICES_NETWORK_PUBLIC_MOJOM_URL_RESPONSE_HEAD_MOJOM_H_

#include <memory>
#include "net/http/http_response_headers.h"

namespace network::mojom {
  class  URLResponseHead {
  public:
    std::shared_ptr<::net::HttpResponseHeaders> headers;
  };
}


#endif