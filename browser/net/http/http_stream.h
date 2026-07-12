#ifndef NET_HTTP_HTTP_STREAM_H_
#define NET_HTTP_HTTP_STREAM_H_

#include "net/http/http_request_headers.h"
#include "net/base/completion_once_callback.h"

namespace net {
struct HttpRequestInfo;

class HttpStream {
public:
  virtual void RegisterRequest(const HttpRequestInfo* request_info) = 0;
  virtual int InitializeStream(/*bool can_send_early,
                               RequestPriority priority,
                               const NetLogWithSource& net_log,
                               CompletionOnceCallback callback*/) = 0;

  virtual int SendRequest(const HttpRequestHeaders& headers/*,
                  HttpResponseInfo* response,
                  CompletionOnceCallback callback*/) = 0;
  virtual int ReadResponseHeaders(CompletionOnceCallback callback) = 0;

};

}

#endif