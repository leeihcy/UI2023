#include "net/url_request/url_request.h"

#include "net/url_request/url_request_job.h"
#include "net/url_request/url_request_context.h"

namespace net {

URLRequest::URLRequest(const GURL &url, Delegate* delegate, const URLRequestContext *context)
    : url_chain_(1, url), delegate_(delegate), m_context(context), method_("GET") {}

void URLRequest::set_method(std::string_view method) {
  // DCHECK(!is_pending_);
  method_ = std::string(method);
}

void URLRequest::Start() {
  StartJob(m_context->job_factory()->CreateJob(this));
}

void URLRequest::SetResponseHeadersCallback(ResponseHeadersCallback callback) {
  response_headers_callback_ = std::move(callback);
}

void URLRequest::StartJob(std::unique_ptr<URLRequestJob> job) {
  job_ = std::move(job);

  job_->SetResponseHeadersCallback(response_headers_callback_);

  job_->Start();
}


int URLRequest::NotifyConnected(const TransportInfo& info) {
  int result = delegate_->OnConnected(this, info);
  return result;
}


}