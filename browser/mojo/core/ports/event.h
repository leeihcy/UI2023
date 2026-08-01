#ifndef MOJO_CORE_PORTS_EVENT_H_
#define MOJO_CORE_PORTS_EVENT_H_

#include "mojo/mojo_export.h"
#include "mojo/core/ports/port.h"

namespace mojo {
namespace core {
namespace ports {

class MOJO_EXPORT Event {
public:
  enum Type : uint32_t {
    kUserMessage,
  };

  Event(Type type, const PortName &port_name, const PortName &from_port,
        uint64_t control_sequence_num)
      : type_(type), port_name_(port_name), from_port_(from_port),
        control_sequence_num_(control_sequence_num) {}

private:
  const Type type_;
  PortName port_name_;
  PortName from_port_;
  uint64_t control_sequence_num_;
};

using ScopedEvent = std::unique_ptr<Event>;

}  // namespace ports
}  // namespace core
}  // namespace mojo

#endif  // MOJO_CORE_PORTS_EVENT_H_