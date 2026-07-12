#include "net/http/http_cache.h"

#include <assert.h>

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

HttpCache::Transaction::Transaction(HttpCache* cache) : m_cache(cache) {
  io_callback_ = std::bind(&Transaction::OnIOComplete, this, std::placeholders::_1);
}

int HttpCache::Transaction::Start(const HttpRequestInfo *request,
                                  CompletionOnceCallback callback) {
  initial_request_ = request;
  request_ = initial_request_;

  // TODO: Load Cache

  int rv = DoSendRequest();
  
  // Setting this here allows us to check for the existence of a callback_ to
  // determine if we are still inside Start.
  // if (rv == ERR_IO_PENDING) {
  //   callback_ = std::move(callback);
  // }
  return 0;
}

void HttpCache::Transaction::SetResponseHeadersCallback(
    ResponseHeadersCallback callback) {
  response_headers_callback_ = std::move(callback);
}

int HttpCache::Transaction::DoSendRequest() {
   m_network_trans = m_cache->m_network_layer->CreateTransaction(/*priority_*/);

   m_network_trans->SetResponseHeadersCallback(response_headers_callback_);

   return m_network_trans->Start(request_, io_callback_);
}

int HttpCache::Transaction::Read(IOBuffer* buf,
                                 int buf_len,
                                 CompletionOnceCallback callback) {
  return m_network_trans->Read(buf, buf_len, callback);
}


void HttpCache::Transaction::OnIOComplete(int result) {
  assert(false);
}

}