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
    // node_controller_->SetPortObserver(
    //     control_port_, base::MakeRefCounted<PortObserverThunk>(this));

    return true;
}

}
}