
#include "net/http/http_response_headers.h"
#include <assert.h>

namespace net {

std::optional<std::string>
HttpResponseHeaders::GetNormalizedHeader(std::string_view name) const {
  assert(false);
  return std::optional<std::string>();
}

} // namespace net