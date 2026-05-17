
#ifndef SERVICES_NETWORK_PUBLIC_MOJOM_CORS_MOJOM_DATA_VIEW_H_
#define SERVICES_NETWORK_PUBLIC_MOJOM_CORS_MOJOM_DATA_VIEW_H_

#include <stdint.h>

namespace network::mojom {



enum class CorsError : int32_t {
  
  kDisallowedByMode = 0,
  
  kInvalidResponse = 1,
  
  kWildcardOriginNotAllowed = 2,
  
  kMissingAllowOriginHeader = 3,
  
  kMultipleAllowOriginValues = 4,
  
  kInvalidAllowOriginValue = 5,
  
  kAllowOriginMismatch = 6,
  
  kInvalidAllowCredentials = 7,
  
  kCorsDisabledScheme = 8,
  
  kPreflightInvalidStatus = 9,
  
  kPreflightDisallowedRedirect = 10,
  
  kPreflightWildcardOriginNotAllowed = 11,
  
  kPreflightMissingAllowOriginHeader = 12,
  
  kPreflightMultipleAllowOriginValues = 13,
  
  kPreflightInvalidAllowOriginValue = 14,
  
  kPreflightAllowOriginMismatch = 15,
  
  kPreflightInvalidAllowCredentials = 16,
  
  kInvalidAllowMethodsPreflightResponse = 17,
  
  kInvalidAllowHeadersPreflightResponse = 18,
  
  kMethodDisallowedByPreflightResponse = 19,
  
  kHeaderDisallowedByPreflightResponse = 20,
  
  kRedirectContainsCredentials = 21,
  
  kInsecureLocalNetwork = 22,
  
  kInvalidLocalNetworkAccess = 23,
  
  kLocalNetworkAccessPermissionDenied = 24,
  kMinValue = 0,
  kMaxValue = 24,
};


}
#endif