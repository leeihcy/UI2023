
#ifndef NET_HTTP_HTTP_TRANSACTION_H_
#define NET_HTTP_HTTP_TRANSACTION_H_

#include "net/base/completion_once_callback.h"
#include "net/http/http_response_headers.h"

namespace net {
struct HttpRequestInfo;
class IOBuffer;
struct TransportInfo;

/*
有两个实现类：
HttpCache::Transaction
HttpNetworkTransaction
其中HttpCache::Transaction在没有cache时，会wrap HttpNetworkTransaction进行调用。
*/
class HttpTransaction {
public:
  // Start负责 连接+发送+读取Response Header
  virtual int Start(const HttpRequestInfo *request_info,
                    CompletionOnceCallback callback) = 0;
  // Read负责继续读取Response Body
  virtual int Read(IOBuffer *buf, int buf_len,
                   CompletionOnceCallback callback) = 0;

  using ConnectedCallback = std::function<int(const TransportInfo &info)>;
  virtual void SetConnectedCallback(const ConnectedCallback& callback) {};
  // virtual void SetRequestHeadersCallback(RequestHeadersCallback callback) {};
  virtual void SetResponseHeadersCallback(ResponseHeadersCallback callback) {};
};

} // namespace net

#endif