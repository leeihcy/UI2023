#include "net/http/http_network_transaction.h"

namespace net {

int HttpNetworkTransaction::Start() {
  DoCreateStream();
  return 0;
}

void HttpNetworkTransaction::DoCreateStream() {
  stream_request_ = session_->http_stream_factory()->RequestStream();
}
}