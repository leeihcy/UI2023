#include "services/network/url_loader_factory.h"

#include <memory>
#include "services/network/public/cpp/resource_request.h"
#include "services/network/url_loader.h"

namespace network {


URLLoaderFactory& URLLoaderFactory::GetInstance() {
  static URLLoaderFactory s;
  return s;
}

void URLLoaderFactory::CreateLoaderAndStartWithSyncClient() {
  auto loader = std::make_unique<URLLoader>();
  // cors_url_loader_factory_->OnURLLoaderCreated(std::move(loader));
}

void URLLoaderFactory::CreateLoaderAndStart(ResourceRequest& resource_request) {
    
}

}