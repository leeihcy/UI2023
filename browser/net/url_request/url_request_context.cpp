#include "net/url_request/url_request_context.h"

namespace net {
std::unique_ptr<URLRequest> URLRequestContext::CreateRequest() {
  return std::make_unique<URLRequest>();
}

}