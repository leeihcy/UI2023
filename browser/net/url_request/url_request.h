#ifndef NET_URL_REQUEST_URL_REQUEST_H_
#define NET_URL_REQUEST_URL_REQUEST_H_

#include <memory>
#include <vector>

#include "url/gurl.h"
#include "net/url_request/url_request_job.h"

namespace net {
class URLRequestContext;

class URLRequest {
public:
  URLRequest(const GURL& url, const URLRequestContext* context);

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
  
private:
  const URLRequestContext* m_context;
  std::vector<GURL> url_chain_;

  std::string method_;  // "GET", "POST", etc. Case-sensitive.

  std::unique_ptr<URLRequestJob> job_;
};

}

#endif
