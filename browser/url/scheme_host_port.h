
#ifndef URL_SCHEME_HOST_PORT_H_
#define URL_SCHEME_HOST_PORT_H_

#include <stdint.h>

#include <compare>
#include <string>
#include <string_view>
#include <utility>
#include "url/gurl.h"

namespace url {

class SchemeHostPort {
public:
  SchemeHostPort();
  explicit SchemeHostPort(const GURL& url);

public:
  uint16_t port_ = 0;
  std::string scheme_;
  std::string host_;
};

}


#endif