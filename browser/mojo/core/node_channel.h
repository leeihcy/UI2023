// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_CORE_NODE_CHANNEL_H_
#define MOJO_CORE_NODE_CHANNEL_H_

#include <utility>
#include <vector>
#include "mojo/core/channel.h"

namespace mojo {
namespace core {

// Wraps a Channel to send and receive Node control messages.
class NodeChannel : public Channel::Delegate {
public:
  static Channel::MessagePtr CreateEventMessage(size_t capacity,
                                                size_t payload_size,
                                                void **payload,
                                                size_t num_handles);
};

} // namespace core
}

#endif