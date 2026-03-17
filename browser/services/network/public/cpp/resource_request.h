#ifndef SERVICES_NETWORK_PUBLIC_CPP_RESOURCE_REQUEST_H_
#define SERVICES_NETWORK_PUBLIC_CPP_RESOURCE_REQUEST_H_

#include <memory>
#include "url/gurl.h"
#include "net/http/http_request_header.h"

namespace network {

//
// 对外使用接口，不要直接调用UrlLoader。
//
class ResourceRequest {
public:

public:
  std::string method = net::HttpRequestHeaders::kGetMethod;
  GURL url;
  
};

}

#endif