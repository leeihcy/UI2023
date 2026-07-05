#include "net/http/http_basic_stream.h"

#include <assert.h>

namespace net {

HttpBasicState::HttpBasicState(std::unique_ptr<StreamSocketHandle> connection) : connection_(std::move(connection)) {

}

void HttpBasicState::Initialize(/*const HttpRequestInfo* request_info,
                  RequestPriority priority,
                  const NetLogWithSource& net_log*/) 
{
  parser_ = std::make_unique<HttpStreamParser>();
}

std::string HttpBasicState::GenerateRequestLine() const {
#if 0
  return HttpUtil::GenerateRequestLine(parser_->method(), parser_->url(),
                                       is_for_get_to_http_proxy_);
#endif
  assert(false);
  return "";
}


HttpBasicStream::HttpBasicStream(std::unique_ptr<StreamSocketHandle> connection) : state_(std::move(connection)) {

}

int HttpBasicStream::InitializeStream(/*bool can_send_early,
                               RequestPriority priority,
                               const NetLogWithSource& net_log,
                               CompletionOnceCallback callback*/) {
  state_.Initialize(/*request_info_, priority, net_log*/);

  // RequestInfo is no longer needed after this point.
  // request_info_ = nullptr;

  return 0;
}

int HttpBasicStream::SendRequest(const HttpRequestHeaders& headers/*,
                  HttpResponseInfo* response,
                  CompletionOnceCallback callbac*/) {
  // if (request_headers_callback_) {
  //   HttpRawRequestHeaders raw_headers;
  //   raw_headers.set_request_line(state_.GenerateRequestLine());
  //   for (HttpRequestHeaders::Iterator it(headers); it.GetNext();) {
  //     raw_headers.Add(it.name(), it.value());
  //   }
  //   request_headers_callback_.Run(std::move(raw_headers));
  // }

  return parser()->SendRequest(
      state_.GenerateRequestLine()/*, headers,
      NetworkTrafficAnnotationTag(state_.traffic_annotation()), response,
      std::move(callback)*/);
}

}