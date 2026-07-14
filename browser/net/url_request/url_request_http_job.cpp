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

void URLRequestHttpJob::SetResponseHeadersCallback(
    ResponseHeadersCallback callback) {
  response_headers_callback_ = std::move(callback);
}

void URLRequestHttpJob::StartTransaction() {
  m_transaction =
      request_->context()->http_transaction_factory()->CreateTransaction();

  m_transaction->SetResponseHeadersCallback(response_headers_callback_);

  m_transaction->Start(&request_info_,
                       std::bind(&URLRequestHttpJob::OnStartCompleted, this,
                                 std::placeholders::_1));
}

void URLRequestHttpJob::OnStartCompleted(int result) {
  assert(false);
  // NetworkDelegate* network_delegate = request()->network_delegate();
  // if (network_delegate) {
  //  int error = network_delegate->NotifyHeadersReceived(
  //           request_,
  //           base::BindOnce(&URLRequestHttpJob::OnHeadersReceivedCallback,
  //                          weak_factory_.GetWeakPtr()),
  //           headers.get(), &override_response_headers_, endpoint,
  //           &preserve_fragment_on_redirect_url_, ssl_info);
  // }
}

}