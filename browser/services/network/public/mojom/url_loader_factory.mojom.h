#ifndef SERVICES_NETWORK_PUBLIC_MOJOM_URL_LOADER_FACTORY_MOJOM_H_
#define SERVICES_NETWORK_PUBLIC_MOJOM_URL_LOADER_FACTORY_MOJOM_H_

namespace network::mojom {

class URLLoaderFactory {
public:
  virtual void CreateLoaderAndStart() = 0;
};
}

#endif