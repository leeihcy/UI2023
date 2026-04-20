#include "services/network/url_loader.h"
#include "services/network/url_loader_factory.h"
#include "net/url_request/url_request_context.h"

namespace network {

URLLoader::URLLoader(URLLoaderContext& context) {
  url_request_ = context.GetUrlRequestContext()->CreateRequest();
  // url_request_ = std::make_unique<net::URLRequest>();

  ScheduleStart();
}

void URLLoader::ScheduleStart() {
  url_request_->Start();
}

}