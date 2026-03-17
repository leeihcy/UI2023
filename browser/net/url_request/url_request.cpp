#include "net/url_request/url_request.h"

#include "net/url_request/url_request_job.h"

namespace net {

URLRequest::URLRequest() {

}

void URLRequest::Start() {
  // StartJob(std::make_unique<URLRequestJob>(this));
}

void URLRequest::StartJob(std::unique_ptr<URLRequestJob> job) {
  job->Start();
}

}