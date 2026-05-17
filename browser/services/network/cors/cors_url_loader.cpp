#include "services/network/cors/cors_url_loader.h"
#include "services/network/url_loader_factory.h"
#include "services/network/public/cpp/cors/cors.h"
#include "url/origin.h"
#include <assert.h>

namespace network {

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
  //  if (!preflight_required) {
  StartNetworkRequest();
  //    return;
  //  }
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