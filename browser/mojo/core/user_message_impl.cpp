#include "mojo/core/user_message_impl.h"
#include "mojo/core/channel.h"

namespace mojo {
namespace core {

namespace {
MojoResult CreateOrExtendSerializedEventMessage(
    ports::UserMessageEvent *event, size_t payload_size,
    size_t payload_buffer_size,
    const Dispatcher::DispatcherInTransit *new_dispatchers,
    size_t num_new_dispatchers, Channel::MessagePtr *out_message,
    void **out_header, size_t *out_header_size, void **out_user_payload) {
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

}
}