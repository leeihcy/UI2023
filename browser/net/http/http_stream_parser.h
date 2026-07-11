#ifndef NET_HTTP_HTTP_STREAM_PARSER_H_
#define NET_HTTP_HTTP_STREAM_PARSER_H_

#include <string>
#include "url/gurl.h"

namespace net {
class StreamSocket;

class HttpStreamParser {
public:
  HttpStreamParser(StreamSocket *stream_socket, const GURL& url,
                   const std::string& method);
  
  const GURL& url() { return url_; }
  const std::string& method() { return method_; }

  int SendRequest(const std::string& request_line/*,
                  const HttpRequestHeaders& headers,
                  const NetworkTrafficAnnotationTag& traffic_annotation,
                  HttpResponseInfo* response,
                  CompletionOnceCallback callback*/);

private:
  int DoSendHeaders();


private:
  StreamSocket* stream_socket_ = nullptr;

  const GURL url_;
  const std::string method_;
};

};

#endif