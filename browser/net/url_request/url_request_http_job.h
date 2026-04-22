
#ifndef NET_URL_REQUEST_URL_REQUEST_HTTP_JOB_H_
#define NET_URL_REQUEST_URL_REQUEST_HTTP_JOB_H_

#include "net/url_request/url_request_job.h"

#include <memory>

namespace net {
class URLRequestHttpJob : public URLRequestJob {
public:
  URLRequestHttpJob(URLRequest* request) : URLRequestJob(request) {}
  
  // Creates URLRequestJob for the specified HTTP, HTTPS, WS, or WSS URL.
  // Returns a job that returns a redirect in the case of HSTS, and returns a
  // job that fails for unencrypted requests if current settings dont allow
  // them. Never returns nullptr.
  static std::unique_ptr<URLRequestJob> Create(URLRequest* request);

  void Start() override;
};

}

#endif