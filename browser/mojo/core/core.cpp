#include "mojo/core/core.h"

#include "mojo/core/ports/port.h"
#include "mojo/core/data_pipe_consumer_dispatcher.h"
#include "mojo/core/data_pipe_producer_dispatcher.h"
#include "base/memory/unsafe_shared_memory_region.h"

namespace mojo {
namespace core {

NodeController *Core::GetNodeController() {
  // base::AutoLock lock(node_controller_lock_);
  if (!node_controller_) {
    node_controller_ = std::make_unique<NodeController>();
  }
  return node_controller_.get();
}


uint64_t MakePipeId() {
  return 0;
}

MojoResult Core::CreateDataPipe(const MojoCreateDataPipeOptions* options,
                                MojoHandle* data_pipe_producer_handle,
                                MojoHandle* data_pipe_consumer_handle) {

  // 创建共享内存
  base::subtle::PlatformSharedMemoryRegion ring_buffer_region =
      base::WritableSharedMemoryRegion::TakeHandleForSerialization(
          GetNodeController()->CreateSharedBuffer(options->capacity_num_bytes));
  // 提取底层句柄
  auto writable_region_handle = ring_buffer_region.PassPlatformHandle();

  // “降级”为不安全模式??
    base::UnsafeSharedMemoryRegion producer_region =
      base::UnsafeSharedMemoryRegion::Deserialize(
          base::subtle::PlatformSharedMemoryRegion::Take(
              std::move(writable_region_handle),
              base::subtle::PlatformSharedMemoryRegion::Mode::kUnsafe,
              options->capacity_num_bytes/*, ring_buffer_region.GetGUID()*/));

  ports::PortRef port0, port1;
  GetNodeController()->node()->CreatePortPair(&port0, &port1);

  uint64_t pipe_id = MakePipeId();
  std::shared_ptr<Dispatcher> producer = DataPipeProducerDispatcher::Create(
      GetNodeController(), port0, std::move(producer_region), *options,
      pipe_id);

  base::UnsafeSharedMemoryRegion consumer_region = producer_region.Duplicate();
  std::shared_ptr<Dispatcher> consumer = DataPipeConsumerDispatcher::Create(
      GetNodeController(), port1, std::move(consumer_region), *options,
      pipe_id);

  *data_pipe_producer_handle = AddDispatcher(producer);
  *data_pipe_consumer_handle = AddDispatcher(consumer);
  if (*data_pipe_producer_handle == MOJO_HANDLE_INVALID ||
      *data_pipe_consumer_handle == MOJO_HANDLE_INVALID) {
    if (*data_pipe_producer_handle != MOJO_HANDLE_INVALID) {
      std::shared_ptr<Dispatcher> unused;
      // base::AutoLock lock(handles_->GetLock());
      handles_->GetAndRemoveDispatcher(*data_pipe_producer_handle, &unused);
    }
    producer->Close();
    consumer->Close();
    return MOJO_RESULT_RESOURCE_EXHAUSTED;
  }

  return MOJO_RESULT_OK;
}


MojoHandle Core::AddDispatcher(std::shared_ptr<Dispatcher> dispatcher) {
  // base::AutoLock lock(handles_->GetLock());
  return handles_->AddDispatcher(dispatcher);
}

}
}