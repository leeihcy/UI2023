#include "services/network/url_loader.h"
#include "services/network/url_loader_factory.h"
#include "net/url_request/url_request_context.h"
#include "services/network/public/cpp/resource_request.h"

namespace network {

URLLoader::URLLoader(URLLoaderContext& context, const ResourceRequest& request) {
  url_request_ = context.GetUrlRequestContext()->CreateRequest(request.url);
  ScheduleStart();
}

void URLLoader::ScheduleStart() {
  url_request_->Start();
}

}