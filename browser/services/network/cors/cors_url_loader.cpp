#include "services/network/cors/cors_url_loader.h"
#include "services/network/url_loader_factory.h"
#include "services/network/public/cpp/cors/cors.h"
#include "url/origin.h"
#include <assert.h>

namespace network {

namespace {

// 预检请求（Preflight）的核心作用可以用一句话概括：
// 在实际发送跨域请求之前，先“试探”一下服务器是否允许这种跨域操作，
// 保护那些传统 HTML 表单无法发起的、可能具有破坏性的请求。
// 1. 保护服务器资源（核心安全作用）
// 2. 减少不必要的数据传输（性能优化）
// 3. 携带额外凭证信息
//
// 为了性能，预检结果可以被缓存。服务器通过 Access-Control-Max-Age 头指定缓存时间（单位：秒）：
//
// preflight场景：
enum class PreflightRequiredReason {

  // 强制指定：
  // fetch('https://api.example.com/ping', {
  //   mode: 'cors-with-forced-preflight'
  // });
  kCorsWithForcedPreflightMode,

  // 简单方法（GET/HEAD/POST）
  kDisallowedMethod,

  // 安全头（如 Accept、Accept-Language、Content-Language、
  // Content-Type 仅限于 application/x-www-form-urlencoded、
  // multipart/form-data 或 text/plain）；
  kDisallowedHeader
};


// Returns std::nullopt when a preflight isn't needed. Otherwise returns the
// reason why a preflight is needed.
std::optional<PreflightRequiredReason> NeedsPreflight(
    const ResourceRequest& request) {

  // if (!IsCorsSafelistedMethod(request.method))
  //   return PreflightRequiredReason::kDisallowedMethod;

  // if (!CorsUnsafeNotForbiddenRequestHeaderNames(
  //          request.headers.GetHeaderVector(), request.is_revalidating)
  //          .empty())
  //   return PreflightRequiredReason::kDisallowedHeader;

  return std::nullopt;
}

}

CorsURLLoader::CorsURLLoader(const ResourceRequest &resource_request,
                             URLLoaderFactory *factory)
    : request_(resource_request), m_network_loader_factory(factory) {
  SetCorsFlagIfNeeded();
}

// 判断是否需要进行跨域检测
bool ShouldCheckCors(const GURL& request_url,
                     const std::optional<url::Origin>& request_initiator/*,
                     mojom::RequestMode request_mode*/) {
#if 0
  if (request_mode == network::mojom::RequestMode::kNavigate ||
      request_mode == network::mojom::RequestMode::kNoCors) {
    return false;
  }

  // CORS needs a proper origin (including a unique opaque origin). If the
  // request doesn't have one, CORS should not work.
  DCHECK(request_initiator);
#endif

  if (request_initiator->IsSameOriginWith(request_url))
    return false;
  return true;
}

bool CorsURLLoader::HasSpecialAccessToDestination() const {
#if 0
  // The source origin and destination URL pair may be in the allow list.
  switch (origin_access_list_->CheckAccessState(request_)) {
    case OriginAccessList::AccessState::kAllowed:
      return true;
    case OriginAccessList::AccessState::kBlocked:
    case OriginAccessList::AccessState::kNotListed:
      return false;
  }
#endif
  return false;
}

void CorsURLLoader::SetCorsFlagIfNeeded() {
 if (fetch_cors_flag_) {
   return;
 }

 if (!ShouldCheckCors(request_.url, request_.request_initiator/*,
                                     request_.mode*/)) {
   return;
 }

 if (HasSpecialAccessToDestination())
   return;

 fetch_cors_flag_ = true;
}


void CorsURLLoader::Start() {
#if 0
   if (fetch_cors_flag_ && IsCorsEnabledRequestMode(request_.mode)) {
    // Username and password should be stripped in a CORS-enabled request.
    if (request_.url.has_username() || request_.url.has_password()) {
      GURL::Replacements replacements;
      replacements.SetUsernameStr("");
      replacements.SetPasswordStr("");
      request_.url = request_.url.ReplaceComponents(replacements);
    }
  }
#endif
  StartRequest();
}

void CorsURLLoader::StartRequest() {
  std::optional<PreflightRequiredReason> needs_preflight =
      NeedsPreflight(request_);

  // 只在跨域的时候，才触发预检
  bool preflight_required = needs_preflight.has_value() && fetch_cors_flag_;
  if (!preflight_required) {
    StartNetworkRequest();
    return;
  }

  // TODO: PerformPreflightCheck();
  assert(false);
}

void CorsURLLoader::StartNetworkRequest() {
  m_network_loader_factory->CreateLoaderAndStartWithSyncClient(request_);
}

// static
std::optional<std::string> CorsURLLoader::GetHeaderString(
    const mojom::URLResponseHead& response,
    const std::string& header_name) {
  if (!response.headers) {
    return std::nullopt;
  }
  return response.headers->GetNormalizedHeader(header_name);
}

void CorsURLLoader::OnReceiveResponse(
    mojom::URLResponseHead* response_head //,
    // mojo::ScopedDataPipeConsumerHandle body,
    // std::optional<mojo_base::BigBuffer> cached_metadata 
){
  if (fetch_cors_flag_) {
    std::optional<CorsErrorStatus> result = cors::CheckAccess(
        request_.url,

        // Access-Control-Allow-Origin: *
        GetHeaderString(*response_head,
                        cors::header_names::kAccessControlAllowOrigin),

        // Access-Control-Allow-Credentials: 
        GetHeaderString(*response_head,
                        cors::header_names::kAccessControlAllowCredentials),
        request_.credentials_mode,

        /*tainted_ ? url::Origin() :*/ *request_.request_initiator);
    if (!result.has_value()) {
      HandleComplete(URLLoaderCompletionStatus(*result));
      return;
    }
  }
}

void CorsURLLoader::HandleComplete(URLLoaderCompletionStatus status) {
  // 跨域错误，上报给控制台。
  // if (devtools_observer_ && status.cors_error_status) {
  //   ReportCorsErrorToDevTools(*status.cors_error_status);
  // }
}

}