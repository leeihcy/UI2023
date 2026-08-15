#ifndef MOJO_CORE_DATA_PIPE_CONSUMER_DISPATCHER_H_
#define MOJO_CORE_DATA_PIPE_CONSUMER_DISPATCHER_H_

#include "base/memory/unsafe_shared_memory_region.h"
#include "base/memory/shared_memory_mapping.h"
#include "mojo/core/dispatcher.h"
#include "mojo/core/ports/port.h"
#include "mojo/mojo_export.h"
#include "mojo/public/cpp/system/data_pipe.h"
#include "mojo/core/node_controller.h"
#include "mojo/core/watcher_set.h"
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
                             uint64_t pipe_id);

  class PortObserverThunk : public NodeController::PortObserver {
  public:
    explicit PortObserverThunk(
        DataPipeConsumerDispatcher* dispatcher)
        : dispatcher_(dispatcher) {}
    ~PortObserverThunk() override { }
  private:
    void OnPortStatusChanged() override { dispatcher_->OnPortStatusChanged(); }
    DataPipeConsumerDispatcher* dispatcher_;
  };

  // Dispatcher:
  Type GetType() const override;
  MojoResult Close() override;
  MojoResult ReadData(const MojoReadDataOptions& validated_options,
                      void* elements,
                      uint32_t* num_bytes) override;
  MojoResult BeginReadData(const void** buffer,
                           uint32_t* buffer_num_bytes) override;
  MojoResult EndReadData(uint32_t num_bytes_read) override;
private:
  bool InitializeNoLock();
  MojoResult CloseNoLock();

  void OnPortStatusChanged();
  void UpdateSignalsStateNoLock();

  HandleSignalsState GetHandleSignalsStateNoLock() const;

private:
  const MojoCreateDataPipeOptions options_;
  NodeController* node_controller_;
  const ports::PortRef control_port_;
  const uint64_t pipe_id_;
    
  WatcherSet watchers_;

  base::WritableSharedMemoryMapping ring_buffer_mapping_;
  base::UnsafeSharedMemoryRegion shared_ring_buffer_;

  bool in_transit_ = false;
  bool is_closed_ = false;
  bool peer_closed_ = false;
  bool peer_remote_ = false;
  bool transferred_ = false;

  bool in_two_phase_read_ = false;  
  uint32_t two_phase_max_bytes_read_ = 0;

  uint32_t read_offset_ = 0;
  uint32_t bytes_available_ = 0;

  // Indicates whether any new data is available since the last read attempt.
  bool new_data_available_ = false;
};

} // namespace core
} // namespace mojo

#endif