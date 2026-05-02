#include "net/http/http_network_transaction.h"
#include "net/http/http_network_session.h"

namespace net {

int HttpNetworkTransaction::Start() {
  DoCreateStream();
  DoInitStream();
  return 0;
}

void HttpNetworkTransaction::DoCreateStream() {
  stream_request_ = m_session->http_stream_factory()->RequestStream();
}

void HttpNetworkTransaction::DoInitStream() {
  // stream_request_->InitializeStream();
}

}