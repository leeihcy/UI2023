#ifndef SERVICES_NETWORK_CORS_CORSURLLOADER_H_
#define SERVICES_NETWORK_CORS_CORSURLLOADER_H_

#include <memory>
#include "services/network/public/mojom/url_loader_factory.mojom.h"

namespace network {
class NetworkContext;
class URLLoaderFactory;

// 负责执行CORS 算法。在请求发起前和收到响应后，检查是否符合 CORS 规则，只允许安全的跨域请求通过
class CorsURLLoader {
public:
  CorsURLLoader(URLLoaderFactory* factory);

  void Start();
  void SetCorsFlagIfNeeded();
  bool HasSpecialAccessToDestination() const;

  void StartRequest();
  void StartNetworkRequest();

private:
  bool fetch_cors_flag_ = false;
  URLLoaderFactory* m_network_loader_factory = nullptr;
};

}

#endif