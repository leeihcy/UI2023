#ifndef NET_HTTP_HTTP_NETWORK_LAYER_H_
#define NET_HTTP_HTTP_NETWORK_LAYER_H_

#include "net/http/http_transaction_factory.h"

namespace net {

class HttpNetworkLayer : public HttpTransactionFactory {
public:
   std::unique_ptr<HttpTransaction> CreateTransaction(
      /*RequestPriority priority*/) override;
};

}

#endif