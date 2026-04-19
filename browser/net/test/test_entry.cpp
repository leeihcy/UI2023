#include "net/base/net_export.h"
#include "services/network/public/cpp/resource_request.h"
#include <iostream>
#include <memory>

#include "chrome/browser/net/system_network_context_manager.h"

#include "net/socket/tcp_client_socket.h"
#include "services/network/cors/cors_url_loader.h"
#include "services/network/network_service.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/network_context.h"

namespace net {

void CorsUrlLoaderTest() {
  network::CorsURLLoader cors_url_loader;
  cors_url_loader.Start();
}

void SimpleUrlLoaderTest() {
  auto& network_service = network::NetworkService::GetInstance();
  auto* network_context = network_service.CreateNetworkContext();

  // scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory = SystemNetworkContextManager::GetInstance().GetSharedURLLoaderFactory();
  network::mojom::URLLoaderFactory* url_loader_factory = network_context->CreateURLLoaderFactory();

  auto resource_request = std::make_unique<network::ResourceRequest>();
  resource_request->url = GURL("http://zhihu.com");
  resource_request->method = net::HttpRequestHeaders::kGetMethod;

  std::unique_ptr<network::SimpleURLLoader> loader = 
    network::SimpleURLLoader::Create(std::move(resource_request));
  loader->Start(url_loader_factory);
}

void _NetUnitTest() {
  // TCPClientSocket socket;

  CorsUrlLoaderTest();
  SimpleUrlLoaderTest();
}

NET_EXPORT void NetUnitTest() {
#if defined(NET_UNITTEST)
  std::cout << "net unit test begin!" << std::endl;
  _NetUnitTest();
  std::cout << "net unit test end!" << std::endl;
#endif
}

}