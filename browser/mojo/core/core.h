#ifndef MOJO_CORE_CORE_H_
#define MOJO_CORE_CORE_H_

#include <memory>
#include "mojo/mojo_export.h"
#include "mojo/public/c/system/data_pipe.h"
#include "mojo/core/node_controller.h"
#include "mojo/core/dispatcher.h"
#include "mojo/core/handle_table.h"

namespace mojo {
namespace core {

class MOJO_EXPORT Core {
 public:
  // Core();
  static Core* Get();

  NodeController* GetNodeController();

  // These methods correspond to the API functions defined in
  // "mojo/public/c/system/data_pipe.h":
  MojoResult CreateDataPipe(const MojoCreateDataPipeOptions* options,
                            MojoHandle* data_pipe_producer_handle,
                            MojoHandle* data_pipe_consumer_handle);

  MojoHandle AddDispatcher(std::shared_ptr<Dispatcher> dispatcher);

 private:
  std::unique_ptr<NodeController> node_controller_;

  std::unique_ptr<HandleTable> handles_;
};

}
}

#endif