#ifndef MOJO_CORE_PORTS_PORT_REF_H_
#define MOJO_CORE_PORTS_PORT_REF_H_

#include <stdint.h>
#include <memory>
#include "mojo/core/ports/user_data.h"
#include "mojo/core/ports/message_queue.h"
#include "base/log.h"

namespace mojo {
namespace core {
namespace ports {

struct Name {
  constexpr Name(uint64_t v1, uint64_t v2) : v1(v1), v2(v2) {}
  uint64_t v1, v2;
};

struct PortName : Name {
  constexpr PortName() : Name(0, 0) {}
  constexpr PortName(uint64_t v1, uint64_t v2) : Name(v1, v2) {}
};

struct NodeName : Name {
  constexpr NodeName() : Name(0, 0) {}
  constexpr NodeName(uint64_t v1, uint64_t v2) : Name(v1, v2) {}
};

class Port {
public:
 enum State {
    kUninitialized,
    kReceiving,
    kBuffering,
    kProxying,
    kClosed
  };
  Port(uint64_t next_sequence_num_to_send,
       uint64_t next_sequence_num_to_receive) : state(kUninitialized) {
  }
  Port(const Port&) = delete;
  Port& operator=(const Port&) = delete;
  ~Port() {
  }

  State state;

  NodeName peer_node_name;
  PortName peer_port_name;

  NodeName prev_node_name;
  PortName prev_port_name;

  // Next sequence number to send for all event messages.
  uint64_t next_control_sequence_num_to_send;
  uint64_t next_control_sequence_num_to_receive;

  MessageQueue message_queue;

  // Arbitrary user data attached to the Port. In practice, Mojo uses this to
  // stash an observer interface which can be notified about various Port state
  // changes.
  // std::shared_ptr<UserData> user_data;
};

class PortRef {
public:
  PortRef() = default;
  PortRef(const PortName& name, std::shared_ptr<Port> port)    
    : name_(name), port_(std::move(port)) {}

  const PortName& name() const { return name_; }
  bool is_valid() const { return !!port_; }
  Port* port() const { return port_.get(); }
  std::shared_ptr<Port> port_ptr() const { return port_; }

 private:
  PortName name_;

  // 这意味着多个PortRef可以安全地指向同一个Port对象。
  std::shared_ptr<Port> port_;
};

constexpr PortName kInvalidPortName = {0, 0};
constexpr NodeName kInvalidNodeName = {0, 0};

inline bool operator==(const Name& a, const Name& b) {
  return a.v1 == b.v1 && a.v2 == b.v2;
}

inline bool operator!=(const Name& a, const Name& b) {
  return !(a == b);
}

inline bool operator<(const Name& a, const Name& b) {
  return std::tie(a.v1, a.v2) < std::tie(b.v1, b.v2);
}

}
}
}

#endif