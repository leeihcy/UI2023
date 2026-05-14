#include "net/url_request/url_request_http_job.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_context.h"

namespace net {


std::unique_ptr<URLRequestJob> URLRequestHttpJob::Create(URLRequest* request) {
  const GURL& url = request->url();

  // Check for HSTS upgrade.
  return std::make_unique<URLRequestHttpJob>(request);
}

void URLRequestHttpJob::Start() {
  printf("Hello URL Request Http Job \n");

  request_info_.url = request_->url();
  request_info_.method = request_->method();

  // TOOD: GetCookie

  StartTransaction();
}

void URLRequestHttpJob::StartTransaction() {
  m_transaction =
      request_->context()->http_transaction_factory()->CreateTransaction();
  m_transaction->Start(&request_info_);
}

}