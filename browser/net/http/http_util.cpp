#include "net/http/http_util.h"

#include "base/strings/strcat.h"

namespace net {

std::string HttpUtil::GenerateRequestLine(std::string_view method,
                                          const GURL& url) {
  static constexpr char kSuffix[] = " HTTP/1.1\r\n";
  return base::StrCat({method, " ", url.PathForRequest(), kSuffix});
}

}
