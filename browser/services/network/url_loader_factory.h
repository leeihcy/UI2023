#ifndef SERVICES_NETWORK_URL_LOADER_FACTORY_H_
#define SERVICES_NETWORK_URL_LOADER_FACTORY_H_

namespace network {
class ResourceRequest;

class URLLoaderFactory {
public:
  static URLLoaderFactory& GetInstance();
  void CreateLoaderAndStartWithSyncClient();
  
  void CreateLoaderAndStart(ResourceRequest& resource_request);
};

}

#endif