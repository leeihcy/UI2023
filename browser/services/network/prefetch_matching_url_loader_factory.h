#ifndef SERVICES_NETWORK_PREFETCH_MATCHING_URL_LOADER_FACTORY_H_
#define SERVICES_NETWORK_PREFETCH_MATCHING_URL_LOADER_FACTORY_H_

#include "services/network/cors/cors_url_loader_factory.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"

namespace network {

//
// 用于处理预加载（prefetch）资源的缓存匹配与复用。
//
// 当页面已经通过 <link rel="prefetch"> 或 HTTP 头部预取（prefetch）了某些资源后，
// 后续实际请求这些资源时，该类负责将实际请求“匹配”到之前预取并缓存的资源上，
// 避免重复网络请求，从而加速页面加载。
//
// 它实际上是 URLLoaderFactory 的一个包装器（wrapper），
// 内部维护了一个“待匹配的预取请求”的映射表。
//
class PrefetchMatchingURLLoaderFactory : public mojom::URLLoaderFactory
{
public:
  PrefetchMatchingURLLoaderFactory();
  ~PrefetchMatchingURLLoaderFactory();
  
  void CreateLoaderAndStart() override;

private:
  const std::unique_ptr<cors::CorsURLLoaderFactory> next_;

  // const raw_ptr<PrefetchCache> cache_;
};

}

#endif