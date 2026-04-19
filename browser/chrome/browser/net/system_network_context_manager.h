#ifndef CHROME_BROWSER_NET_SYSTEM_NETWORK_CONTEXT_MANAGER_H_
#define CHROME_BROWSER_NET_SYSTEM_NETWORK_CONTEXT_MANAGER_H_

class SystemNetworkContextManager {
public:
  SystemNetworkContextManager();
  static SystemNetworkContextManager& GetInstance();

  // Returns a SharedURLLoaderFactory owned by the SystemNetworkContextManager
  // that is backed by the SystemNetworkContext.
  // std::shared_ptr<network::SharedURLLoaderFactory> GetSharedURLLoaderFactory() {
  //   return shared_url_loader_factory_;
  // }

private:
  // URLLoaderFactory backed by the NetworkContext returned by GetContext(), so
  // consumers don't all need to create their own factory.
  // scoped_refptr<URLLoaderFactoryForSystem> shared_url_loader_factory_;
  // URLLoaderFactory url_loader_factory_;
};

#endif