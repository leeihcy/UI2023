#include "net/url_request/url_request.h"

#include "net/url_request/url_request_job.h"
#include "net/url_request/url_request_context.h"

namespace net {

URLRequest::URLRequest(const GURL& url, const URLRequestContext* context) : url_chain_(1, url), m_context(context) {

}

void URLRequest::Start() {
  StartJob(m_context->job_factory()->CreateJob(this));
}

void URLRequest::StartJob(std::unique_ptr<URLRequestJob> job) {
  job->Start();
}

}