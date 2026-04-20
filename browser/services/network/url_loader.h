#ifndef SERVICES_NETWORK_URL_LOADER_H_
#define SERVICES_NETWORK_URL_LOADER_H_

#include <memory>
#include "net/url_request/url_request.h"

namespace network {
class URLLoaderContext;

class URLLoader {
public:
  URLLoader(URLLoaderContext& context);
  void ScheduleStart();

private:
   std::unique_ptr<net::URLRequest> url_request_;
};

}

#endif