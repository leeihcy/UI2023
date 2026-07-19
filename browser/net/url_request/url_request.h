#ifndef NET_URL_REQUEST_URL_REQUEST_H_
#define NET_URL_REQUEST_URL_REQUEST_H_

#include <memory>
#include <vector>

#include "url/gurl.h"
#include "net/url_request/url_request_job.h"

namespace net {
class IOBuffer;
class URLRequestContext;

class URLRequest {
public:
  class Delegate {
  public:
    virtual int OnConnected(URLRequest *request, const TransportInfo &info) = 0;
    
    // 调用 Start() 后，将触发这个回调。
    virtual void OnResponseStarted(URLRequest* request, int net_error) = 0;
  };

  URLRequest(const GURL& url, Delegate* delegate, const URLRequestContext* context);

  // The original URL is the URL used to initialize the request, and it may
  // differ from the URL if the request was redirected.
  const GURL& original_url() const { return url_chain_.front(); }
  // The chain of URLs traversed by this request. If the request had no
  // redirects, this vector will contain one element.
  const std::vector<GURL>& url_chain() const { return url_chain_; }
  const GURL& url() const { return url_chain_.back(); }
  
  // The request method. "GET" is the default value. The request method may
  // only be changed before Start() is called. Request methods are
  // case-sensitive, so standard HTTP methods like GET or POST should be
  // specified in uppercase.
  const std::string& method() const { return method_; }
  void set_method(std::string_view method);

  const URLRequestContext* context() const { return m_context; }

  void Start();
  void StartJob(std::unique_ptr<URLRequestJob> job);
  
  int Read(IOBuffer* buf, int max_bytes);

  // Sets a callback that will be invoked each time the response is received
  // from the remote party with the actual response headers received. Note this
  // is different from response_headers() getter in that in case of revalidation
  // request, the latter will return cached headers, while the callback will be
  // called with a response from the server.
  void SetResponseHeadersCallback(ResponseHeadersCallback callback);


public:
   int NotifyConnected(const TransportInfo& info);
  // Called by URLRequestJob to allow interception when the final response
  // occurs.
  void NotifyResponseStarted(int net_error);

private:
  const URLRequestContext* m_context;
  std::vector<GURL> url_chain_;

  std::string method_;  // "GET", "POST", etc. Case-sensitive.

  Delegate* delegate_;
  ResponseHeadersCallback response_headers_callback_;

  std::unique_ptr<URLRequestJob> job_;
};

}

#endif
