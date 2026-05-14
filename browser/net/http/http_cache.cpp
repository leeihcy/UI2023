#include "net/http/http_cache.h"

namespace  net {

HttpCache::HttpCache(std::unique_ptr<HttpTransactionFactory> network_layer)
    : m_network_layer(std::move(network_layer)) {}

std::unique_ptr<HttpTransaction> HttpCache::CreateTransaction() {

 auto new_transaction =
      std::make_unique<HttpCache::Transaction>(/*priority, */this);

  return new_transaction;
}

HttpNetworkSession* HttpCache::GetSession() {
  return m_network_layer->GetSession();
}

int HttpCache::Transaction::Start(const HttpRequestInfo* request) {
  initial_request_ = request;
  request_ = initial_request_;

  // TODO: Load Cache

  DoSendRequest();

  return 0;
}

void HttpCache::Transaction::DoSendRequest() {
   m_network_trans = m_cache->m_network_layer->CreateTransaction(/*priority_*/);

   m_network_trans->Start(request_/*, io_callback_, net_log_*/);
}

}