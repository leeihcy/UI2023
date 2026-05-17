#include "services/network/public/cpp/url_loader_completion_status.h"
#include "net/base/net_errors.h"

namespace network {
URLLoaderCompletionStatus::URLLoaderCompletionStatus() = default;

URLLoaderCompletionStatus::URLLoaderCompletionStatus(int error_code)
    : error_code(error_code) {}

URLLoaderCompletionStatus::URLLoaderCompletionStatus(
    const CorsErrorStatus &error)
    : URLLoaderCompletionStatus(net::ERR_FAILED) {
  cors_error_status = error;
}
}
