// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_CORE_USER_MESSAGE_IMPL_H_
#define MOJO_CORE_USER_MESSAGE_IMPL_H_

#include <cstdint>
#include <memory>
#include "mojo/core/ports/user_message.h"
#include "mojo/public/c/system/types.h"
#include "mojo/core/ports/event.h"
#include "mojo/core/dispatcher.h"
#include "mojo/core/channel.h"

namespace mojo {
namespace core {

inline constexpr uint32_t kMinimumPayloadBufferSize = 128;

class UserMessageImpl : public ports::UserMessage {
 public:
   static const TypeInfo kUserMessageTypeInfo;

   static MojoResult CreateEventForNewSerializedMessage(
      uint32_t num_bytes,
      const Dispatcher::DispatcherInTransit* dispatchers,
      uint32_t num_dispatchers,
      std::unique_ptr<ports::UserMessageEvent>* out_event);

  UserMessageImpl(ports::UserMessageEvent* message_event,
                  Channel::MessagePtr channel_message,
                  void* header,
                  size_t header_size,
                  void* user_payload,
                  size_t user_payload_size);

  void *user_payload() { return user_payload_; }

  const void *user_payload() const { return user_payload_; }
  size_t num_handles() const;

  size_t user_payload_size() const {
    // DCHECK(IsSerialized());
    return user_payload_size_;
  }

private:
  ports::UserMessageEvent* const message_event_;
  Channel::MessagePtr channel_message_;
  
  const bool unlimited_size_ = false;
  bool has_serialized_handles_ = false;
  bool is_committed_ = false;

  void* header_ = nullptr;
  size_t header_size_ = 0;
  void *user_payload_ = nullptr;
  size_t user_payload_size_ = 0;
};

}  // namespace core
}  // namespace mojo

#endif  // MOJO_CORE_USER_MESSAGE_IMPL_H_
