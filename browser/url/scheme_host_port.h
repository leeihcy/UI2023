
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
  explicit SchemeHostPort(const GURL &url);

  const std::string& host() const { return host_; }
  const std::string& scheme() const { return scheme_; }
  uint16_t port() const { return port_; }

  std::string Serialize() const;
  
  friend bool operator==(const SchemeHostPort &left,
                         const SchemeHostPort &right) = default;
  friend auto operator<=>(const SchemeHostPort &left,
                          const SchemeHostPort &right) = default;

public:
  uint16_t port_ = 0;
  std::string scheme_;
  std::string host_;
};

}


#endif