#ifndef MOJO_CORE_DATA_PIPE_PRODUCER_DISPATCHER_H_
#define MOJO_CORE_DATA_PIPE_PRODUCER_DISPATCHER_H_

#include "base/memory/unsafe_shared_memory_region.h"
#include "mojo/mojo_export.h"
#include "mojo/core/dispatcher.h"
#include "mojo/public/cpp/system/data_pipe.h"
#include "mojo/core/ports/port.h"
#include <memory>

namespace mojo {
namespace core {
class NodeController;

class MOJO_EXPORT DataPipeProducerDispatcher final
    : public Dispatcher {
public:
  static std::shared_ptr<DataPipeProducerDispatcher>
  Create(NodeController *node_controller, const ports::PortRef &control_port,
         base::UnsafeSharedMemoryRegion shared_ring_buffer,
         const MojoCreateDataPipeOptions &options, uint64_t pipe_id) {
    std::shared_ptr<DataPipeProducerDispatcher> producer =
        std::make_shared<DataPipeProducerDispatcher>(
            node_controller, control_port, std::move(shared_ring_buffer),
            options, pipe_id);
    // base::AutoLock lock(producer->lock_);
    if (!producer->InitializeNoLock()) {
      return nullptr;
    }
    return producer;
  }
  DataPipeProducerDispatcher(NodeController *node_controller,
                             const ports::PortRef &control_port,
                             base::UnsafeSharedMemoryRegion shared_ring_buffer,
                             const MojoCreateDataPipeOptions &options,
                             uint64_t pipe_id)
      : options_(options), node_controller_(node_controller),
        control_port_(control_port), 
        pipe_id_(pipe_id), 
        // watchers_(this),
        shared_ring_buffer_(std::move(shared_ring_buffer)),
        available_capacity_(options_.capacity_num_bytes) {}

  // Dispatcher:
  Type GetType() const override;
  MojoResult Close() override;

private:
  bool InitializeNoLock() { return true; }
  MojoResult CloseNoLock() { return 0;} ;


  const MojoCreateDataPipeOptions options_;
  const NodeController* node_controller_;
  const ports::PortRef control_port_;
  const uint64_t pipe_id_;

  
  base::UnsafeSharedMemoryRegion shared_ring_buffer_;
  // base::WritableSharedMemoryMapping ring_buffer_mapping_;
  
  uint32_t write_offset_ = 0;
  uint32_t available_capacity_;
};

}
}

#endif