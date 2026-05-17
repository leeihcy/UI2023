
#ifndef SERVICES_NETWORK_PUBLIC_CPP_URL_LOADER_COMPLETION_STATUS_H_
#define SERVICES_NETWORK_PUBLIC_CPP_URL_LOADER_COMPLETION_STATUS_H_

#include "services/network/public/cpp/cors/cors_error_status.h"
#include <optional>

namespace network {
struct URLLoaderCompletionStatus {

  URLLoaderCompletionStatus();
  explicit URLLoaderCompletionStatus(int error_code);
  URLLoaderCompletionStatus(const CorsErrorStatus& error);

  // The error code. ERR_FAILED is set for CORS errors.
  int error_code = 0;

  // Optional CORS error details.
  std::optional<CorsErrorStatus> cors_error_status;
};

}

#endif