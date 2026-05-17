#ifndef SERVICES_NETWORK_PUBLIC_CPP_CORS_CORS_ERROR_STATUS_H_
#define SERVICES_NETWORK_PUBLIC_CPP_CORS_CORS_ERROR_STATUS_H_
#include "services/network/public/mojom/cors.mojom-data-view.h"

namespace network {

struct CorsErrorStatus {
  explicit CorsErrorStatus(mojom::CorsError cors_error);

  mojom::CorsError cors_error = mojom::CorsError::kMaxValue;

};

}

#endif