#ifndef NET_HTTP_HTTP_STREAM_PARSER_H_
#define NET_HTTP_HTTP_STREAM_PARSER_H_

#include <string>
#include <memory>

#include "net/base/io_buffer.h"
#include "net/http/http_request_headers.h"
#include "url/gurl.h"
#include "net/base/completion_once_callback.h"
#include "net/http/http_response_info.h"

namespace net {
class StreamSocket;

class HttpStreamParser {
public:
  HttpStreamParser(StreamSocket *stream_socket, const GURL& url,
                   const std::string& method, GrowableIOBuffer *read_buffer);
  
  const GURL& url() { return url_; }
  const std::string& method() { return method_; }

  int SendRequest(const std::string& request_line,
                  const HttpRequestHeaders& headers/*,
                  const NetworkTrafficAnnotationTag& traffic_annotation,
                  HttpResponseInfo* response*/,
                  CompletionOnceCallback callback);

  int ReadResponseHeaders(CompletionOnceCallback callback);
  int ReadResponseBody(IOBuffer* buf,
                       int buf_len,
                       CompletionOnceCallback callback);
                       
private:
  enum State {
    // STATE_NONE indicates that this is waiting on an external call before
    // continuing.
    STATE_NONE,
    STATE_SEND_HEADERS,
    STATE_SEND_HEADERS_COMPLETE,
    STATE_SEND_BODY,
    STATE_SEND_BODY_COMPLETE,
    STATE_SEND_REQUEST_READ_BODY_COMPLETE,
    STATE_SEND_REQUEST_COMPLETE,
    STATE_READ_HEADERS,
    STATE_READ_HEADERS_COMPLETE,
    STATE_READ_BODY,
    STATE_READ_BODY_COMPLETE,
    STATE_DONE
  };
  int DoLoop(int result);

  int DoSendHeaders();
  int DoReadHeaders();
  int DoReadHeadersComplete(int result);
  int DoReadBody();
  int DoReadBodyComplete(int result);

  // Handle callbacks.
  void OnIOComplete(int result);

  int HandleReadHeaderResult(int result);
  bool IsResponseBodyComplete() const;
  int FindAndParseResponseHeaders(int new_bytes);
  int ParseResponseHeaders(size_t end_of_header_offset);
  void CalculateResponseBodySize();

private:
  static const int kHeaderBufInitialSize = 4 * 1024;  // 4K
  static const int kMaxHeaderBufSize = kHeaderBufInitialSize * 64;  // 256K

  // Next state of the request, when the current one completes.
  State io_state_ = STATE_NONE;

  StreamSocket* stream_socket_ = nullptr;

  const GURL url_;
  const std::string method_;

  // The request header data.  May include a merged request body.
  std::shared_ptr<DrainableIOBuffer> request_headers_;
  // Size of just the request headers.  May be less than the length of
  // |request_headers_| if the body was merged with the headers.
  int request_headers_length_ = 0;

  // Temporary buffer for reading.
  GrowableIOBuffer* read_buf_;

  // Where the caller wants the body data.
  IOBuffer* user_read_buf_ = nullptr;
  size_t user_read_buf_len_ = 0;

  int64_t response_body_length_ = -1;

  // 指向第一个还没读的字节
  size_t read_buf_unused_offset_ = 0;

  // 记录的是状态行 HTTP/1.1 200 OK 的起始位置。
  size_t response_header_start_offset_ = std::string::npos;

  HttpResponseInfo* response_ = nullptr;

  // Helper if the data is chunked.
  // std::unique_ptr<HttpChunkedDecoder> chunked_decoder_;

  // The callback to notify a user that their request or response is
  // complete or there was an error
  CompletionOnceCallback callback_;

  // Callback to be used when doing IO.
  CompletionOnceCallback io_callback_;

};

};

#endif