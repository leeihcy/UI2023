#include "mojo/core/data_pipe_producer_dispatcher.h"
#include "base/memory/unsafe_shared_memory_region.h"

#ifdef min
#undef min
#endif

namespace mojo {
namespace core {

Dispatcher::Type DataPipeProducerDispatcher::GetType() const {
  return Type::DATA_PIPE_PRODUCER;
}

MojoResult DataPipeProducerDispatcher::Close() {
  // base::AutoLock lock(lock_);
  // DVLOG(1) << "Closing data pipe producer " << pipe_id_;
  return CloseNoLock();
}

bool DataPipeProducerDispatcher::InitializeNoLock() {
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

void DataPipeProducerDispatcher::OnPortStatusChanged() {
  // base::AutoLock lock(lock_);
  if (transferred_) {
    return;
  }
  UpdateSignalsStateNoLock();
}


void DataPipeProducerDispatcher::UpdateSignalsStateNoLock() {
  // lock_.AssertAcquired();

  const bool was_peer_closed = peer_closed_;
  const bool was_peer_remote = peer_remote_;
  size_t previous_capacity = available_capacity_;

  ports::PortStatus port_status;
  int rv = node_controller_->node()->GetStatus(control_port_, &port_status);
  peer_remote_ = rv == ports::OK && port_status.peer_remote;
#if 0
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

        if (m->command != DataPipeCommand::DATA_WAS_READ) {
          peer_closed_ = true;
          break;
        }

        TRACE_EVENT0("ipc",
                     "DataPipeProducerDispatcher received DATA_WAS_READ");

        uint32_t new_available_capacity;
        if (!base::CheckAdd(available_capacity_, m->num_bytes)
                 .AssignIfValid(&new_available_capacity) ||
            new_available_capacity > options_.capacity_num_bytes) {
          break;
        }

        DVLOG(1) << "Data pipe producer " << pipe_id_ << " is aware that "
                 << m->num_bytes
                 << " bytes were read. [control_port=" << control_port_.name()
                 << "]";

        available_capacity_ = new_available_capacity;
      }
    } while (message_event);
  }

  if (peer_closed_ != was_peer_closed ||
      available_capacity_ != previous_capacity ||
      was_peer_remote != peer_remote_) {
    watchers_.NotifyState(GetHandleSignalsStateNoLock());
  }
#endif
}

/*
 	mojo_core_embedder_internal.dll!mojo::core::DataPipeProducerDispatcher::BeginWriteData 行 179	C++
 	mojo_core_embedder_internal.dll!mojo::core::Core::BeginWriteData 行 839	C++
 	mojo_core_embedder_internal.dll!MojoBeginWriteDataImpl 行 157	C++
 	mojo_public_system.dll!MojoBeginWriteData 行 117	C++
 	network_cpp.dll!mojo::DataPipeProducerHandle::BeginWriteData 行 106	C++
 	network_cpp.dll!network::NetToMojoPendingBuffer::BeginWrite 行 36	C++
>	services_network_network_service.dll!network::URLLoader::ReadMore 行 1593	C++
 	services_network_network_service.dll!network::URLLoader::StartReading 行 2603	C++
 	services_network_network_service.dll!network::URLLoader::ContinueOnResponseStarted 行 1424	C++
 	services_network_network_service.dll!network::URLLoader::ProcessInboundSharedStorageInterceptorOnResponseStarted 行 1182	C++
 	services_network_network_service.dll!network::URLLoader::OnResponseStarted 行 1240	C++
 	net.dll!net::URLRequest::NotifyResponseStarted 行 964	C++
 	net.dll!net::URLRequestJob::NotifyFinalHeadersReceived 行 545	C++
 	net.dll!net::URLRequestJob::NotifyHeadersComplete 行 503	C++
 	net.dll!net::URLRequestHttpJob::NotifyHeadersComplete 行 651	C++
 	net.dll!net::URLRequestHttpJob::SaveCookiesAndNotifyHeadersComplete 行 1038	C++
 	net.dll!net::URLRequestHttpJob::OnStartCompleted 行 1301	C++
*/
MojoResult DataPipeProducerDispatcher::BeginWriteData(
    void **buffer, uint32_t *buffer_num_bytes, MojoBeginWriteDataFlags flags) {
  // base::AutoLock lock(lock_);
  if (!shared_ring_buffer_.IsValid() || in_transit_) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }

  if (in_two_phase_write_) {
    return MOJO_RESULT_BUSY;
  }
  if (peer_closed_) {
    return MOJO_RESULT_FAILED_PRECONDITION;
  }

  if (available_capacity_ == 0) {
    return peer_closed_ ? MOJO_RESULT_FAILED_PRECONDITION
                        : MOJO_RESULT_SHOULD_WAIT;
  }

  in_two_phase_write_ = true;
  *buffer_num_bytes = std::min(options_.capacity_num_bytes - write_offset_,
                               available_capacity_);

  uint8_t* data = static_cast<uint8_t*>(ring_buffer_mapping_.memory());
  *buffer = (data + write_offset_);

  return MOJO_RESULT_OK;
}

MojoResult
DataPipeProducerDispatcher::WriteData(const void *elements, uint32_t *num_bytes,
                                      const MojoWriteDataOptions &options) {

  // base::AutoLock lock(lock_);
  if (!shared_ring_buffer_.IsValid() || in_transit_) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }

  if (in_two_phase_write_) {
    return MOJO_RESULT_BUSY;
  }

  if (peer_closed_) {
    return MOJO_RESULT_FAILED_PRECONDITION;
  }

  if (*num_bytes % options_.element_num_bytes != 0) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }
  if (*num_bytes == 0) {
    return MOJO_RESULT_OK;  // Nothing to do.
  }

  if ((options.flags & MOJO_WRITE_DATA_FLAG_ALL_OR_NONE) &&
      (*num_bytes > available_capacity_)) {
    // Don't return "should wait" since you can't wait for a specified amount of
    // data.
    return MOJO_RESULT_OUT_OF_RANGE;
  }

  uint32_t num_bytes_to_write = std::min(*num_bytes, available_capacity_);
  if (num_bytes_to_write == 0) {
    return MOJO_RESULT_SHOULD_WAIT;
  }

  *num_bytes = num_bytes_to_write;

  uint8_t* data = static_cast<uint8_t*>(ring_buffer_mapping_.memory());

  const uint8_t* source = static_cast<const uint8_t*>(elements);

  uint32_t tail_bytes_to_write =
      std::min(options_.capacity_num_bytes - write_offset_, num_bytes_to_write);
  uint32_t head_bytes_to_write = num_bytes_to_write - tail_bytes_to_write;

  memcpy(data + write_offset_, source, tail_bytes_to_write);
  if (head_bytes_to_write > 0) {
        memcpy(data, source + tail_bytes_to_write, head_bytes_to_write);
  }

  available_capacity_ -= num_bytes_to_write;
  write_offset_ =
      (write_offset_ + num_bytes_to_write) % options_.capacity_num_bytes;

  // watchers_.NotifyState(GetHandleSignalsStateNoLock());

  // base::AutoUnlock unlock(lock_);
  // NotifyWrite(num_bytes_to_write);

  return MOJO_RESULT_OK;
}
MojoResult
DataPipeProducerDispatcher::EndWriteData(uint32_t num_bytes_written) {
  // base::AutoLock lock(lock_);
  if (is_closed_ || in_transit_) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }

  if (!in_two_phase_write_) {
    return MOJO_RESULT_FAILED_PRECONDITION;
  }

  // Note: Allow successful completion of the two-phase write even if the other
  // side has been closed.
  MojoResult rv = MOJO_RESULT_OK;
  if (num_bytes_written > available_capacity_ ||
      num_bytes_written % options_.element_num_bytes != 0 ||
      write_offset_ + num_bytes_written > options_.capacity_num_bytes) {
    rv = MOJO_RESULT_INVALID_ARGUMENT;
  } else {
    available_capacity_ -= num_bytes_written;
    write_offset_ =
        (write_offset_ + num_bytes_written) % options_.capacity_num_bytes;

    // base::AutoUnlock unlock(lock_);
    // NotifyWrite(num_bytes_written);
  }

  in_two_phase_write_ = false;

  // If we're now writable, we *became* writable (since we weren't writable
  // during the two-phase write), so notify watchers.
  // watchers_.NotifyState(GetHandleSignalsStateNoLock());

  return rv;
}

}
}