#include "net/url_request/url_request_context.h"
#include "net/http/http_cache.h"
#include "net/http/http_network_layer.h"

namespace net {

URLRequestContext::URLRequestContext() {
  m_job_factory = std::make_unique<URLRequestJobFactory>();

  auto network_layer =
    std::make_unique<HttpNetworkLayer>(/*context->http_network_session()*/);

  m_http_transaction_factory = std::make_unique<HttpCache>(
        std::move(network_layer)
        //,  std::move(http_cache_backend),
        // std::move(file_operations)
  );
}

std::unique_ptr<URLRequest> URLRequestContext::CreateRequest(GURL url) {
  return std::make_unique<URLRequest>(url, this);
}

}