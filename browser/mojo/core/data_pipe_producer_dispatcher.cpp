#include "mojo/core/data_pipe_producer_dispatcher.h"
#include "base/memory/unsafe_shared_memory_region.h"

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
  assert(false);
  return 0;
}

MojoResult
DataPipeProducerDispatcher::WriteData(const void *elements, uint32_t *num_bytes,
                                      const MojoWriteDataOptions &options) {
  assert(false);
  return 0;
}
MojoResult
DataPipeProducerDispatcher::EndWriteData(uint32_t num_bytes_written) {
  assert(false);
  return 0;
}

}
}