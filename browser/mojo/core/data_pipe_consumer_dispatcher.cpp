#include "mojo/core/data_pipe_consumer_dispatcher.h"

#include "mojo/core/node_controller.h"

#ifdef min
#undef min
#endif
#include <algorithm>

namespace mojo {
namespace core {

  DataPipeConsumerDispatcher::DataPipeConsumerDispatcher(
    NodeController* node_controller,
    const ports::PortRef& control_port,
    base::UnsafeSharedMemoryRegion shared_ring_buffer,
    const MojoCreateDataPipeOptions& options,
    uint64_t pipe_id)
    : options_(options),
      node_controller_(node_controller),
      control_port_(control_port),
      pipe_id_(pipe_id),
      // watchers_(this),
      shared_ring_buffer_(std::move(shared_ring_buffer)) {}

Dispatcher::Type DataPipeConsumerDispatcher::GetType() const {
  return Type::DATA_PIPE_CONSUMER;
}

MojoResult DataPipeConsumerDispatcher::Close() {
  // base::AutoLock lock(lock_);
  // DVLOG(1) << "Closing data pipe consumer " << pipe_id_;
  return CloseNoLock();
}

bool DataPipeConsumerDispatcher::InitializeNoLock() { 
  // lock_.AssertAcquired();
  if (!shared_ring_buffer_.IsValid()) {
    return false;
  }

  ring_buffer_mapping_ = shared_ring_buffer_.Map();
  if (!ring_buffer_mapping_.IsValid()) {
    shared_ring_buffer_ = base::UnsafeSharedMemoryRegion();
    return false;
  }

  // base::AutoUnlock unlock(lock_);
  node_controller_->SetPortObserver(
      control_port_, std::make_shared<PortObserverThunk>(this));

  return true;
}

MojoResult DataPipeConsumerDispatcher::CloseNoLock() {
#if 0
    lock_.AssertAcquired();
  if (is_closed_ || in_transit_) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }
  is_closed_ = true;
  ring_buffer_mapping_ = base::WritableSharedMemoryMapping();
  shared_ring_buffer_ = base::UnsafeSharedMemoryRegion();

  watchers_.NotifyClosed();
  if (!transferred_) {
    base::AutoUnlock unlock(lock_);
    node_controller_->ClosePort(control_port_);
  }
#endif
  return MOJO_RESULT_OK;
}

MojoResult DataPipeConsumerDispatcher::ReadData(
    const MojoReadDataOptions &options, void *elements,
    uint32_t *num_bytes) {
  // base::AutoLock lock(lock_);

  if (!shared_ring_buffer_.IsValid() || in_transit_) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }

  if (in_two_phase_read_) {
    return MOJO_RESULT_BUSY;
  }

  const bool had_new_data = new_data_available_;
  new_data_available_ = false;

  if ((options.flags & MOJO_READ_DATA_FLAG_QUERY)) {
    if ((options.flags & MOJO_READ_DATA_FLAG_PEEK) ||
        (options.flags & MOJO_READ_DATA_FLAG_DISCARD)) {
      return MOJO_RESULT_INVALID_ARGUMENT;
    }
    *num_bytes = static_cast<uint32_t>(bytes_available_);

    // if (had_new_data) {
    //   watchers_.NotifyState(GetHandleSignalsStateNoLock());
    // }
    return MOJO_RESULT_OK;
  }

  bool discard = false;
  if ((options.flags & MOJO_READ_DATA_FLAG_DISCARD)) {
    // These flags are mutally exclusive.
    if (options.flags & MOJO_READ_DATA_FLAG_PEEK) {
      return MOJO_RESULT_INVALID_ARGUMENT;
    }
    discard = true;
  }

  uint32_t max_num_bytes_to_read = *num_bytes;
  if (max_num_bytes_to_read % options_.element_num_bytes != 0) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }

  bool all_or_none = options.flags & MOJO_READ_DATA_FLAG_ALL_OR_NONE;
  uint32_t min_num_bytes_to_read = all_or_none ? max_num_bytes_to_read : 0;

  if (min_num_bytes_to_read > bytes_available_) {
    // if (had_new_data) {
    //   watchers_.NotifyState(GetHandleSignalsStateNoLock());
    // }
    return peer_closed_ ? MOJO_RESULT_FAILED_PRECONDITION
                        : MOJO_RESULT_OUT_OF_RANGE;
  }

  uint32_t bytes_to_read = std::min(max_num_bytes_to_read, bytes_available_);
  if (bytes_to_read == 0) {
    // if (had_new_data) {
    //   watchers_.NotifyState(GetHandleSignalsStateNoLock());
    // }
    return peer_closed_ ? MOJO_RESULT_FAILED_PRECONDITION
                        : MOJO_RESULT_SHOULD_WAIT;
  }

  if (!discard) {
    const uint8_t* data =
        static_cast<const uint8_t*>(ring_buffer_mapping_.memory());

    uint8_t* destination = static_cast<uint8_t*>(elements);

    uint32_t tail_bytes_to_copy =
        std::min(options_.capacity_num_bytes - read_offset_, bytes_to_read);
    uint32_t head_bytes_to_copy = bytes_to_read - tail_bytes_to_copy;
    if (tail_bytes_to_copy > 0) {
      memcpy(destination, data + read_offset_, tail_bytes_to_copy);
    }
    if (head_bytes_to_copy > 0) {
          memcpy(destination + tail_bytes_to_copy, data, head_bytes_to_copy);
    }
  }
  *num_bytes = bytes_to_read;

  bool peek = !!(options.flags & MOJO_READ_DATA_FLAG_PEEK);
  if (discard || !peek) {
    read_offset_ = (read_offset_ + bytes_to_read) % options_.capacity_num_bytes;
    bytes_available_ -= bytes_to_read;

    // base::AutoUnlock unlock(lock_);
    // NotifyRead(bytes_to_read);
  }

  // We may have just read the last available data and thus changed the signals
  // state.
  // watchers_.NotifyState(GetHandleSignalsStateNoLock());

  return MOJO_RESULT_OK;
}

/**
>	mojo_core_embedder_internal.dll!mojo::core::DataPipeConsumerDispatcher::BeginReadData	C++
 	mojo_core_embedder_internal.dll!mojo::core::Core::BeginReadData	C++
 	mojo_core_embedder_internal.dll!MojoBeginReadDataImpl	C++
 	mojo_public_system.dll!MojoBeginReadData	C++
 	network_cpp.dll!mojo::DataPipeConsumerHandle::BeginReadData	C++
 	network_cpp.dll!network::`anonymous namespace'::BodyReader::ReadData	C++
 	network_cpp.dll!network::`anonymous namespace'::BodyReader::Start	C++
 	network_cpp.dll!network::`anonymous namespace'::SaveToStringBodyHandler::OnStartLoadingResponseBody	C++
 	network_cpp.dll!network::`anonymous namespace'::SimpleURLLoaderImpl::OnReceiveResponse	C++
*/
MojoResult
DataPipeConsumerDispatcher::BeginReadData(const void **buffer,
                                          uint32_t *buffer_num_bytes) {
  //  base::AutoLock lock(lock_);
  if (!shared_ring_buffer_.IsValid() || in_transit_) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }

  if (in_two_phase_read_) {
    return MOJO_RESULT_BUSY;
  }

  const bool had_new_data = new_data_available_;
  new_data_available_ = false;

  if (bytes_available_ == 0) {
    // if (had_new_data) {
    //   watchers_.NotifyState(GetHandleSignalsStateNoLock());
    // }
    return peer_closed_ ? MOJO_RESULT_FAILED_PRECONDITION
                        : MOJO_RESULT_SHOULD_WAIT;
  }

  uint32_t bytes_to_read =
      std::min(bytes_available_, options_.capacity_num_bytes - read_offset_);

  uint8_t *data = static_cast<uint8_t *>(ring_buffer_mapping_.memory());

  in_two_phase_read_ = true;
  *buffer = (data + read_offset_);
  *buffer_num_bytes = bytes_to_read;
  two_phase_max_bytes_read_ = bytes_to_read;

  // if (had_new_data) {
  //   watchers_.NotifyState(GetHandleSignalsStateNoLock());
  // }

  return MOJO_RESULT_OK;
}

MojoResult DataPipeConsumerDispatcher::EndReadData(uint32_t num_bytes_read) {
  //  base::AutoLock lock(lock_);
  if (!in_two_phase_read_) {
    return MOJO_RESULT_FAILED_PRECONDITION;
  }

  if (in_transit_) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }

  MojoResult rv;
  if (num_bytes_read > two_phase_max_bytes_read_ ||
      num_bytes_read % options_.element_num_bytes != 0) {
    rv = MOJO_RESULT_INVALID_ARGUMENT;
  } else {
    rv = MOJO_RESULT_OK;
    read_offset_ =
        (read_offset_ + num_bytes_read) % options_.capacity_num_bytes;

    bytes_available_ -= num_bytes_read;

    // base::AutoUnlock unlock(lock_);
    // NotifyRead(num_bytes_read);
  }

  in_two_phase_read_ = false;
  two_phase_max_bytes_read_ = 0;

  // watchers_.NotifyState(GetHandleSignalsStateNoLock());

  return rv;
}


void DataPipeConsumerDispatcher::OnPortStatusChanged() {
  // base::AutoLock lock(lock_);

  // We stop observing the control port as soon it's transferred, but this can
  // race with events which are raised right before that happens. This is fine
  // to ignore.
  if (transferred_) {
    return;
  }

  UpdateSignalsStateNoLock();
}

void DataPipeConsumerDispatcher::UpdateSignalsStateNoLock() {
  // lock_.AssertAcquired();
#if 0
  const bool was_peer_closed = peer_closed_;
  const bool was_peer_remote = peer_remote_;
  size_t previous_bytes_available = bytes_available_;

  ports::PortStatus port_status;
  int rv = node_controller_->node()->GetStatus(control_port_, &port_status);
  peer_remote_ = rv == ports::OK && port_status.peer_remote;
  if (rv != ports::OK || !port_status.receiving_messages) {
    peer_closed_ = true;
  } else if (rv == ports::OK && port_status.has_messages && !in_transit_) {
    std::unique_ptr<ports::UserMessageEvent> message_event;
    do {
      rv = node_controller_->node()->GetMessage(control_port_, &message_event,
                                                nullptr);
      if (rv != ports::OK) {
        peer_closed_ = true;
      }
      if (message_event) {
        auto* message = message_event->GetMessage<UserMessageImpl>();
        if (message->user_payload_size() < sizeof(DataPipeControlMessage)) {
          peer_closed_ = true;
          break;
        }

        const DataPipeControlMessage* m =
            static_cast<const DataPipeControlMessage*>(message->user_payload());

        if (m->command != DataPipeCommand::DATA_WAS_WRITTEN) {
          peer_closed_ = true;
          break;
        }

        uint32_t new_bytes_available;
        if (!base::CheckAdd(bytes_available_, m->num_bytes)
                 .AssignIfValid(&new_bytes_available) ||
            new_bytes_available > options_.capacity_num_bytes) {
          peer_closed_ = true;
          break;
        }

        bytes_available_ = new_bytes_available;
      }
    } while (message_event);
  }

  bool has_new_data = bytes_available_ != previous_bytes_available;
  if (has_new_data) {
    new_data_available_ = true;
  }

  if (peer_closed_ != was_peer_closed || has_new_data ||
      peer_remote_ != was_peer_remote) {
    watchers_.NotifyState(GetHandleSignalsStateNoLock());
  }
#endif  
}


} // namespace core
} // namespace mojo