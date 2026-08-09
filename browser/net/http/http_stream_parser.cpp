#include "net/http/http_stream_parser.h"

#include "base/log.h"
#include "net/socket/stream_socket.h"
#include "net/http/http_response_headers.h"
#include "net/http/http_util.h"
#include "net/http/http_status_code.h"

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

    char text[128];
    sprintf(text, "HttpStreamParser::DoLoop state=%d", state);
    base::Log(text);
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
        io_state_ = STATE_SEND_REQUEST_COMPLETE;
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
        result = DoReadHeadersComplete(result);
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
    // 要先清空callbac_再执行回调，不能先回调再清callback。
    // 因为回调的过程中可能会再重入一次设置callback。
    auto f = std::move(callback_);
    f(result);
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
  if (result == ERR_IO_PENDING) {
      callback_ = std::move(callback);
  }

  return result > 0 ? OK : result;
}

int HttpStreamParser::DoReadHeaders() {
  io_state_ = STATE_READ_HEADERS_COMPLETE;
  if (read_buf_->RemainingCapacity() == 0)
    read_buf_->SetCapacity(read_buf_->capacity() + kHeaderBufInitialSize);

  return stream_socket_->Read(read_buf_, read_buf_->RemainingCapacity(),
                              io_callback_);
}


int HttpStreamParser::DoReadHeadersComplete(int result) {
  // DoReadHeadersComplete is called with the result of Socket::Read, which is a
  // (byte_count | error), and returns (error | OK).

  result = HandleReadHeaderResult(result);
  if (io_state_ == STATE_READ_HEADERS) {
    return result;
  }
  
  if (io_state_ == STATE_DONE || !response_->headers ||
      response_->headers->response_code() / 100 != 1) {
    response_ = nullptr;
  }

  return result;
}

static void copy_prefix_from(std::span<uint8_t> target, std::span<uint8_t> source, size_t n) {
    // 确保不会越界
    size_t copy_count = std::min({n, source.size(), target.size()});
    std::copy(source.begin(), source.begin() + copy_count, target.begin());
}

int HttpStreamParser::HandleReadHeaderResult(int result) {
  if (result == 0)
    result = ERR_CONNECTION_CLOSED;

  if (result == ERR_CONNECTION_CLOSED) {
        // The connection closed without getting any more data.
    if (read_buf_->offset() == 0) {
      io_state_ = STATE_DONE;
      // if (!connection_is_reused_)
      //   return ERR_EMPTY_RESPONSE;
      return result;
    }

    // Parse things as well as we can and let the caller decide what to do.
    int end_offset;
    if (response_header_start_offset_ != std::string::npos) {
      io_state_ = STATE_READ_BODY_COMPLETE;
      end_offset = read_buf_->offset();
    } else {
      end_offset = 0;
    }
    int rv = ParseResponseHeaders(end_offset);
    if (rv < 0)
      return rv;
    return result;
  }

  if (result < 0) {
    if (result == ERR_SSL_CLIENT_AUTH_CERT_NEEDED) {
      base::Assert(false);
    }
    io_state_ = STATE_DONE;
    return result;
  }

  read_buf_->set_offset(read_buf_->offset() + result);
  int end_of_header_offset = FindAndParseResponseHeaders(result);

  // Note: -1 is special, it indicates we haven't found the end of headers.
  // Anything less than -1 is a Error, so we bail out.
  if (end_of_header_offset < -1)
    return end_of_header_offset;

   if (end_of_header_offset == -1) {
    io_state_ = STATE_READ_HEADERS;
    // Prevent growing the headers buffer indefinitely.
    if (read_buf_->offset() >= kMaxHeaderBufSize) {
      io_state_ = STATE_DONE;
      return ERR_RESPONSE_HEADERS_TOO_BIG;
    }
  } else {

    CalculateResponseBodySize();

    // If the body is zero length, the caller may not call ReadResponseBody,
    // which is where any extra data is copied to read_buf_, so we move the
    // data here.
    const auto end_of_header_offset_s =
        static_cast<size_t>(end_of_header_offset);
    if (response_body_length_ == 0) {
      std::span<uint8_t> extra_bytes =
          read_buf_->span_before_offset().subspan(end_of_header_offset_s);
      if (!extra_bytes.empty()) {
        // read_buf_->everything().copy_prefix_from(extra_bytes);
        copy_prefix_from(read_buf_->everything(), extra_bytes, extra_bytes.size());
      }
      read_buf_->SetCapacity(extra_bytes.size());
      if (response_->headers->response_code() / 100 == 1) {
        // After processing a 1xx response, the caller will ask for the next
        // header, so reset state to support that. We don't completely ignore a
        // 1xx response because it cannot be returned in reply to a CONNECT
        // request so we return OK here, which lets the caller inspect the
        // response and reject it in the event that we're setting up a CONNECT
        // tunnel.
        response_header_start_offset_ = std::string::npos;
        response_body_length_ = -1;
        // Record the timing of the 103 Early Hints response for the experiment
        // (https://crbug.com/1093693).
        // if (response_->headers->response_code() == HTTP_EARLY_HINTS &&
        //     first_early_hints_time_.is_null()) {
        //   first_early_hints_time_ = current_response_start_time_;
        // }
        // Now waiting for the second set of headers to be read.
      } else {
        // Only set keep-alive based on final set of headers.
        response_is_keep_alive_ = response_->headers->IsKeepAlive();

        io_state_ = STATE_DONE;
      }
      return OK;
    }

    // Only set keep-alive based on final set of headers.
    response_is_keep_alive_ = response_->headers->IsKeepAlive();

    // Note where the headers stop.
    read_buf_unused_offset_ = end_of_header_offset_s;
    // Now waiting for the body to be read.
  }

}


int HttpStreamParser::FindAndParseResponseHeaders(int new_bytes) {
  size_t end_offset = std::string::npos;

  // Look for the start of the status line, if it hasn't been found yet.
  if (response_header_start_offset_ == std::string::npos) {
    response_header_start_offset_ =
        HttpUtil::LocateStartOfStatusLine(read_buf_->span_before_offset());
  }

  if (response_header_start_offset_ != std::string::npos) {
    size_t lower_bound = read_buf_->offset() - new_bytes - 3;
    size_t search_start = std::max(response_header_start_offset_, lower_bound);
    end_offset = HttpUtil::LocateEndOfHeaders(read_buf_->span_before_offset(),
                                              search_start);
  } else if (read_buf_->offset() >= 8) {
    // Enough data to decide that this is an HTTP/0.9 response.
    // 8 bytes = (4 bytes of junk) + "http".length()
    end_offset = 0;
  }
  if (end_offset == std::string::npos)
    return -1;

  int rv = ParseResponseHeaders(end_offset);
  if (rv < 0)
    return rv;
  return (int)end_offset;
}

int HttpStreamParser::ParseResponseHeaders(size_t end_offset) {
  std::shared_ptr<HttpResponseHeaders> headers;
#if 0
  if (response_header_start_offset_ != std::string::npos) {
    received_bytes_ += (end_offset);
    headers = HttpResponseHeaders::TryToCreate(
        base::as_string_view(read_buf_->everything().first(end_offset)));
    if (!headers)
      return ERR_INVALID_HTTP_RESPONSE;
    has_seen_status_line_ = true;
  }

   else {
    // Enough data was read -- there is no status line, so this is HTTP/0.9, or
    // the server is broken / doesn't speak HTTP.

    if (has_seen_status_line_) {
      // If we saw a status line previously, the server can speak HTTP/1.x so it
      // is not reasonable to interpret the response as an HTTP/0.9 response.
      return ERR_INVALID_HTTP_RESPONSE;
    }

    std::string_view scheme = url_.scheme();
    if (url::DefaultPortForScheme(scheme) != url_.EffectiveIntPort()) {
      // If the port is not the default for the scheme, assume it's not a real
      // HTTP/0.9 response, and fail the request.

      // Allow Shoutcast responses over HTTP, as it's somewhat common and relies
      // on HTTP/0.9 on weird ports to work.
      // See
      // https://groups.google.com/a/chromium.org/forum/#!topic/blink-dev/qS63pYso4P0
      if (read_buf_->offset() < 3 || scheme != "http" ||
          !base::EqualsCaseInsensitiveASCII(
              base::as_string_view(read_buf_->everything().first(3u)), "icy")) {
        return ERR_INVALID_HTTP_RESPONSE;
      }
    }

    headers = base::MakeRefCounted<HttpResponseHeaders>(
        std::string("HTTP/0.9 200 OK"));
  }

  // Check for multiple Content-Length headers when the response is not
  // chunked-encoded.  If they exist, and have distinct values, it's a potential
  // response smuggling attack.
  if (!headers->IsChunkEncoded()) {
    if (HttpUtil::HeadersContainMultipleCopiesOfField(*headers,
                                                      "Content-Length"))
      return ERR_RESPONSE_HEADERS_MULTIPLE_CONTENT_LENGTH;
  }

  // Check for multiple Content-Disposition or Location headers.  If they exist,
  // it's also a potential response smuggling attack.
  if (HttpUtil::HeadersContainMultipleCopiesOfField(*headers,
                                                    "Content-Disposition"))
    return ERR_RESPONSE_HEADERS_MULTIPLE_CONTENT_DISPOSITION;
  if (HttpUtil::HeadersContainMultipleCopiesOfField(*headers, "Location"))
    return ERR_RESPONSE_HEADERS_MULTIPLE_LOCATION;

  response_->headers = headers;
  if (headers->GetHttpVersion() == HttpVersion(0, 9)) {
    response_->connection_info = HttpConnectionInfo::kHTTP0_9;
  } else if (headers->GetHttpVersion() == HttpVersion(1, 0)) {
    response_->connection_info = HttpConnectionInfo::kHTTP1_0;
  } else if (headers->GetHttpVersion() == HttpVersion(1, 1)) {
    response_->connection_info = HttpConnectionInfo::kHTTP1_1;
  }
#endif
  return OK;
}

void HttpStreamParser::CalculateResponseBodySize() {
  if (response_->headers->response_code() / 100 == 1) {
    response_body_length_ = 0;
  } else {
    switch (response_->headers->response_code()) {
      case HTTP_NO_CONTENT:     // No Content
      case HTTP_RESET_CONTENT:  // Reset Content
      case HTTP_NOT_MODIFIED:   // Not Modified
        response_body_length_ = 0;
        break;
    }
  }

  if (method_ == "HEAD") {
    response_body_length_ = 0;
  }

  if (response_body_length_ == -1) {
    // "Transfer-Encoding: chunked" trumps "Content-Length: N"
    if (response_->headers->IsChunkEncoded()) {
      // chunked_decoder_ = std::make_unique<HttpChunkedDecoder>();
      base::Assert(false);
    } else {
      std::optional<int64_t> content_length =
          response_->headers->GetContentLength();
      response_body_length_ = content_length ? *content_length : -1;
      // If response_body_length_ is still -1, then we have to wait
      // for the server to close the connection.
    }
  }
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

int HttpStreamParser::DoReadBodyComplete(int result/*bytes_read*/) {
  // if (result > 0)
  //   received_bytes_ += result;

  // if (result > 0)
  //   response_body_read_ += result;
  
  if (result <= 0 || IsResponseBodyComplete()) {
    // recv返回0表示连接已断开
      io_state_ = STATE_DONE;
  } else {
    // Now waiting for more of the body to be read.
    user_read_buf_ = nullptr;
    user_read_buf_len_ = 0;
  }
  return result;
}

} // namespace net