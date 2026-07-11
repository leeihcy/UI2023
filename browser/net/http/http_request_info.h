
#ifndef NET_HTTP_HTTP_REQUEST_INFO_H__
#define NET_HTTP_HTTP_REQUEST_INFO_H__

#include "url/gurl.h"
#include <string>

namespace net {

struct HttpRequestInfo {
  HttpRequestInfo() {
  }
  ~HttpRequestInfo() {
  }

  // The requested URL.
  GURL url;

  // The method to use (GET, POST, etc.).
  std::string method;

};

}


#endif