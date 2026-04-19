#ifndef _SERVICES_NETWORK_NETWORK_SERVICE_H_
#define _SERVICES_NETWORK_NETWORK_SERVICE_H_

#include <vector>
#include <memory>

namespace network {
class NetworkContext;

class NetworkService {
public:
  static NetworkService& GetInstance();

  NetworkContext* CreateNetworkContext();

private:
  std::vector<std::unique_ptr<NetworkContext> > m_network_contexts;
};

}

#endif