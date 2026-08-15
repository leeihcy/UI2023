#include "mojo/core/channel.h"

#include <algorithm>

namespace mojo::core {

namespace {
Channel::AlignedBuffer MakeAlignedBuffer(size_t size) {
  auto buffer = Channel::AlignedBuffer(size);
  return buffer;
}
}

size_t Channel::Message::num_handles() const {
  return is_legacy_message() ? legacy_header()->num_handles
                             : header()->num_handles;
}

bool Channel::Message::is_legacy_message() const {
  return legacy_header()->message_type == MessageType::NORMAL_LEGACY;
}
Channel::Message::LegacyHeader* Channel::Message::legacy_header() {
  return reinterpret_cast<LegacyHeader*>(mutable_data());
}
const Channel::Message::LegacyHeader* Channel::Message::legacy_header() const {
  return reinterpret_cast<const LegacyHeader*>(data());
}
Channel::Message::Header* Channel::Message::header() {
  return reinterpret_cast<Header*>(mutable_data());
}
const Channel::Message::Header* Channel::Message::header() const {
  return reinterpret_cast<const Header*>(data());
}


void* Channel::Message::mutable_payload() {
  return mutable_payload_span().data();
}

std::span<char> Channel::Message::mutable_payload_span() {
  if (is_legacy_message()) {
    return mutable_data_span().subspan(sizeof(LegacyHeader));
  }
  return mutable_data_span().subspan(header()->num_header_bytes);
}

const void* Channel::Message::payload() const {
  return payload_span().data();
}

std::span<const char> Channel::Message::payload_span() const {
  if (is_legacy_message()) {
    return data_span().subspan(sizeof(LegacyHeader));
  }
  return data_span().subspan(header()->num_header_bytes);
}

size_t Channel::Message::payload_size() const {
  if (is_legacy_message()) {
    return legacy_header()->num_bytes - sizeof(LegacyHeader);
  }
  return size_ - header()->num_header_bytes;
}

const void* Channel::Message::extra_header() const {
  // DCHECK(!is_legacy_message());
  return data_span().subspan(sizeof(Header)).data();
}

void* Channel::Message::mutable_extra_header() {
  return mutable_extra_header_span().data();
}

std::span<char> Channel::Message::mutable_extra_header_span() {
  // DCHECK(!is_legacy_message());
  return mutable_data_span().subspan(sizeof(Header));
}

size_t Channel::Message::extra_header_size() const {
  return header()->num_header_bytes - sizeof(Header);
}


// A complex message can be large or contain file handles. Not used with ipcz.
struct ComplexMessage final : public Channel::Message {
  ComplexMessage() = default;
  ComplexMessage(size_t capacity,
                 size_t max_handles,
                 size_t payload_size,
                 MessageType message_type);

  ComplexMessage(const ComplexMessage&) = delete;
  ComplexMessage& operator=(const ComplexMessage&) = delete;

  ~ComplexMessage() override = default;

  // Message impl:
  void SetHandles(std::vector<PlatformHandle> new_handles) override;
  void SetHandles(std::vector<PlatformHandleInTransit> new_handles) override;
  std::vector<PlatformHandleInTransit> TakeHandles() override;

  std::span<const char> data_span() const override { return data_; }
  std::span<char> mutable_data_span() override { return data_; }
  size_t capacity() const override;

  // bool ExtendPayload(size_t new_payload_size) override;

 private:
  friend struct Channel::Message;

  // The message data buffer.
  Channel::AlignedBuffer data_;

  // The capacity of the buffer at |data_|.
  size_t capacity_ = 0;

  // Maximum number of handles which may be attached to this message.
  size_t max_handles_ = 0;

  std::vector<PlatformHandleInTransit> handle_vector_;

#if defined (OS_WIN)
  // On Windows, handles are serialised into the extra header section.
  HandleEntry* handles_ = nullptr;
#endif
};


ComplexMessage::ComplexMessage(size_t capacity,
                               size_t payload_size,
                               size_t max_handles,
                               MessageType message_type)
    : max_handles_(max_handles) {
  const bool is_legacy_message = (message_type == MessageType::NORMAL_LEGACY);
  size_t extra_header_size = 0;
#if BUILDFLAG(IS_WIN)
  // On Windows we serialize HANDLEs into the extra header space.
  extra_header_size = max_handles_ * sizeof(HandleEntry);
#endif
  // Pad extra header data to be aliged to |kChannelMessageAlignment| bytes.
  if (!IsAlignedForChannelMessage(extra_header_size)) {
    extra_header_size += kChannelMessageAlignment -
                         (extra_header_size % kChannelMessageAlignment);
  }
  const size_t header_size =
      is_legacy_message ? sizeof(LegacyHeader) : sizeof(Header);

  capacity_ = header_size + extra_header_size + capacity;
  size_ = header_size + extra_header_size + payload_size;
  data_ = MakeAlignedBuffer(capacity_);
  // Only zero out the header and not the payload. Since the payload is going to
  // be memcpy'd, zeroing the payload is unnecessary work and a significant
  // performance issue when dealing with large messages. Any sanitizer errors
  // complaining about an uninitialized read in the payload area should be
  // treated as an error and fixed.
  std::ranges::fill(mutable_data_span().first(header_size + extra_header_size),
                    0);

  legacy_header()->num_bytes = static_cast<uint32_t>(size_);

  legacy_header()->message_type = message_type;

  if (is_legacy_message) {
    legacy_header()->num_handles = static_cast<uint16_t>(max_handles);
  } else {
    header()->num_header_bytes =
        static_cast<uint16_t>(header_size + extra_header_size);
  }

  if (max_handles_ > 0) {
#if BUILDFLAG(IS_WIN)
    handles_ = reinterpret_cast<HandleEntry*>(mutable_extra_header());
    // Initialize all handles to invalid values.
    for (size_t i = 0; i < max_handles_; ++i) {
      (handles_[i]).handle = -1; //INVALID_HANDLE_VALUE;
    }
#endif
  }
}

size_t ComplexMessage::capacity() const {
  if (is_legacy_message()) {
    return capacity_ - sizeof(LegacyHeader);
  }
  return capacity_ - header()->num_header_bytes;
}


void ComplexMessage::SetHandles(std::vector<PlatformHandle> new_handles) {
  std::vector<PlatformHandleInTransit> handles;
  handles.reserve(new_handles.size());
  for (auto& h : new_handles) {
    handles.emplace_back(PlatformHandleInTransit(std::move(h)));
  }
  SetHandles(std::move(handles));
}

void ComplexMessage::SetHandles(
    std::vector<PlatformHandleInTransit> new_handles) {
  if (is_legacy_message()) {
    // Old semantics for ChromeOS and Android
    if (legacy_header()->num_handles == 0) {
      return;
    }
    std::swap(handle_vector_, new_handles);
    return;
  }

  if (max_handles_ == 0) {
    return;
  }

  header()->num_handles = static_cast<uint16_t>(new_handles.size());
  std::swap(handle_vector_, new_handles);
#if defined (OS_WIN)
  (memset(handles_, 0, extra_header_size()));
  for (size_t i = 0; i < handle_vector_.size(); i++) {
    HANDLE handle = handle_vector_[i].remote_handle();
    if (handle == INVALID_HANDLE_VALUE) {
      handle = handle_vector_[i].handle().GetHandle().Get();
    }
    (handles_[i]).handle = (uint32_t)handle;
  }
#endif  // BUILDFLAG(IS_WIN)

}

std::vector<PlatformHandleInTransit> ComplexMessage::TakeHandles() {
  return std::move(handle_vector_);
}


// static
Channel::MessagePtr Channel::Message::CreateMessage(size_t payload_size,
                                                    size_t max_handles) {
  return CreateMessage(payload_size, payload_size, max_handles);
}

// static
Channel::MessagePtr Channel::Message::CreateMessage(size_t payload_size,
                                                    size_t max_handles,
                                                    MessageType message_type) {
  return CreateMessage(payload_size, payload_size, max_handles, message_type);
}

// static
Channel::MessagePtr Channel::Message::CreateMessage(size_t capacity,
                                                    size_t payload_size,
                                                    size_t max_handles) {
#if defined(MOJO_CORE_LEGACY_PROTOCOL)
  return CreateMessage(capacity, payload_size, max_handles,
                       Message::MessageType::NORMAL_LEGACY);
#else
  return CreateMessage(capacity, payload_size, max_handles,
                       Message::MessageType::NORMAL);
#endif
}

// static
Channel::MessagePtr Channel::Message::CreateMessage(size_t capacity,
                                                    size_t payload_size,
                                                    size_t max_handles,
                                                    MessageType message_type) {
  return std::unique_ptr<Channel::Message>(
      new ComplexMessage(capacity, payload_size, max_handles, message_type));
}


}
