#include "mojo/core/core.h"

namespace mojo {
namespace core {

NodeController *Core::GetNodeController() {
  // base::AutoLock lock(node_controller_lock_);
  if (!node_controller_) {
    node_controller_ = std::make_unique<NodeController>();
  }
  return node_controller_.get();
}

MojoResult Core::CreateDataPipe(const MojoCreateDataPipeOptions* options,
                                MojoHandle* data_pipe_producer_handle,
                                MojoHandle* data_pipe_consumer_handle) {

      //                              base::subtle::PlatformSharedMemoryRegion ring_buffer_region =
      // base::WritableSharedMemoryRegion::TakeHandleForSerialization();

  // GetNodeController()->CreateSharedBuffer(create_options.capacity_num_bytes);

  return MOJO_RESULT_OK;
}

}
}