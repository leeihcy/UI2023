#ifndef SERVICES_NETWORK_PUBLIC_CPP_CORS_CORS_H_
#define SERVICES_NETWORK_PUBLIC_CPP_CORS_CORS_H_

#include <optional>
#include "url/gurl.h"
#include "url/origin.h"
#include "services/network/public/cpp/cors/cors_error_status.h"


namespace network {
namespace mojom {

// 用于控制网络请求在什么情况下携带凭据（Credentials）。
// 凭据包括：Cookies、Authorization 头、客户端证书 和 TLS 客户端证书等。
enum class CredentialsMode : int32_t {
  // 永不携带
  kOmit = 0,
  // 仅同源携带
  kSameOrigin = 1,
  // 始终携带
  kInclude = 2,
};
} // namespace mojom

namespace cors {

namespace header_names {

extern const char kAccessControlAllowCredentials[];
extern const char kAccessControlAllowExternal[];
extern const char kAccessControlAllowHeaders[];
extern const char kAccessControlAllowMethods[];
extern const char kAccessControlAllowOrigin[];
extern const char kAccessControlMaxAge[];
extern const char kAccessControlRequestExternal[];
extern const char kAccessControlRequestHeaders[];
extern const char kAccessControlRequestMethod[];

}  // namespace header_names

// Performs a CORS access check on the response parameters.
// This implements https://fetch.spec.whatwg.org/#concept-cors-check
std::optional<CorsErrorStatus> CheckAccess(
    const GURL& response_url,
    const std::optional<std::string>& allow_origin_header,
    const std::optional<std::string>& allow_credentials_header,
    mojom::CredentialsMode credentials_mode,
    const url::Origin& origin
  );


}
}

#endif