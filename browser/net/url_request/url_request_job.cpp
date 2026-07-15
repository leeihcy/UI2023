#include "net/url_request/url_request_job.h"

#include "net/url_request/url_request.h"

namespace net {

URLRequestJob::URLRequestJob(URLRequest* request) : request_(request) {}

int URLRequestJob::NotifyConnected(const TransportInfo& info/*,
                      CompletionOnceCallback callback*/) {
  return request_->NotifyConnected(info/*, std::move(callback)*/);
}

} // namespace net