
#include "mojo/core/ports/event.h"

namespace mojo {
namespace core {
namespace ports {

Event::PortDescriptor::PortDescriptor() {
  memset(padding, 0, sizeof(padding));
}

UserMessageEvent::UserMessageEvent(size_t num_ports)
    : Event(Type::kUserMessage, kInvalidPortName, kInvalidPortName, -1) {
  ReservePorts(num_ports);
}

void UserMessageEvent::ReservePorts(size_t num_ports) {
  port_descriptors_.resize(num_ports);
  ports_.resize(num_ports);
}

size_t UserMessageEvent::GetSizeIfSerialized() const {
  if (!message_) {
    return 0;
  }
  return message_->GetSizeIfSerialized();
}

}
}
}