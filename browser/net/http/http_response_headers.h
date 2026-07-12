#ifndef NET_HTTP_HTTP_RESPONSE_HEADERS_H_
#define NET_HTTP_HTTP_RESPONSE_HEADERS_H_

#include <string>
#include <optional>
#include <functional>

namespace net {

class HttpResponseHeaders {
public:

  [[nodiscard]] std::optional<std::string> GetNormalizedHeader(
      std::string_view name) const;

};
using ResponseHeadersCallback =
    std::function<void(const HttpResponseHeaders*)>;

}

#endif