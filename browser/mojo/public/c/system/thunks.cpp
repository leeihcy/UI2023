#include "mojo/public/c/system/data_pipe.h"

#include "mojo/core/core.h"

#ifdef __cplusplus
extern "C" {
#endif

MojoResult MojoCreateDataPipe(const MojoCreateDataPipeOptions* options,
                              MojoHandle* data_pipe_producer_handle,
                              MojoHandle* data_pipe_consumer_handle) {
  // return INVOKE_THUNK(CreateDataPipe, options, data_pipe_producer_handle,
  //                     data_pipe_consumer_handle);
  return mojo::core::Core::Get()->CreateDataPipe(
      options, data_pipe_producer_handle, data_pipe_consumer_handle);
}

#ifdef __cplusplus
}
#endif