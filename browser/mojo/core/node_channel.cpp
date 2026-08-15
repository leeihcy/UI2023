
#include "mojo/core/node_channel.h"

#ifdef max
#undef max
#endif

namespace mojo {
namespace core {

namespace {

// NOTE: Please ONLY append messages to the end of this enum.
enum class MessageType : uint32_t {
  ACCEPT_INVITEE,
  ACCEPT_INVITATION,
  ADD_BROKER_CLIENT,
  BROKER_CLIENT_ADDED,
  ACCEPT_BROKER_CLIENT,
  EVENT_MESSAGE,
  REQUEST_PORT_MERGE,
  REQUEST_INTRODUCTION,
  INTRODUCE,
#if defined(OS_WIN)
  RELAY_EVENT_MESSAGE,
#endif
  BROADCAST_EVENT,
#if defined(OS_WIN)
  EVENT_MESSAGE_FROM_RELAY,
#endif
  ACCEPT_PEER,
  BIND_BROKER_HOST,
};

#pragma pack(push, 1)

struct alignas(8) Header {
  MessageType type;
};


#pragma pack(pop)

Channel::MessagePtr CreateMessage(MessageType type,
                                  size_t payload_size,
                                  size_t num_handles,
                                  void** out_data,
                                  size_t capacity = 0) {
  const size_t total_size = payload_size + sizeof(Header);
  if (capacity == 0) {
    capacity = total_size;
  } else {
    capacity = std::max(total_size, capacity);
  }
  auto message =
      Channel::Message::CreateMessage(capacity, total_size, num_handles);
  Header* header = reinterpret_cast<Header*>(message->mutable_payload());

  // Make sure any header padding gets zeroed.
  (memset(header, 0, sizeof(Header)));
  header->type = type;

  // The out_data starts beyond the header.
  *out_data = (reinterpret_cast<void*>(header + 1));
  return message;
}

template <typename DataType>
Channel::MessagePtr CreateMessage(MessageType type,
                                  size_t payload_size,
                                  size_t num_handles,
                                  DataType** out_data,
                                  size_t capacity = 0) {
  auto msg_ptr = CreateMessage(type, payload_size, num_handles,
                               reinterpret_cast<void**>(out_data), capacity);

  // Since we know the type let's make sure any padding areas are zeroed.
  (memset(*out_data, 0, sizeof(DataType)));

  return msg_ptr;
}

}

// static
Channel::MessagePtr NodeChannel::CreateEventMessage(size_t capacity,
                                                    size_t payload_size,
                                                    void** payload,
                                                    size_t num_handles) {
  return CreateMessage(MessageType::EVENT_MESSAGE, payload_size, num_handles,
                       payload, capacity);
}

}
}