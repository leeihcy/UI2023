#ifndef MOJO_CORE_DATA_PIPE_CONSUMER_DISPATCHER_H_
#define MOJO_CORE_DATA_PIPE_CONSUMER_DISPATCHER_H_

#include "base/memory/unsafe_shared_memory_region.h"
#include "mojo/core/dispatcher.h"
#include "mojo/core/ports/port.h"
#include "mojo/mojo_export.h"
#include "mojo/public/cpp/system/data_pipe.h"
#include <memory>


namespace mojo {
namespace core {

class NodeController;

class MOJO_EXPORT DataPipeConsumerDispatcher final : public Dispatcher {
public:
  static std::shared_ptr<DataPipeConsumerDispatcher>
  Create(NodeController *node_controller, const ports::PortRef &control_port,
         base::UnsafeSharedMemoryRegion shared_ring_buffer,
         const MojoCreateDataPipeOptions &options, uint64_t pipe_id) {
    std::shared_ptr<DataPipeConsumerDispatcher> consumer =
        std::make_shared<DataPipeConsumerDispatcher>(node_controller, control_port,
                                       std::move(shared_ring_buffer), options,
                                       pipe_id);
    // base::AutoLock lock(consumer->lock_);
    if (!consumer->InitializeNoLock()) {
      return nullptr;
    }
    return consumer;
  }
  DataPipeConsumerDispatcher(NodeController *node_controller,
                             const ports::PortRef &control_port,
                             base::UnsafeSharedMemoryRegion shared_ring_buffer,
                             const MojoCreateDataPipeOptions &options,
                             uint64_t pipe_id)    : options_(options),
      node_controller_(node_controller),
      control_port_(control_port),
      pipe_id_(pipe_id),
      // watchers_(this),
      shared_ring_buffer_(std::move(shared_ring_buffer)) {}

  // Dispatcher:
  Type GetType() const override;
  MojoResult Close() override;

private:
  bool InitializeNoLock() { return true;}
  MojoResult CloseNoLock();

private:
  const MojoCreateDataPipeOptions options_;
  const NodeController* node_controller_;
  const ports::PortRef control_port_;
  const uint64_t pipe_id_;
  
  base::UnsafeSharedMemoryRegion shared_ring_buffer_;
};

} // namespace core
} // namespace mojo

#endif