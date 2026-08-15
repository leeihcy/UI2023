// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mojo/core/data_pipe_control_message.h"

#ifdef GetMessage
#undef GetMessage
#endif

#include "mojo/core/node_controller.h"
#include "mojo/core/ports/event.h"
#include "mojo/core/user_message_impl.h"
#include "mojo/public/c/system/types.h"

namespace mojo {
namespace core {

void SendDataPipeControlMessage(NodeController* node_controller,
                                const ports::PortRef& port,
                                DataPipeCommand command,
                                uint32_t num_bytes) {
  std::unique_ptr<ports::UserMessageEvent> event;
  MojoResult result = UserMessageImpl::CreateEventForNewSerializedMessage(
      sizeof(DataPipeControlMessage), nullptr, 0, &event);

  DataPipeControlMessage* data = static_cast<DataPipeControlMessage*>(
      event->GetMessage1<UserMessageImpl>()->user_payload());
  data->command = command;
  data->num_bytes = num_bytes;

  int rv = node_controller->SendUserMessage(port, std::move(event));
  if (rv != ports::OK && rv != ports::ERROR_PORT_PEER_CLOSED) {
    base::Assert(false);
  }
}

}  // namespace core
}  // namespace mojo
