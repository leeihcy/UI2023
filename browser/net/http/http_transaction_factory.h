
#ifndef NET_HTTP_HTTP_TRANSACTION_FACTORY_H_
#define NET_HTTP_HTTP_TRANSACTION_FACTORY_H_

#include <memory>

#include "net/http/http_transaction.h"

namespace net {

class HttpTransactionFactory {
 public:
  virtual ~HttpTransactionFactory() = default;

  // Creates a HttpTransaction object.
  virtual std::unique_ptr<HttpTransaction> CreateTransaction(
      /*RequestPriority priority*/) = 0;

  // // Returns the associated cache if any (may be NULL).
  // virtual HttpCache* GetCache() = 0;

  // // Returns the associated HttpNetworkSession used by new transactions.
  // virtual HttpNetworkSession* GetSession() = 0;
};

}

#endif