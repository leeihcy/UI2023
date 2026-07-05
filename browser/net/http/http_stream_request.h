#ifndef NET_HTTP_HTTP_STREAM_REQUEST_H_
#define NET_HTTP_HTTP_STREAM_REQUEST_H_

#include <memory>

namespace net {
class HttpStream;

class HttpStreamRequest {
public:
  void InitializeStream();

  class Delegate {
  public:
    virtual ~Delegate() = default;
    virtual void OnStreamReady(/*const ProxyInfo& used_proxy_info,*/
                               std::unique_ptr<HttpStream> stream) = 0;
  };
};
}

#endif