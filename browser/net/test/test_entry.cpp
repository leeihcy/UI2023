#include "net/base/net_export.h"
#include <iostream>
#include <memory>

#include "net/socket/tcp_client_socket.h"
#include "services/network/cors/cors_url_loader.h"
#include "services/network/public/cpp/simple_url_loader.h"

namespace net {

void CorsUrlLoaderTest() {
  network::CorsURLLoader cors_url_loader;
  cors_url_loader.Start();
}

void SimpleUrlLoaderTest() {
  auto resource_request = std::make_unique<network::ResourceRequest>();
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