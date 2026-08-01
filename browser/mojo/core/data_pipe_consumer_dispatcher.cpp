#include "mojo/core/data_pipe_consumer_dispatcher.h"

namespace mojo {
namespace core {


Dispatcher::Type DataPipeConsumerDispatcher::GetType() const {
  return Type::DATA_PIPE_CONSUMER;
}

MojoResult DataPipeConsumerDispatcher::Close() {
  // base::AutoLock lock(lock_);
  // DVLOG(1) << "Closing data pipe consumer " << pipe_id_;
  return CloseNoLock();
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

}
}