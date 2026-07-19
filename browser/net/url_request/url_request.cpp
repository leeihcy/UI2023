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

void URLRequest::NotifyResponseStarted(int net_error) {
   delegate_->OnResponseStarted(this, net_error);
  // Nothing may appear below this line as OnResponseStarted may delete
  // |this|.
}

int URLRequest::Read(IOBuffer* dest, int dest_size) {
  int rv = job_->Read(dest, dest_size);
  // if (rv == ERR_IO_PENDING) {
  //   set_status(ERR_IO_PENDING);
  // } else if (rv <= 0) {
  //   NotifyRequestCompleted();
  // }
  // 
  // // If rv is not 0 or actual bytes read, the status cannot be success.
  // DCHECK(rv >= 0 || status_ != OK);
  return rv;
}

void URLRequest::NotifyReadCompleted(int result) {
  delegate_->OnReadCompleted(this, result);
}

}