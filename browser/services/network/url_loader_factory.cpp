#include "services/network/url_loader_factory.h"

#include <memory>
#include "services/network/public/cpp/resource_request.h"
#include "services/network/url_loader.h"
#include "services/network/network_context.h"

namespace network {

URLLoaderFactory::URLLoaderFactory(
    NetworkContext *context,
    cors::CorsURLLoaderFactory *cors_url_loader_factory)
    : m_network_context(context),
      cors_url_loader_factory_(cors_url_loader_factory) {}

// URLLoaderFactory& URLLoaderFactory::GetInstance() {
//   static URLLoaderFactory s;
//   return s;
// }

net::URLRequestContext* URLLoaderFactory::GetUrlRequestContext() const {
  return m_network_context->url_request_context();
}

void URLLoaderFactory::CreateLoaderAndStartWithSyncClient(
  ResourceRequest& resource_request, mojom::URLLoaderClient* client) {
  auto loader = std::make_unique<URLLoader>(*this, resource_request, client);
  cors_url_loader_factory_->OnURLLoaderCreated(std::move(loader));
}

void URLLoaderFactory::CreateLoaderAndStart(ResourceRequest &resource_request,
                                            mojom::URLLoaderClient *client) {
  CreateLoaderAndStartWithSyncClient(resource_request, client);
}

} // namespace network