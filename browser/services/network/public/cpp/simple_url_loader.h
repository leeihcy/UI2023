#ifndef SERVICES_NETWORK_PUBLIC_CPP_SIMPLEURLLOADER_H_
#define SERVICES_NETWORK_PUBLIC_CPP_SIMPLEURLLOADER_H_

#include <memory>
#include "services/network/public/cpp/resource_request.h"
#include "services/network/url_loader_factory.h"

namespace network {

//
// 对外使用接口，不要直接调用UrlLoader。
//
class SimpleURLLoader {
public:
  static std::unique_ptr<SimpleURLLoader> Create(
    std::unique_ptr<ResourceRequest> resource_request
  );

  SimpleURLLoader(std::unique_ptr<ResourceRequest> resource_request);
  void Start(URLLoaderFactory* url_loader_factory);
  void StartRequest(URLLoaderFactory* url_loader_factory);

private:
  std::unique_ptr<ResourceRequest> m_resource_request;
};

}

#endif