#include "chrome/browser/net/system_network_context_manager.h"


SystemNetworkContextManager::SystemNetworkContextManager() {
  // shared_url_loader_factory_ = new URLLoaderFactoryForSystem(this);
}

SystemNetworkContextManager& SystemNetworkContextManager::GetInstance() {
  static SystemNetworkContextManager s;
  return s;
}
