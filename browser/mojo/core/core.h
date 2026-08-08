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
  Core();
  static Core* Get();

  NodeController* GetNodeController();

  // These methods correspond to the API functions defined in
  // "mojo/public/c/system/data_pipe.h":
  MojoResult CreateDataPipe(const MojoCreateDataPipeOptions* options,
                            MojoHandle* data_pipe_producer_handle,
                            MojoHandle* data_pipe_consumer_handle);

  MojoHandle AddDispatcher(std::shared_ptr<Dispatcher> dispatcher);
  std::shared_ptr<Dispatcher> GetDispatcher(MojoHandle handle);
  
  MojoResult WriteData(MojoHandle data_pipe_producer_handle,
                       const void* elements,
                       uint32_t* num_bytes,
                       const MojoWriteDataOptions* options);
  MojoResult BeginWriteData(MojoHandle data_pipe_producer_handle,
                            const MojoBeginWriteDataOptions* options,
                            void** buffer,
                            uint32_t* buffer_num_bytes);
  MojoResult EndWriteData(MojoHandle data_pipe_producer_handle,
                          uint32_t num_bytes_written,
                          const MojoEndWriteDataOptions* options);

  MojoResult ReadData(MojoHandle data_pipe_consumer_handle,
                      const MojoReadDataOptions* options,
                      void* elements,
                      uint32_t* num_bytes);
  MojoResult BeginReadData(MojoHandle data_pipe_consumer_handle,
                           const MojoBeginReadDataOptions* options,
                           const void** buffer,
                           uint32_t* buffer_num_bytes);
  MojoResult EndReadData(MojoHandle data_pipe_consumer_handle,
                         uint32_t num_bytes_read,
                         const MojoEndReadDataOptions* options);

 private:
  std::unique_ptr<NodeController> node_controller_;

  std::unique_ptr<HandleTable> handles_;
};

}
}

#endif