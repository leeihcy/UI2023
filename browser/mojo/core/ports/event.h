#ifndef MOJO_CORE_PORTS_EVENT_H_
#define MOJO_CORE_PORTS_EVENT_H_

#include <vector>
#include <span>

#include "mojo/mojo_export.h"
#include "mojo/core/ports/port.h"
#include "mojo/core/ports/user_message.h"

namespace mojo {
namespace core {
namespace ports {

class Event;
using ScopedEvent = std::unique_ptr<Event>;

class MOJO_EXPORT Event {
public:
  enum Type : uint32_t {
    kUserMessage,
  };

  Event(Type type, const PortName &port_name, const PortName &from_port,
        uint64_t control_sequence_num)
      : type_(type), port_name_(port_name), from_port_(from_port),
        control_sequence_num_(control_sequence_num) {}

  Type type() const { return type_; }
  const PortName& port_name() const { return port_name_; }

  template <typename T>
  static std::unique_ptr<T> Cast(ScopedEvent* event) {
    return std::unique_ptr<T>(static_cast<T*>(event->release()));
  }

#pragma pack(push, 1)
  struct PortDescriptor {
    PortDescriptor();

    NodeName peer_node_name;
    PortName peer_port_name;
    NodeName referring_node_name;
    PortName referring_port_name;
    uint64_t next_sequence_num_to_send;
    uint64_t next_sequence_num_to_receive;
    uint64_t last_sequence_num_to_receive;
    bool peer_closed;
    char padding[7];
  };
#pragma pack(pop)

  const PortName& from_port() const { return from_port_; }
  void set_from_port(const PortName& from_port) { from_port_ = from_port; }

private:
  const Type type_;
  PortName port_name_;
  PortName from_port_;
  uint64_t control_sequence_num_;
};


class UserMessageEvent : public Event {
 public:
  explicit UserMessageEvent(size_t num_ports);

  void AttachMessage(std::unique_ptr<UserMessage> message) {
    message_ = std::move(message);
  }

  template <typename T> T *GetMessage() {
    return static_cast<T *>(message_.get());
  }

  template <typename T> const T *GetMessage() const {
    return static_cast<const T *>(message_.get());
  }

  void ReservePorts(size_t num_ports);
  size_t num_ports() const { return ports_.size(); }
  std::span<PortName> ports() { return ports_; }
  std::span<PortDescriptor> port_descriptors() { return port_descriptors_; }

  uint64_t sequence_num() const { return sequence_num_; }
  void set_sequence_num(uint64_t sequence_num) { sequence_num_ = sequence_num; }

  size_t GetSizeIfSerialized() const;

 private:
  uint64_t sequence_num_ = 0;
  std::vector<PortDescriptor> port_descriptors_;
  std::vector<PortName> ports_;
  std::unique_ptr<UserMessage> message_;
};

}  // namespace ports
}  // namespace core
}  // namespace mojo

#endif  // MOJO_CORE_PORTS_EVENT_H_