
#ifndef NET_HTTP_HTTP_NETWORK_TRANSACTION_H_
#define NET_HTTP_HTTP_NETWORK_TRANSACTION_H_

#include "net/http/http_stream_request.h"
#include "net/http/http_transaction.h"
#include <memory>


namespace net {

class HttpNetworkTransaction : public HttpTransaction {
  enum State {
    STATE_CREATE_STREAM,
    STATE_CREATE_STREAM_COMPLETE,
    STATE_INIT_STREAM,
    STATE_INIT_STREAM_COMPLETE,
    STATE_CONNECTED_CALLBACK,
    STATE_CONNECTED_CALLBACK_COMPLETE,
    STATE_GENERATE_PROXY_AUTH_TOKEN,
    STATE_GENERATE_PROXY_AUTH_TOKEN_COMPLETE,
    STATE_GENERATE_SERVER_AUTH_TOKEN,
    STATE_GENERATE_SERVER_AUTH_TOKEN_COMPLETE,
    STATE_INIT_REQUEST_BODY,
    STATE_INIT_REQUEST_BODY_COMPLETE,
    STATE_BUILD_REQUEST,
    STATE_BUILD_REQUEST_COMPLETE,
    STATE_SEND_REQUEST,
    STATE_SEND_REQUEST_COMPLETE,
    STATE_READ_HEADERS,
    STATE_READ_HEADERS_COMPLETE,
    STATE_READ_BODY,
    STATE_READ_BODY_COMPLETE,
    STATE_DRAIN_BODY_FOR_AUTH_RESTART,
    STATE_DRAIN_BODY_FOR_AUTH_RESTART_COMPLETE,
    STATE_NONE
  };

public:
  int Start() override;

private:
  void DoCreateStream();

private:
  std::unique_ptr<HttpStreamRequest> stream_request_;
};

} // namespace net

#endif