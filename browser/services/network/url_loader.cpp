#include "services/network/url_loader.h"

namespace network {

URLLoader::URLLoader() {
  // url_request_ = url_request_context_->CreateRequest();
  url_request_ = std::make_unique<net::URLRequest>();

  ScheduleStart();
}

void URLLoader::ScheduleStart() {
  url_request_->Start();
}

}