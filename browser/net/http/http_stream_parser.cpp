#include "net/http/http_stream_parser.h"

namespace net {

HttpStreamParser::HttpStreamParser() {

}

int HttpStreamParser::SendRequest(const std::string& request_line/*,
                  const HttpRequestHeaders& headers,
                  const NetworkTrafficAnnotationTag& traffic_annotation,
                  HttpResponseInfo* response,
                  CompletionOnceCallback callback*/) {

  // // Put the peer's IP address and port into the response.
  // IPEndPoint ip_endpoint;
  // int result = stream_socket_->GetPeerAddress(&ip_endpoint);
  // if (result != OK)
  //   return result;
  // response_->remote_endpoint = ip_endpoint;

  // std::string request = request_line + headers.ToString();
  // request_headers_length_ = request.size();

  DoSendHeaders();

  return 0;
}

int HttpStreamParser::DoSendHeaders() {
  // BrokeredTcpClientSocket::Write
  //
  // return stream_socket_->Write(
  //     request_headers_.get(), bytes_remaining, io_callback_,
  //     NetworkTrafficAnnotationTag(traffic_annotation_));
  return 0;
}

}