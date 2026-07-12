#ifndef NET_URL_REQUEST_URL_REQUEST_JOB_H_
#define NET_URL_REQUEST_URL_REQUEST_JOB_H_

#include "net/http/http_response_headers.h"

namespace net {
class URLRequest;

class  URLRequestJob {
public:
  explicit URLRequestJob(URLRequest* request);
  virtual ~URLRequestJob() {}
  
  virtual void Start() = 0;
  virtual void SetResponseHeadersCallback(ResponseHeadersCallback callback) = 0;

protected:
  // The request that initiated this job. This value will never be nullptr.
  const URLRequest* request_;
};

}

#endif