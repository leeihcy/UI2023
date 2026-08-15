
#include "mojo/core/ports/event.h"

namespace mojo {
namespace core {
namespace ports {

namespace  {

const size_t kPortsMessageAlignment = 8;

#pragma pack(push, 1)

struct SerializedHeader {
  Event::Type type;
  uint32_t padding;
  PortName port_name;
#ifndef MOJO_BACKWARDS_COMPAT
  PortName from_port;
  uint64_t control_sequence_num;
#endif
};

struct UserMessageEventData {
  uint64_t sequence_num;
  uint32_t num_ports;
  uint32_t padding;
};

struct ObserveProxyEventData {
  NodeName proxy_node_name;
  PortName proxy_port_name;
  NodeName proxy_target_node_name;
  PortName proxy_target_port_name;
};

struct ObserveProxyAckEventData {
  uint64_t last_sequence_num;
};

struct ObserveClosureEventData {
  uint64_t last_sequence_num;
};

struct MergePortEventData {
  PortName new_port_name;
  Event::PortDescriptor new_port_descriptor;
};

struct UserMessageReadAckRequestEventData {
  uint64_t sequence_num_to_acknowledge;
};

struct UserMessageReadAckEventData {
  uint64_t sequence_num_acknowledged;
};

struct UpdatePreviousPeerEventData {
  NodeName new_node_name;
  PortName new_port_name;
};

#pragma pack(pop)
}

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


size_t Event::GetSerializedSize() const {
  return sizeof(SerializedHeader) + GetSerializedDataSize();
}

void Event::Serialize(void* buffer) const {
  auto* header = static_cast<SerializedHeader*>(buffer);
  header->type = type_;
  header->padding = 0;
  header->port_name = port_name_;
#ifndef MOJO_BACKWARDS_COMPAT
  header->from_port = from_port_;
  header->control_sequence_num = control_sequence_num_;
#endif
  SerializeData((header + 1));
}

size_t UserMessageEvent::GetSerializedDataSize() const {
  size_t size = sizeof(UserMessageEventData);
  size_t ports_size =
      sizeof(PortDescriptor) + sizeof(PortName);
  ports_size *= ports_.size();
  return (size + ports_size);
}

void UserMessageEvent::SerializeData(void* buffer) const {
  auto* data = static_cast<UserMessageEventData*>(buffer);
  data->sequence_num = sequence_num_;
  data->num_ports = static_cast<uint32_t>(ports_.size());
  data->padding = 0;

  auto* ports_data = reinterpret_cast<PortDescriptor*>((data + 1));
  std::ranges::copy(port_descriptors_, ports_data);

  auto* port_names_data =
      reinterpret_cast<PortName*>((ports_data + ports_.size()));
  std::ranges::copy(ports_, port_names_data);
}



UserMessageReadAckEvent::UserMessageReadAckEvent(
    const PortName& port_name,
    const PortName& from_port,
    uint64_t control_sequence_num,
    uint64_t sequence_num_acknowledged)
    : Event(Type::kUserMessageReadAck,
            port_name,
            from_port,
            control_sequence_num),
      sequence_num_acknowledged_(sequence_num_acknowledged) {}

UserMessageReadAckEvent::~UserMessageReadAckEvent() = default;

// static
ScopedEvent UserMessageReadAckEvent::Deserialize(const PortName& port_name,
                                                 const PortName& from_port,
                                                 uint64_t control_sequence_num,
                                                 const void* buffer,
                                                 size_t num_bytes) {
  if (num_bytes < sizeof(UserMessageReadAckEventData)) {
    return nullptr;
  }

  const auto* data = static_cast<const UserMessageReadAckEventData*>(buffer);
  return std::make_unique<UserMessageReadAckEvent>(
      port_name, from_port, control_sequence_num,
      data->sequence_num_acknowledged);
}

size_t UserMessageReadAckEvent::GetSerializedDataSize() const {
  return sizeof(UserMessageReadAckEventData);
}

void UserMessageReadAckEvent::SerializeData(void* buffer) const {
  auto* data = static_cast<UserMessageReadAckEventData*>(buffer);
  data->sequence_num_acknowledged = sequence_num_acknowledged_;
}

}
}
}