#ifndef NET_URL_REQUEST_URL_REQUEST_JOB_H_
#define NET_URL_REQUEST_URL_REQUEST_JOB_H_

#include "net/http/http_response_headers.h"
#include "net/base/transport_info.h"

namespace net {
class URLRequest;

class  URLRequestJob {
public:
  explicit URLRequestJob(URLRequest* request);
  virtual ~URLRequestJob() {}
  
  virtual void Start() = 0;
  virtual void SetResponseHeadersCallback(ResponseHeadersCallback callback) = 0;

public:
  int NotifyConnected(const TransportInfo& info/*,
                      CompletionOnceCallback callback*/);
protected:
  // The request that initiated this job. This value will never be nullptr.
  URLRequest* request_;
};

}

#endif