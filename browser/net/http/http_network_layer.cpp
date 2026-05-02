#include "net/http/http_network_layer.h"
#include "net/http/http_network_transaction.h"

namespace net {

HttpNetworkLayer::HttpNetworkLayer(HttpNetworkSession* session) : m_session(session) {

}

std::unique_ptr<HttpTransaction> HttpNetworkLayer::CreateTransaction(
    /*RequestPriority priority*/) {
  return std::make_unique<HttpNetworkTransaction>(/*priority,*/ GetSession());
}

}