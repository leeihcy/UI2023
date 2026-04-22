#include "net/url_request/url_request_job_factory.h"
#include "net/url_request/url_request.h"
#include "url/url_constants.h"
#include "net/url_request/url_request_http_job.h"
#include <assert.h>

namespace net {

class HttpProtocolHandler : public ProtocolHandler {
 public:
  explicit HttpProtocolHandler(bool is_for_websockets)
      : is_for_websockets_(is_for_websockets) {}

  HttpProtocolHandler(const HttpProtocolHandler&) = delete;
  HttpProtocolHandler& operator=(const HttpProtocolHandler&) = delete;
  ~HttpProtocolHandler() override = default;

  std::unique_ptr<URLRequestJob> CreateJob(URLRequest* request) const override {
    return URLRequestHttpJob::Create(request);
  }

  const bool is_for_websockets_;
};


URLRequestJobFactory::URLRequestJobFactory() {
  SetProtocolHandler(url::kHttpScheme, std::make_unique<HttpProtocolHandler>(
                                           /*is_for_websockets=*/false));
  SetProtocolHandler(url::kHttpsScheme, std::make_unique<HttpProtocolHandler>(
                                            /*is_for_websockets=*/false));
// #if BUILDFLAG(ENABLE_WEBSOCKETS)
//   SetProtocolHandler(url::kWsScheme, std::make_unique<HttpProtocolHandler>(
//                                          /*is_for_websockets=*/true));
//   SetProtocolHandler(url::kWssScheme, std::make_unique<HttpProtocolHandler>(
//                                           /*is_for_websockets=*/true));
// #endif  // BUILDFLAG(ENABLE_WEBSOCKETS)
}

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