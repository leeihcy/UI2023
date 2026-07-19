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
                  HttpResponseInfo* response*/,
                  CompletionOnceCallback callback) {

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

  io_state_ = STATE_SEND_HEADERS;
  int result = DoLoop(OK);
  if (result == ERR_IO_PENDING)
    callback_ = std::move(callback);

  return result > 0 ? OK : result;
}


int HttpStreamParser::DoLoop(int result) {
  do {
    State state = io_state_;
    io_state_ = STATE_NONE;
    switch (state) {
      case STATE_SEND_HEADERS:
        // DCHECK_EQ(OK, result);
        result = DoSendHeaders();
        // DCHECK_NE(STATE_NONE, io_state_);
        break;
      case STATE_SEND_HEADERS_COMPLETE:
        // result = DoSendHeadersComplete(result);
        // DCHECK_NE(STATE_NONE, io_state_);
        break;
      case STATE_SEND_BODY:
        // DCHECK_EQ(OK, result);
        // result = DoSendBody();
        // DCHECK_NE(STATE_NONE, io_state_);
        break;
      case STATE_SEND_BODY_COMPLETE:
        // result = DoSendBodyComplete(result);
        // DCHECK_NE(STATE_NONE, io_state_);
        break;
      case STATE_SEND_REQUEST_READ_BODY_COMPLETE:
        // result = DoSendRequestReadBodyComplete(result);
        // DCHECK_NE(STATE_NONE, io_state_);
        break;
      case STATE_SEND_REQUEST_COMPLETE:
        // result = DoSendRequestComplete(result);
        break;
      case STATE_READ_HEADERS:
        result = DoReadHeaders();
        break;
      case STATE_READ_HEADERS_COMPLETE:
        // result = DoReadHeadersComplete(result);
        break;
      case STATE_READ_BODY:
        // DCHECK_GE(result, 0);
        result = DoReadBody();
        break;
      case STATE_READ_BODY_COMPLETE:
        result = DoReadBodyComplete(result);
        break;
      default:
        NOTREACHED();
    }
  } while (result != ERR_IO_PENDING &&
           (io_state_ != STATE_DONE && io_state_ != STATE_NONE));

  return result;
}

int HttpStreamParser::DoSendHeaders() {
  int bytes_remaining = request_headers_->BytesRemaining();

  io_state_ = STATE_SEND_HEADERS_COMPLETE;
  // BrokeredTcpClientSocket::Write
  return stream_socket_->Write(
      request_headers_.get(), bytes_remaining, io_callback_);
}

// Handle callbacks.
void HttpStreamParser::OnIOComplete(int result) {
  result = DoLoop(result);

  // The client callback can do anything, including destroying this class,
  // so any pending callback must be issued after everything else is done.
  if (result != ERR_IO_PENDING && callback_) {
    callback_(result);
    callback_ = nullptr;
  }
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
  // if (io_state_ == STATE_DONE)
  //   return OK;

  user_read_buf_ = buf;
  user_read_buf_len_ = size_t(buf_len);
  io_state_ = STATE_READ_BODY;

  int result = DoLoop(OK);
  if (result == ERR_IO_PENDING)
    callback_ = std::move(callback);

  return result;
}

int HttpStreamParser::DoReadBody() {
  io_state_ = STATE_READ_BODY_COMPLETE;

  uint64_t remaining_read_len = user_read_buf_len_;
  uint64_t remaining_body = 0;

  // There may be some data left over from reading the response headers.
  if (read_buf_->offset()) {
    const auto read_offset_s = size_t(read_buf_->offset());
    const size_t available = read_offset_s - read_buf_unused_offset_;
    if (available) {
      const auto bytes_from_buffer = size_t(
          std::min(uint64_t{available}, remaining_read_len));
      auto src = read_buf_->everything().subspan(
          read_buf_unused_offset_, bytes_from_buffer);
      auto dst = user_read_buf_->span();
      std::copy(src.begin(), src.end(), dst.begin());
      
      read_buf_unused_offset_ += bytes_from_buffer;
      // Clear out the remaining data if we've reached the end of the body.
      if (bytes_from_buffer == available) {
        read_buf_->SetCapacity(0);
        read_buf_unused_offset_ = 0;
      }
      return (int)bytes_from_buffer;
    }
    read_buf_->SetCapacity(0);
    read_buf_unused_offset_ = 0;
  }
  
  if (IsResponseBodyComplete())
    return 0;

  return stream_socket_->Read(user_read_buf_,
                              int(user_read_buf_len_),
                              io_callback_);
}


bool HttpStreamParser::IsResponseBodyComplete() const {
  // if (chunked_decoder_.get())
  //   return chunked_decoder_->reached_eof();
  // if (response_body_length_ != -1)
  //   return response_body_read_ >= response_body_length_;

  return false;  // Must read to EOF.
}

int HttpStreamParser::DoReadBodyComplete(int result) {
  // if (result > 0)
  //   received_bytes_ += result;

  // if (result > 0)
  //   response_body_read_ += result;

  return result;
}

} // namespace net