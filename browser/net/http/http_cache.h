#ifndef NET_HTTP_HTTP_CACHE_H_
#define NET_HTTP_HTTP_CACHE_H_
#include "net/http/http_transaction_factory.h"

namespace net {

class HttpCache : public HttpTransactionFactory {
public:
  class Transaction : public HttpTransaction {
  public:
    Transaction(HttpCache* cache) : m_cache(cache) {}
    int Start() override;

  private:
    void DoSendRequest();
    HttpCache* m_cache;

    std::unique_ptr<HttpTransaction> m_network_trans;
  };

public:
  HttpCache(std::unique_ptr<HttpTransactionFactory> network_layer);
  std::unique_ptr<HttpTransaction> CreateTransaction() override;

public:
  std::unique_ptr<HttpTransactionFactory> m_network_layer;
};

}

#endif