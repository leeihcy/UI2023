
#ifndef NET_HTTP_HTTP_NETWORK_TRANSACTION_H_
#define NET_HTTP_HTTP_NETWORK_TRANSACTION_H_

#include "net/base/completion_once_callback.h"
#include "net/http/http_stream_request.h"
#include "net/http/http_transaction.h"
#include "net/http/http_stream.h"
#include "net/http/http_request_headers.h"
#include "net/http/http_response_headers.h"
#include "net/http/http_response_info.h"
#include <memory>

namespace net {
class IOBuffer;
class HttpNetworkSession;

class HttpNetworkTransaction : public HttpTransaction,
                               public HttpStreamRequest::Delegate {
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
  HttpNetworkTransaction(HttpNetworkSession* session);
  int Start(const HttpRequestInfo* request_info, CompletionOnceCallback callback) override;
  void SetResponseHeadersCallback(ResponseHeadersCallback callback) override;

protected:
  void OnStreamReady(/*const ProxyInfo& used_proxy_info,*/
                     std::unique_ptr<HttpStream> stream) override;
  int Read(IOBuffer* buf,
           int buf_len,
           CompletionOnceCallback callback) override;
private:
  void DoCreateStream();
  void DoInitStream();
  void DoBuildRequest();
  void DoConnectedCallback();
  int BuildRequestHeaders(bool using_http_proxy_without_tunnel);
  int DoSendRequest();
  int DoSendRequestComplete(int result);
  int DoReadHeaders();
  int DoReadHeadersComplete(int result);
  int DoReadBody();
  int DoReadBodyComplete(int result);

  void DoCallback(int rv);
  void OnIOComplete(int result);

private:
  std::unique_ptr<HttpStreamRequest> stream_request_;

  HttpNetworkSession* m_session;
  const HttpRequestInfo* request_ = nullptr;

  std::unique_ptr<HttpStream> stream_;

  // RequestHeadersCallback request_headers_callback_;
  HttpRequestHeaders request_headers_;
  ResponseHeadersCallback response_headers_callback_;

  CompletionRepeatingCallback io_callback_;
  CompletionOnceCallback callback_;

  HttpResponseInfo response_;

  // User buffer and length passed to the Read method.
  IOBuffer* read_buf_ = nullptr;
  int read_buf_len_ = 0;
};

} // namespace net

#endif