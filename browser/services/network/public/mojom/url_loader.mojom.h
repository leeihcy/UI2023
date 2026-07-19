#ifndef SERVICES_NETWORK_PUBLIC_MOJOM_URL_LOADER_MOJOM_H_
#define SERVICES_NETWORK_PUBLIC_MOJOM_URL_LOADER_MOJOM_H_

#include <optional>
#include "services/network/public/mojom/url_response_head.mojom.h"

namespace network::mojom {

class URLLoaderClient {
public:
    virtual void OnReceiveResponse(
      ::network::mojom::URLResponseHead* head/*, 
      ::mojo::ScopedDataPipeConsumerHandle body, 
      std::optional<::mojo_base::BigBuffer> cached_metadata*/) = 0;
  
};

}

#endif