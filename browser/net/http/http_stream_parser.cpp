#include "net/http/http_stream_parser.h"

#include "net/socket/stream_socket.h"

namespace net {

// The number of bytes by which the header buffer is grown when it reaches
// capacity.
static const int kHeaderBufInitialSize = 4 * 1024;  // 4K

HttpStreamParser::HttpStreamParser(StreamSocket *stream_socket, const GURL &url,
                                   const std::string &method,
                                   GrowableIOBuffer *read_buffer)
    : url_(url), method_(method), read_buf_(read_buffer),
      stream_socket_(stream_socket) {

  io_callback_ =
      std::bind(&HttpStreamParser::OnIOComplete, this, std::placeholders::_1);
  // weak_ptr_factory_.GetWeakPtr());
}

int HttpStreamParser::SendRequest(const std::string& request_line,
                  const HttpRequestHeaders& headers/*,
                  const NetworkTrafficAnnotationTag& traffic_annotation,
                  HttpResponseInfo* response,
                  CompletionOnceCallback callback*/) {

  // // Put the peer's IP address and port into the response.
  // IPEndPoint ip_endpoint;
  // int result = stream_socket_->GetPeerAddress(&ip_endpoint);
  // if (result != OK)
  //   return result;
  // response_->remote_endpoint = ip_endpoint;


  std::string request = request_line + headers.ToString();
  request_headers_length_ = (int)request.size();

  size_t request_size = request.size();
  std::shared_ptr<StringIOBuffer> headers_io_buf =
        std::make_shared<StringIOBuffer>(std::move(request));

  request_headers_ = std::make_shared<DrainableIOBuffer>(
        std::move(headers_io_buf), request_size);

  int result = DoSendHeaders();

  return result > 0 ? OK : result;
}

int HttpStreamParser::DoSendHeaders() {
  int bytes_remaining = request_headers_->BytesRemaining();

  // BrokeredTcpClientSocket::Write
  return stream_socket_->Write(
      request_headers_.get(), bytes_remaining, io_callback_);
}

// Handle callbacks.
void HttpStreamParser::OnIOComplete(int result) {

}

int HttpStreamParser::ReadResponseHeaders(CompletionOnceCallback callback) {
  int result = OK;
  if (read_buf_->offset() > 0) {
    // Simulate the state where the data was just read from the socket.
    result = read_buf_->offset();
    read_buf_->set_offset(0);
  }
//   if (result > 0)
//     io_state_ = STATE_READ_HEADERS_COMPLETE;

  result = DoReadHeaders();
  if (result == ERR_IO_PENDING)
    callback_ = std::move(callback);

  return result > 0 ? OK : result;
}

int HttpStreamParser::DoReadHeaders() {
  if (read_buf_->RemainingCapacity() == 0)
    read_buf_->SetCapacity(read_buf_->capacity() + kHeaderBufInitialSize);

  return stream_socket_->Read(read_buf_, read_buf_->RemainingCapacity(),
                              io_callback_);
}

int HttpStreamParser::ReadResponseBody(IOBuffer* buf,
                       int buf_len,
                       CompletionOnceCallback callback) {
  return 0;
}

} // namespace net