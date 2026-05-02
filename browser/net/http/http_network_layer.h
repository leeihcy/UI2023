#ifndef NET_HTTP_HTTP_NETWORK_LAYER_H_
#define NET_HTTP_HTTP_NETWORK_LAYER_H_

#include "net/http/http_transaction_factory.h"

namespace net {

class HttpNetworkLayer : public HttpTransactionFactory {
public:
   explicit HttpNetworkLayer(HttpNetworkSession* session);

   std::unique_ptr<HttpTransaction> CreateTransaction(
      /*RequestPriority priority*/) override;

   HttpNetworkSession* GetSession() override { return m_session; }

private:
   HttpNetworkSession* m_session;
};

}

#endif