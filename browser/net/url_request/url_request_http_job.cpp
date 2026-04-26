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

  // TOOD: GetCookie

  StartTransaction();
}

void URLRequestHttpJob::StartTransaction() {
  m_transaction =
      m_request->context()->http_transaction_factory()->CreateTransaction();
}
}