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

}