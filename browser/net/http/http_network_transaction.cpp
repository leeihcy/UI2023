#include "net/http/http_network_transaction.h"
#include "net/http/http_network_session.h"

namespace net {

int HttpNetworkTransaction::Start(const HttpRequestInfo* request_info) {
  request_ = request_info;

  DoCreateStream();
  DoInitStream();
  return 0;
}

void HttpNetworkTransaction::DoCreateStream() {
  stream_request_ = m_session->http_stream_factory()->RequestStream(*request_);
}

void HttpNetworkTransaction::DoInitStream() {
  // stream_request_->InitializeStream();
}

}