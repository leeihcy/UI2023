#ifndef SERVICES_NETWORK_PUBLIC_CPP_RESOURCE_REQUEST_H_
#define SERVICES_NETWORK_PUBLIC_CPP_RESOURCE_REQUEST_H_

#include <memory>
#include <optional>
#include "url/gurl.h"
#include "url/origin.h"
#include "net/http/http_request_headers.h"
#include "services/network/public/cpp/cors/cors.h"

namespace network {

//
// 对外使用接口，不要直接调用UrlLoader。
//
class ResourceRequest {
public:

public:
  std::string method = net::HttpRequestHeaders::kGetMethod;
  GURL url;
  std::optional<url::Origin> request_initiator;

  mojom::CredentialsMode credentials_mode = mojom::CredentialsMode::kInclude;

};

}

#endif