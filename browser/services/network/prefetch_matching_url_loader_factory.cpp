#include "services/network/prefetch_matching_url_loader_factory.h"

namespace network {

PrefetchMatchingURLLoaderFactory::PrefetchMatchingURLLoaderFactory(NetworkContext* network_context)
    : next_(std::make_unique<cors::CorsURLLoaderFactory>(network_context)) {
}

PrefetchMatchingURLLoaderFactory::~PrefetchMatchingURLLoaderFactory() = default;

void PrefetchMatchingURLLoaderFactory::CreateLoaderAndStart(ResourceRequest& request) {
  // 1. 检查这个请示是否有缓存
  // TODO: cache
  
  // 2. 没有缓存时重新请示
  next_->CreateLoaderAndStart(request);
}

}