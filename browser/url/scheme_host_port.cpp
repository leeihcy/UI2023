#include "url/scheme_host_port.h"
#include <assert.h>

namespace url {
  
SchemeHostPort::SchemeHostPort() = default;

SchemeHostPort::SchemeHostPort(const GURL& url) {
  if (!url.is_valid())
    return;

  std::string_view scheme = url.scheme();
  std::string_view host = url.host();

  // A valid GURL never returns PORT_INVALID.
  int port = url.EffectiveIntPort();
  if (port == PORT_UNSPECIFIED) {
    port = 0;
  }

  // if (ShouldDiscardHostAndPort(scheme)) {
  //   host = "";
  //   port = 0;
  // }
  // 
  // if (!IsValidInput(scheme, host, port, ALREADY_CANONICALIZED))
  //   return;

  scheme_ = std::string(scheme);
  host_ = std::string(host);
  port_ = port;
}

std::string SchemeHostPort::Serialize() const {
  std::string result;

  // Reserve enough space for the "normal" case of scheme://host/.
  result.reserve(scheme_.size() + host_.size() + 4);

  if (!scheme_.empty()) {
    result.append(scheme_);
  }
  if (!host_.empty()) {
    result.append(host_);
  }
  if (port_ > 0 && port_ < 65535) {
    result.push_back(':');

    char text[8];
    sprintf(text, "%d", port_);
    result.append(text);
  }
  return result;
}

}