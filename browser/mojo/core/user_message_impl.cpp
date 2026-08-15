#include "mojo/core/user_message_impl.h"
#include "mojo/core/channel.h"
#include "mojo/core/node_channel.h"
#include "mojo/public/cpp/platform/platform_handle.h"

namespace mojo {
namespace core {

namespace {

// The maximum number of Mojo handles which can be attached to a serialized
// user message. Much larger than should ever be necessary, but small enough
// to not be a problem.
const uint32_t kMaxMojoHandleAttachments = 1024 * 1024;

// Indicates whether handle serialization failure should be emulated in testing.
bool g_always_fail_handle_serialization = false;

#pragma pack(push, 1)
// Header attached to every message.
struct MessageHeader {
  // The number of serialized dispatchers included in this header.
  uint32_t num_dispatchers;

  // Total size of the header, including serialized dispatcher data.
  uint32_t header_size;
};

// Header for each dispatcher in a message, immediately following the message
// header.
struct DispatcherHeader {
  // The type of the dispatcher, correpsonding to the Dispatcher::Type enum.
  int32_t type;

  // The size of the serialized dispatcher, not including this header.
  uint32_t num_bytes;

  // The number of ports needed to deserialize this dispatcher.
  uint32_t num_ports;

  // The number of platform handles needed to deserialize this dispatcher.
  uint32_t num_platform_handles;
};
#pragma pack(pop)

MojoResult CreateOrExtendSerializedEventMessage(
    ports::UserMessageEvent *event, size_t payload_size,
    size_t payload_buffer_size,
    const Dispatcher::DispatcherInTransit *new_dispatchers,
    size_t num_new_dispatchers, Channel::MessagePtr *out_message,
    void **out_header, size_t *out_header_size, void **out_user_payload) {
 // A structure for tracking information about every Dispatcher that will be
  // serialized into the message. This is NOT part of the message itself.
  struct DispatcherInfo {
    uint32_t num_bytes;
    uint32_t num_ports;
    uint32_t num_handles;
  };

  size_t original_header_size = sizeof(MessageHeader);
  size_t original_num_ports = 0;
  size_t original_num_handles = 0;
  size_t original_payload_size = 0;
  MessageHeader* original_header = nullptr;
  void* original_user_payload = nullptr;
  Channel::MessagePtr original_message;
  if (*out_message) {
    original_message = std::move(*out_message);
    original_header = static_cast<MessageHeader*>(*out_header);
    original_header_size = *out_header_size;
    original_num_ports = event->num_ports();
    original_num_handles = original_message->num_handles();
    original_user_payload = *out_user_payload;
    original_payload_size =
        original_message->payload_size() -
        (static_cast<char*>(original_user_payload) -
         static_cast<char*>(original_message->mutable_payload()));
  }

  // This is only the base header size. It will grow as we accumulate the
  // size of serialized state for each dispatcher.
  size_t safe_header_size = num_new_dispatchers;
  safe_header_size *= sizeof(DispatcherHeader);
  safe_header_size += original_header_size;
  size_t header_size = safe_header_size; //.ValueOrDie();
  size_t num_new_ports = 0;
  size_t num_new_handles = 0;
  std::vector<DispatcherInfo> new_dispatcher_info(num_new_dispatchers);
  for (size_t i = 0; i < num_new_dispatchers; ++i) {
    Dispatcher* d = new_dispatchers[i].dispatcher.get();
    d->StartSerialize(&new_dispatcher_info[i].num_bytes,
                      &new_dispatcher_info[i].num_ports,
                      &new_dispatcher_info[i].num_handles);
    header_size += new_dispatcher_info[i].num_bytes;
    num_new_ports += new_dispatcher_info[i].num_ports;
    num_new_handles += new_dispatcher_info[i].num_handles;
  }

  size_t num_ports = original_num_ports + num_new_ports;
  size_t num_handles = original_num_handles + num_new_handles;

  // We now have enough information to fully allocate the message storage.
  if (num_ports > event->num_ports()) {
    event->ReservePorts(num_ports);
  }
  const size_t event_size = event->GetSerializedSize();
  const size_t total_size = event_size + header_size + payload_size;
  const size_t total_buffer_size =
      event_size + header_size + payload_buffer_size;
  void* data;
  Channel::MessagePtr message = NodeChannel::CreateEventMessage(
      total_buffer_size, total_size, &data, num_handles);
  auto* header = reinterpret_cast<MessageHeader*>(
      static_cast<uint8_t*>(data) + event_size);

  // Populate the message header with information about serialized dispatchers.
  // The front of the message is always a MessageHeader followed by a
  // DispatcherHeader for each dispatcher to be sent.
  DispatcherHeader* new_dispatcher_headers;
  char* new_dispatcher_data;
  size_t total_num_dispatchers = num_new_dispatchers;
  std::vector<PlatformHandle> handles;
  if (original_message) {
    size_t original_dispatcher_headers_size =
        original_header->num_dispatchers * sizeof(DispatcherHeader);
    (
        memcpy(header, original_header,
               original_dispatcher_headers_size + sizeof(MessageHeader)));
    new_dispatcher_headers = reinterpret_cast<DispatcherHeader*>(
        (reinterpret_cast<uint8_t*>(header + 1) +
                    original_dispatcher_headers_size));
    total_num_dispatchers += original_header->num_dispatchers;
    size_t total_dispatcher_headers_size =
        total_num_dispatchers * sizeof(DispatcherHeader);
    char* original_dispatcher_data =
        (reinterpret_cast<char*>(original_header + 1) +
                    original_dispatcher_headers_size);
    char* dispatcher_data = (reinterpret_cast<char*>(header + 1) +
                                        total_dispatcher_headers_size);
    size_t original_dispatcher_data_size = original_header_size -
                                           sizeof(MessageHeader) -
                                           original_dispatcher_headers_size;
    (memcpy(dispatcher_data, original_dispatcher_data,
                       original_dispatcher_data_size));
    new_dispatcher_data =
        (dispatcher_data + original_dispatcher_data_size);
    auto handles_in_transit = original_message->TakeHandles();
    if (!handles_in_transit.empty()) {
      handles.resize(num_handles);
      for (size_t i = 0; i < handles_in_transit.size(); ++i) {
        handles[i] = handles_in_transit[i].TakeHandle();
      }
    }
    
    memcpy(reinterpret_cast<char*>(header) + header_size,
               reinterpret_cast<char*>(original_header) + original_header_size,
               original_payload_size);
  } else {
    new_dispatcher_headers =
        reinterpret_cast<DispatcherHeader*>((header + 1));
    // Serialized dispatcher state immediately follows the series of
    // DispatcherHeaders.
    new_dispatcher_data = reinterpret_cast<char*>(
        (new_dispatcher_headers + num_new_dispatchers));
  }

  if (handles.empty() && num_new_handles) {
    handles.resize(num_new_handles);
  }

  header->num_dispatchers = uint32_t(total_num_dispatchers);

  // |header_size| is the total number of bytes preceding the message payload,
  // including all dispatcher headers and serialized dispatcher state.
  // if (!base::IsValueInRangeForNumericType<uint32_t>(header_size)) {
  //   return MOJO_RESULT_OUT_OF_RANGE;
  // }

  header->header_size = static_cast<uint32_t>(header_size);

  if (num_new_dispatchers > 0) {
    size_t port_index = original_num_ports;
    size_t handle_index = original_num_handles;
    bool fail = false;
    for (size_t i = 0; i < num_new_dispatchers; ++i) {
      Dispatcher* d = (new_dispatchers[i]).dispatcher.get();
      DispatcherHeader* dh = (&new_dispatcher_headers[i]);
      const DispatcherInfo& info = new_dispatcher_info[i];

      // Fill in the header for this dispatcher.
      dh->type = static_cast<int32_t>(d->GetType());
      dh->num_bytes = info.num_bytes;
      dh->num_ports = info.num_ports;
      dh->num_platform_handles = info.num_handles;

      // Fill in serialized state, ports, and platform handles. We'll cancel
      // the send if the dispatcher implementation rejects for some reason.
      if (g_always_fail_handle_serialization ||
          !d->EndSerialize(static_cast<void*>(new_dispatcher_data),
                           event->ports().subspan(port_index).data(),
                           !handles.empty()
                               ? (handles.data() + handle_index)
                               : nullptr)) {
        fail = true;
        break;
      }

      (new_dispatcher_data += info.num_bytes);
      port_index += info.num_ports;
      handle_index += info.num_handles;
    }

    if (fail) {
      // Release any platform handles we've accumulated. Their dispatchers
      // retain ownership when message creation fails, so these are not actually
      // leaking.
      for (auto& handle : handles) {
        handle.release();
      }

      // Leave the original message in place on failure if applicable.
      if (original_message) {
        *out_message = std::move(original_message);
      }
      return MOJO_RESULT_INVALID_ARGUMENT;
    }

    // Take ownership of all the handles and move them into message storage.
    message->SetHandles(std::move(handles));
  }

  *out_message = std::move(message);
  *out_header = header;
  *out_header_size = header_size;
  *out_user_payload =
      (reinterpret_cast<uint8_t*>(header) + header_size);
  return MOJO_RESULT_OK;
}
} // namespace

const ports::UserMessage::TypeInfo UserMessageImpl::kUserMessageTypeInfo = {};

UserMessageImpl::UserMessageImpl(ports::UserMessageEvent* message_event,
                                 Channel::MessagePtr channel_message,
                                 void* header,
                                 size_t header_size,
                                 void* user_payload,
                                 size_t user_payload_size)
    : ports::UserMessage(&kUserMessageTypeInfo),
      message_event_(message_event),
      channel_message_(std::move(channel_message)),
      has_serialized_handles_(true),
      is_committed_(true),
      header_(header),
      header_size_(header_size),
      user_payload_(user_payload),
      user_payload_size_(user_payload_size) 
      {
  // EnsureMemoryDumpProviderExists();
  // IncrementMessageCount();
}

// static
MojoResult UserMessageImpl::CreateEventForNewSerializedMessage(
    uint32_t num_bytes,
    const Dispatcher::DispatcherInTransit* dispatchers,
    uint32_t num_dispatchers,
    std::unique_ptr<ports::UserMessageEvent>* out_event) {
  Channel::MessagePtr channel_message;
  void* header = nullptr;
  void* user_payload = nullptr;
  auto event = std::make_unique<ports::UserMessageEvent>(0);
  size_t header_size = 0;
  MojoResult rv = CreateOrExtendSerializedEventMessage(
      event.get(), num_bytes, num_bytes, dispatchers, num_dispatchers,
      &channel_message, &header, &header_size, &user_payload);
  if (rv != MOJO_RESULT_OK) {
    return rv;
  }
  event->AttachMessage(std::make_unique<UserMessageImpl>(event.get(), std::move(channel_message), header,
                          header_size, user_payload, num_bytes));
  *out_event = std::move(event);
  return MOJO_RESULT_OK;
}


size_t UserMessageImpl::num_handles() const {
  return static_cast<const MessageHeader*>(header_)->num_dispatchers;
}

}
}