#include "net/url_request/url_request_context.h"

namespace net {

URLRequestContext::URLRequestContext() {
   m_job_factory = std::make_unique<URLRequestJobFactory>();
}

std::unique_ptr<URLRequest> URLRequestContext::CreateRequest(GURL url) {
  return std::make_unique<URLRequest>(url, this);
}

}