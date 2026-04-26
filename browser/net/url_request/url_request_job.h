#ifndef NET_URL_REQUEST_URL_REQUEST_JOB_H_
#define NET_URL_REQUEST_URL_REQUEST_JOB_H_

namespace net {
class URLRequest;

class  URLRequestJob {
public:
  explicit URLRequestJob(URLRequest* request);
  
  virtual void Start() = 0;

protected:
  // The request that initiated this job. This value will never be nullptr.
  const URLRequest* m_request;
};

}

#endif