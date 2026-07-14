#ifndef NET_HTTP_HTTP_BASIC_STREAM_H_
#define NET_HTTP_HTTP_BASIC_STREAM_H_

#include <memory>

#include "net/http/http_stream.h"
#include "net/socket/stream_socket_handle.h"
#include "net/http/http_stream_parser.h"
#include "net/http/http_request_info.h"

namespace net {

class HttpBasicState {
public:
  HttpBasicState(std::unique_ptr<StreamSocketHandle> connection);

  void Initialize(const HttpRequestInfo* request_info/*,
                  RequestPriority priority,
                  const NetLogWithSource& net_log*/);

  HttpStreamParser* parser() const { return parser_.get(); }

  // Generates a string of the form "METHOD PATH HTTP/1.1\r\n", based on the
  // values of request_info_ and is_for_get_to_http_proxy_.
  std::string GenerateRequestLine() const;

private:
  std::unique_ptr<StreamSocketHandle> connection_;

  std::shared_ptr<GrowableIOBuffer> read_buf_;
  std::unique_ptr<HttpStreamParser> parser_;
};

class HttpBasicStream : public HttpStream {
public:
  HttpBasicStream(std::unique_ptr<StreamSocketHandle> connection);

  // HttpStream methods:
  void RegisterRequest(const HttpRequestInfo* request_info) override;
  int InitializeStream(/*bool can_send_early,
                               RequestPriority priority,
                               const NetLogWithSource& net_log,
                               CompletionOnceCallback callback*/) override;

  int SendRequest(const HttpRequestHeaders& headers/*,
                  HttpResponseInfo* response*/,
                  CompletionOnceCallback callbac) override;

  int ReadResponseHeaders(CompletionOnceCallback callback) override;
  int ReadResponseBody(IOBuffer *buf, int buf_len,
                       CompletionOnceCallback callback) override;

private:
  HttpStreamParser* parser() const { return state_.parser(); }

private:
  HttpBasicState state_;
  
  const HttpRequestInfo* request_info_;
};

}

#endif