#ifndef NET_HTTP_HTTP_CACHE_H_
#define NET_HTTP_HTTP_CACHE_H_
#include "net/http/http_transaction_factory.h"

namespace net {

class HttpCache : public HttpTransactionFactory {
public:
  class Transaction : public HttpTransaction {

  };

public:
  std::unique_ptr<HttpTransaction> CreateTransaction() override;
};

}

#endif