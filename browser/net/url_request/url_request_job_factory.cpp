#include "net/url_request/url_request_job_factory.h"
#include "net/url_request/url_request.h"

#include <assert.h>

namespace net {

URLRequestJobFactory::URLRequestJobFactory() {}

bool URLRequestJobFactory::SetProtocolHandler(
    const std::string &scheme,
    std::unique_ptr<ProtocolHandler> protocol_handler) {

  if (!protocol_handler) {
    auto it = m_protocol_handler_map.find(scheme);
    if (it == m_protocol_handler_map.end())
      return false;

    m_protocol_handler_map.erase(it);
    return true;
  }

  if (m_protocol_handler_map.contains(scheme))
    return false;
  m_protocol_handler_map[scheme] = std::move(protocol_handler);
  return true;
}

std::unique_ptr<URLRequestJob>
URLRequestJobFactory::CreateJob(URLRequest *request) const {

  auto it = m_protocol_handler_map.find(request->url().GetScheme());
  if (it == m_protocol_handler_map.end()) {
    assert(false);
    // return std::make_unique<URLRequestErrorJob>(request,
    //                                             ERR_UNKNOWN_URL_SCHEME);
  }

  return it->second->CreateJob(request);
}

} // namespace net