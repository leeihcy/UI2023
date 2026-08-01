#include "mojo/core/data_pipe_producer_dispatcher.h"

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


}
}