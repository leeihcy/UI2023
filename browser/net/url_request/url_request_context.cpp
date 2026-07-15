#include "net/url_request/url_request_context.h"
#include "net/http/http_cache.h"
#include "net/http/http_network_layer.h"

namespace net {

URLRequestContext::URLRequestContext() {
  m_job_factory = std::make_unique<URLRequestJobFactory>();

  m_http_network_session = std::make_unique<HttpNetworkSession>();
    
  auto network_layer =
    std::make_unique<HttpNetworkLayer>(http_network_session());

  m_http_transaction_factory = std::make_unique<HttpCache>(
        std::move(network_layer)
        //,  std::move(http_cache_backend),
        // std::move(file_operations)
  );

  // auto client_socket_factory =
  //     std::make_unique<NetworkBindingClientSocketFactory>(bound_network_);
  // context->set_client_socket_factory(std::move(client_socket_factory));
}

std::unique_ptr<URLRequest> URLRequestContext::CreateRequest(GURL url, URLRequest::Delegate* delegate) {
  return std::make_unique<URLRequest>(url, delegate, this);
}

}