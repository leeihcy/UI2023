#ifndef SERVICES_NETWORK_URL_LOADER_H_
#define SERVICES_NETWORK_URL_LOADER_H_

#include <memory>
#include "net/url_request/url_request.h"
#include "net/url_request/url_request.h"

namespace network {
class URLLoaderContext;
class ResourceRequest;

class URLLoader : public net::URLRequest::Delegate {
public:
  URLLoader(URLLoaderContext& context, const ResourceRequest& request);
  void ScheduleStart();

  // net::URLRequest::Delegate
  int OnConnected(net::URLRequest *request, const net::TransportInfo &info) override;

private:
   std::unique_ptr<net::URLRequest> url_request_;
};

}

#endif