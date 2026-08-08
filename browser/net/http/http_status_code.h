#ifndef NET_HTTP_HTTP_STATUS_CODE_H_
#define NET_HTTP_HTTP_STATUS_CODE_H_

namespace net {

// HTTP status codes.
enum HttpStatusCode {

#define HTTP_STATUS_ENUM_VALUE(label, code, reason) HTTP_##label = code,
#include "net/http/http_status_code_list.h"
#undef HTTP_STATUS_ENUM_VALUE

};

}

#endif