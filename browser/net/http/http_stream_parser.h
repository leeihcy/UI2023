#ifndef NET_HTTP_HTTP_STREAM_PARSER_H_
#define NET_HTTP_HTTP_STREAM_PARSER_H_

#include <string>

namespace net {

class HttpStreamParser {
public:
  HttpStreamParser();
  
  int SendRequest(const std::string& request_line/*,
                  const HttpRequestHeaders& headers,
                  const NetworkTrafficAnnotationTag& traffic_annotation,
                  HttpResponseInfo* response,
                  CompletionOnceCallback callback*/);

private:
  int DoSendHeaders();

};

};

#endif