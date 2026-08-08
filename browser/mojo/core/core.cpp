#include "mojo/core/core.h"

#include "base/memory/unsafe_shared_memory_region.h"
#include "mojo/core/data_pipe_consumer_dispatcher.h"
#include "mojo/core/data_pipe_producer_dispatcher.h"
#include "mojo/core/ports/port.h"
#include "mojo/core/request_context.h"

/*

>	mojo_core_embedder_internal.dll!CreateNode	C++
 	mojo_core_embedder_internal.dll!mojo::core::InitializeIpczNodeForProcess	C++
 	mojo_core_embedder.dll!mojo::core::Init	C++
 	content.dll!content::InitializeMojoCore	C++
 	chrome.dll!ChromeMainDelegate::PostEarlyInitialization	C++

*/

namespace mojo {
namespace core {

Core::Core() {
    handles_ = std::make_unique<HandleTable>();
}

NodeController *Core::GetNodeController() {
  // base::AutoLock lock(node_controller_lock_);
  if (!node_controller_) {
    node_controller_ = std::make_unique<NodeController>();
  }
  return node_controller_.get();
}

uint64_t MakePipeId() { return 0; }

MojoResult Core::CreateDataPipe(const MojoCreateDataPipeOptions *options,
                                MojoHandle *data_pipe_producer_handle,
                                MojoHandle *data_pipe_consumer_handle) {

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
              options->capacity_num_bytes /*, ring_buffer_region.GetGUID()*/));

  ports::PortRef port0, port1;
  GetNodeController()->node()->CreatePortPair(&port0, &port1);

  base::UnsafeSharedMemoryRegion consumer_region = producer_region.Duplicate();

  uint64_t pipe_id = MakePipeId();
  std::shared_ptr<Dispatcher> producer = DataPipeProducerDispatcher::Create(
      GetNodeController(), port0, std::move(producer_region), *options,
      pipe_id);

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
std::shared_ptr<Dispatcher> Core::GetDispatcher(MojoHandle handle) {
  // base::AutoLock lock(handles_->GetLock());
  return handles_->GetDispatcher(handle);
}

MojoResult Core::WriteData(MojoHandle data_pipe_producer_handle,
                           const void *elements, uint32_t *num_bytes,
                           const MojoWriteDataOptions *options) {
  return MOJO_RESULT_INVALID_ARGUMENT;
}
MojoResult Core::BeginWriteData(MojoHandle data_pipe_producer_handle,
                                const MojoBeginWriteDataOptions *options,
                                void **buffer, uint32_t *buffer_num_bytes) {
  return MOJO_RESULT_INVALID_ARGUMENT;
}
MojoResult Core::EndWriteData(MojoHandle data_pipe_producer_handle,
                              uint32_t num_bytes_written,
                              const MojoEndWriteDataOptions *options) {
  return MOJO_RESULT_INVALID_ARGUMENT;
}

MojoResult Core::ReadData(MojoHandle data_pipe_consumer_handle,
                          const MojoReadDataOptions *options, void *elements,
                          uint32_t *num_bytes) {
  RequestContext request_context;
  std::shared_ptr<Dispatcher> dispatcher(
      GetDispatcher(data_pipe_consumer_handle));
  if (!dispatcher) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }

  MojoReadDataOptions validated_options;
  if (options) {
    if (options->struct_size < sizeof(*options)) {
      return MOJO_RESULT_INVALID_ARGUMENT;
    }

    constexpr MojoReadDataFlags kSupportedFlags =
        MOJO_READ_DATA_FLAG_NONE | MOJO_READ_DATA_FLAG_ALL_OR_NONE |
        MOJO_READ_DATA_FLAG_DISCARD | MOJO_READ_DATA_FLAG_QUERY |
        MOJO_READ_DATA_FLAG_PEEK;
    if (options->flags & ~kSupportedFlags) {
      return MOJO_RESULT_UNIMPLEMENTED;
    }
    validated_options.flags = options->flags;
  } else {
    validated_options.flags = MOJO_WRITE_DATA_FLAG_NONE;
  }
  return dispatcher->ReadData(validated_options, elements, num_bytes);
}


MojoResult Core::BeginReadData(MojoHandle data_pipe_consumer_handle,
                               const MojoBeginReadDataOptions* options,
                               const void** buffer,
                               uint32_t* buffer_num_bytes) {
  RequestContext request_context;
  std::shared_ptr<Dispatcher> dispatcher(
      GetDispatcher(data_pipe_consumer_handle));
  if (!dispatcher) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }

  if (options) {
    if (options->struct_size < sizeof(*options)) {
      return MOJO_RESULT_INVALID_ARGUMENT;
    }
    if (options->flags != MOJO_BEGIN_READ_DATA_FLAG_NONE) {
      return MOJO_RESULT_UNIMPLEMENTED;
    }
  }
  return dispatcher->BeginReadData(buffer, buffer_num_bytes);
}

MojoResult Core::EndReadData(MojoHandle data_pipe_consumer_handle,
                             uint32_t num_bytes_read,
                             const MojoEndReadDataOptions* options) {
  RequestContext request_context;
  std::shared_ptr<Dispatcher> dispatcher(
      GetDispatcher(data_pipe_consumer_handle));
  if (!dispatcher) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }
  if (options) {
    if (options->struct_size < sizeof(*options)) {
      return MOJO_RESULT_INVALID_ARGUMENT;
    }
    if (options->flags != MOJO_END_READ_DATA_FLAG_NONE) {
      return MOJO_RESULT_UNIMPLEMENTED;
    }
  }
  return dispatcher->EndReadData(num_bytes_read);
}


} // namespace core
} // namespace mojo