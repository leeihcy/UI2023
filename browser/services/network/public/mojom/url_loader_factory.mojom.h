#ifndef SERVICES_NETWORK_PUBLIC_MOJOM_URL_LOADER_FACTORY_MOJOM_H_
#define SERVICES_NETWORK_PUBLIC_MOJOM_URL_LOADER_FACTORY_MOJOM_H_

namespace network {
class ResourceRequest;
}

namespace network::mojom {

class URLLoaderFactory {
public:
  virtual void CreateLoaderAndStart(ResourceRequest& resource_request) = 0;
};
}

#endif