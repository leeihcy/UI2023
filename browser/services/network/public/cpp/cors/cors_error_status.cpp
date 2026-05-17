#include "services/network/public/cpp/cors/cors_error_status.h"
namespace network {

CorsErrorStatus::CorsErrorStatus(mojom::CorsError cors_error)
    : cors_error(cors_error) {}


}