#ifndef NET_HTTP_HTTP_UTIL_H_
#define NET_HTTP_HTTP_UTIL_H_

#include <string>
#include "url/gurl.h"

namespace net {

class HttpResponseHeaders;

class HttpUtil {
 public:
  // Generates a request line that is used for text-based HTTP messaging.
  static std::string GenerateRequestLine(std::string_view method,
                                         const GURL& url);
};
}

#endif
