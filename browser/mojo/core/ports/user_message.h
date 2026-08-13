// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_CORE_PORTS_USER_MESSAGE_H_
#define MOJO_CORE_PORTS_USER_MESSAGE_H_

#include <stddef.h>

namespace mojo {
namespace core {
namespace ports {

class UserMessage {
 public:
  struct TypeInfo {};

  explicit UserMessage(const TypeInfo* type_info);

  UserMessage(const UserMessage&) = delete;
  UserMessage& operator=(const UserMessage&) = delete;

  virtual ~UserMessage();

  const TypeInfo* type_info() const { return type_info_; }

  // Invoked immediately before the system asks the embedder to forward this
  // message to an external node.
  //
  // Returns |true| if the message is OK to route externally, or |false|
  // otherwise. Returning |false| implies an unrecoverable condition, and the
  // message event will be destroyed without further routing.
  virtual bool WillBeRoutedExternally();

  // Returns the size in bytes of this message iff it's serialized. Zero
  // otherwise.
  virtual size_t GetSizeIfSerialized() const;

 private:
  // `type_info_` is not a raw_ptr<...> for performance reasons (based on
  // analysis of sampling profiler data and tab_search:top100:2020).
  const TypeInfo* const type_info_;
};

}  // namespace ports
}  // namespace core
}  // namespace mojo

#endif  // MOJO_CORE_PORTS_USER_MESSAGE_H_
