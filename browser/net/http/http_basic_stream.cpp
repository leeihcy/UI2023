#include "net/http/http_basic_stream.h"

#include <assert.h>
#include "net/http/http_util.h"

namespace net {

HttpBasicState::HttpBasicState(std::unique_ptr<StreamSocketHandle> connection) : connection_(std::move(connection)) {
  read_buf_ = std::make_shared<GrowableIOBuffer>();
}

void HttpBasicState::Initialize(const HttpRequestInfo* request_info/*,
                  RequestPriority priority,
                  const NetLogWithSource& net_log*/) 
{
  parser_ = std::make_unique<HttpStreamParser>(
    connection_->socket(), request_info->url, request_info->method, read_buf_.get());
}

std::string HttpBasicState::GenerateRequestLine() const {
  return HttpUtil::GenerateRequestLine(parser_->method(), parser_->url());
}


HttpBasicStream::HttpBasicStream(std::unique_ptr<StreamSocketHandle> connection) : state_(std::move(connection)) {

}

void HttpBasicStream::RegisterRequest(const HttpRequestInfo* request_info) {
  request_info_ = request_info;
}

int HttpBasicStream::InitializeStream(/*bool can_send_early,
                               RequestPriority priority,
                               const NetLogWithSource& net_log,
                               CompletionOnceCallback callback*/) {
  state_.Initialize(request_info_/*, priority, net_log*/);

  // RequestInfo is no longer needed after this point.
  request_info_ = nullptr;

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
      state_.GenerateRequestLine(), headers/*,
      NetworkTrafficAnnotationTag(state_.traffic_annotation()), response,
      std::move(callback)*/);
}

int HttpBasicStream::ReadResponseHeaders(CompletionOnceCallback callback) {
  return parser()->ReadResponseHeaders(std::move(callback));
}

int HttpBasicStream::ReadResponseBody(IOBuffer *buf, int buf_len,
                                      CompletionOnceCallback callback) {
  return parser()->ReadResponseBody(buf, buf_len, std::move(callback));
}

}