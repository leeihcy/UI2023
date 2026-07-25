#include "mojo/core/core.h"

namespace mojo {
namespace core {


MojoResult Core::CreateDataPipe(const MojoCreateDataPipeOptions* options,
                                MojoHandle* data_pipe_producer_handle,
                                MojoHandle* data_pipe_consumer_handle) {
  return MOJO_RESULT_OK;
}

}
}