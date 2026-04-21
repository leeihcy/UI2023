#include "services/network/url_loader_factory.h"

#include <memory>
#include "services/network/public/cpp/resource_request.h"
#include "services/network/url_loader.h"
#include "services/network/network_context.h"

namespace network {

URLLoaderFactory::URLLoaderFactory(NetworkContext* context) : m_network_context(context) {

}

// URLLoaderFactory& URLLoaderFactory::GetInstance() {
//   static URLLoaderFactory s;
//   return s;
// }

net::URLRequestContext* URLLoaderFactory::GetUrlRequestContext() const {
  return m_network_context->url_request_context();
}

void URLLoaderFactory::CreateLoaderAndStartWithSyncClient(ResourceRequest& resource_request) {
  auto loader = std::make_unique<URLLoader>(*this, resource_request);
}

void URLLoaderFactory::CreateLoaderAndStart(ResourceRequest& resource_request) {
    CreateLoaderAndStartWithSyncClient(resource_request);
}

}