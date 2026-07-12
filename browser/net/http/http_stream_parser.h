#ifndef NET_HTTP_HTTP_STREAM_PARSER_H_
#define NET_HTTP_HTTP_STREAM_PARSER_H_

#include <string>
#include <memory>

#include "net/base/io_buffer.h"
#include "net/http/http_request_headers.h"
#include "url/gurl.h"
#include "net/base/completion_once_callback.h"

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
                  HttpResponseInfo* response,
                  CompletionOnceCallback callback*/);

  int ReadResponseHeaders(CompletionOnceCallback callback);
  int ReadResponseBody(IOBuffer* buf,
                       int buf_len,
                       CompletionOnceCallback callback);
                       
private:
  int DoSendHeaders();
  int DoReadHeaders();
  int DoReadBody();
  int DoReadBodyComplete(int result);

  // Handle callbacks.
  void OnIOComplete(int result);

  bool IsResponseBodyComplete() const;

private:
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

  // Offset of the first unused byte in |read_buf_|.  May be nonzero due to
  // body data in the same packet as header data but is zero when reading
  // headers.
  size_t read_buf_unused_offset_ = 0;

  // The callback to notify a user that their request or response is
  // complete or there was an error
  CompletionOnceCallback callback_;

  // Callback to be used when doing IO.
  CompletionOnceCallback io_callback_;

};

};

#endif