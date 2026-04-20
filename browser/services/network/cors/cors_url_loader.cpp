#include "services/network/cors/cors_url_loader.h"
#include "services/network/url_loader_factory.h"


namespace network {

CorsURLLoader::CorsURLLoader(URLLoaderFactory* factory) : m_network_loader_factory(factory) {
  SetCorsFlagIfNeeded();
}


bool ShouldCheckCors(/*const GURL& request_url,
                     const std::optional<url::Origin>& request_initiator,
                     mojom::RequestMode request_mode*/) {
#if 0
  if (request_mode == network::mojom::RequestMode::kNavigate ||
      request_mode == network::mojom::RequestMode::kNoCors) {
    return false;
  }

  // CORS needs a proper origin (including a unique opaque origin). If the
  // request doesn't have one, CORS should not work.
  DCHECK(request_initiator);

  if (request_initiator->IsSameOriginWith(request_url))
    return false;
  return true;
#endif
  return false;
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

 if (!ShouldCheckCors(/*request_.url, request_.request_initiator,
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
  m_network_loader_factory->CreateLoaderAndStartWithSyncClient();
}

}