
#ifndef NET_URL_REQUEST_URL_REQUEST_HTTP_JOB_H_
#define NET_URL_REQUEST_URL_REQUEST_HTTP_JOB_H_

#include "net/url_request/url_request_job.h"
#include "net/http/http_transaction.h"
#include "net/http/http_request_info.h"
#include <memory>

namespace net {
class URLRequestHttpJob : public URLRequestJob {
public:
  URLRequestHttpJob(URLRequest* request) : URLRequestJob(request) {}
  
  // Creates URLRequestJob for the specified HTTP, HTTPS, WS, or WSS URL.
  // Returns a job that returns a redirect in the case of HSTS, and returns a
  // job that fails for unencrypted requests if current settings dont allow
  // them. Never returns nullptr.
  static std::unique_ptr<URLRequestJob> Create(URLRequest* request);

  void Start() override;
  void SetResponseHeadersCallback(ResponseHeadersCallback callback) override;
  int ReadRawData(IOBuffer* buf, int buf_size) override;

private:
  void StartTransaction();

  void OnStartCompleted(int result);
  void OnReadCompleted(int result);

  int NotifyConnectedCallback(const TransportInfo& info);
  void SaveCookiesAndNotifyHeadersComplete(int result);
  void NotifyHeadersComplete();

private:
   std::unique_ptr<HttpTransaction> m_transaction;
   HttpRequestInfo request_info_;

  ResponseHeadersCallback response_headers_callback_;

};

}

#endif