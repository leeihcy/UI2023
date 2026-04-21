#ifndef NET_URL_REQUEST_URL_REQUEST_CONTEXT_H_
#define NET_URL_REQUEST_URL_REQUEST_CONTEXT_H_

#include <memory>
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_job_factory.h"

namespace net {

class URLRequestContext {
public:
  URLRequestContext();

  std::unique_ptr<URLRequest> CreateRequest(GURL url);

  const URLRequestJobFactory* job_factory() const { return m_job_factory.get(); }

private:
  std::unique_ptr<URLRequestJobFactory> m_job_factory;
};

}

#endif