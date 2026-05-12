#ifndef NET_BASE_HOST_PORT_PAIR_H_
#define NET_BASE_HOST_PORT_PAIR_H_

#include <stdint.h>

#include <optional>
#include <string>
#include <string_view>
#include <tuple>

namespace net {

class HostPortPair {
public:
  uint16_t port_;
  std::string host_;
};

}

#endif