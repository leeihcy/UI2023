#ifndef SERVICES_NETWORK_PUBLIC_MOJOM_URL_LOADER_FACTORY_MOJOM_H_
#define SERVICES_NETWORK_PUBLIC_MOJOM_URL_LOADER_FACTORY_MOJOM_H_

#include "services/network/public/mojom/url_loader.mojom.h"

namespace network {
class ResourceRequest;
}

namespace network::mojom {

class URLLoaderFactory {
public:
  virtual void CreateLoaderAndStart(ResourceRequest &resource_request,
                                    mojom::URLLoaderClient *client) = 0;
};
} // namespace network::mojom

#endif