
#include "services/network/public/cpp/cors/cors.h"
#include "services/network/public/mojom/cors.mojom-data-view.h"

#include <assert.h>

namespace network {
namespace {
const char kAsterisk[] = "*";
const char kLowerCaseTrue[] = "true";
}

namespace cors {

namespace header_names {

// 一个HTTP响应头，由服务器发回给浏览器，用于明确同意跨源请求携带凭据
// 唯一合法值：这个头部的值只能是 true（区分大小写）
const char kAccessControlAllowCredentials[] =
    "Access-Control-Allow-Credentials";

const char kAccessControlAllowHeaders[] = "Access-Control-Allow-Headers";
const char kAccessControlAllowMethods[] = "Access-Control-Allow-Methods";

// 响应头，只能是一个具体的源（origin），或者是一个通配符 *
// 如果要支持多个，需要后台做动态处理。
const char kAccessControlAllowOrigin[] = "Access-Control-Allow-Origin";


const char kAccessControlMaxAge[] = "Access-Control-Max-Age";
const char kAccessControlRequestHeaders[] = "Access-Control-Request-Headers";
const char kAccessControlRequestMethod[] = "Access-Control-Request-Method";

}  // namespace header_names


std::optional<CorsErrorStatus> CheckAccess(const GURL &response_url,
            const std::optional<std::string> &allow_origin_header,
            const std::optional<std::string> &allow_credentials_header,
            mojom::CredentialsMode credentials_mode,
            const url::Origin &origin
        ) {
  assert(false);

  if (allow_origin_header == kAsterisk) {
    // A wildcard Access-Control-Allow-Origin can not be used if credentials are
    // to be sent, even with Access-Control-Allow-Credentials set to true.
    // See https://fetch.spec.whatwg.org/#cors-protocol-and-credentials.
    if (credentials_mode != mojom::CredentialsMode::kInclude)
      return std::optional<CorsErrorStatus>();

    // 不允许后台直接返回 Access-Control-Allow-Origin: *
    if (response_url.SchemeIsHTTPOrHTTPS()) {
      return CorsErrorStatus(mojom::CorsError::kWildcardOriginNotAllowed);
    }
  } else if (!allow_origin_header) {
    return CorsErrorStatus(mojom::CorsError::kMissingAllowOriginHeader);
  } else if (*allow_origin_header != origin.Serialize()) {
    // 只允许返回一个Origin值，不支持逗号分隔
    // ...
    return CorsErrorStatus(mojom::CorsError::kAllowOriginMismatch/*, *allow_origin_header*/);
  }

  if (credentials_mode == mojom::CredentialsMode::kInclude) {
    if (allow_credentials_header != kLowerCaseTrue) {
      return CorsErrorStatus(mojom::CorsError::kInvalidAllowCredentials/*,
                          allow_credentials_header.value_or(std::string())*/);
    }
  }

  return std::optional<CorsErrorStatus>();
}


}
}