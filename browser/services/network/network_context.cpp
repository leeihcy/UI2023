#include "services/network/network_context.h"
#include "services/network/prefetch_matching_url_loader_factory.h"

namespace network {

mojom::URLLoaderFactory* NetworkContext::CreateURLLoaderFactory() {
  auto factory = std::make_unique<PrefetchMatchingURLLoaderFactory>(this);
  m_url_loader_factories.push_back(std::move(factory));

  return m_url_loader_factories.back().get();
}


}