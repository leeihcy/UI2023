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

MojoResult MojoWriteData(MojoHandle data_pipe_producer_handle,
                         const void *elements, uint32_t *num_bytes,
                         const struct MojoWriteDataOptions *options) {
  return mojo::core::Core::Get()->WriteData(data_pipe_producer_handle,
                                                elements, num_bytes, options);
}

MojoResult MojoBeginWriteData(MojoHandle data_pipe_producer_handle,
                              const struct MojoBeginWriteDataOptions *options,
                              void **buffer, uint32_t *buffer_num_bytes) {
  return mojo::core::Core::Get()->BeginWriteData(
      data_pipe_producer_handle, options, buffer, buffer_num_bytes);
}

MojoResult MojoEndWriteData(MojoHandle data_pipe_producer_handle,
                            uint32_t num_bytes_written,
                            const struct MojoEndWriteDataOptions *options) {
  return mojo::core::Core::Get()->EndWriteData(data_pipe_producer_handle,
                                                   num_bytes_written, options);
}

MojoResult MojoReadData(MojoHandle data_pipe_consumer_handle,
                        const struct MojoReadDataOptions *options,
                        void *elements, uint32_t *num_bytes) {
  return mojo::core::Core::Get()->ReadData(data_pipe_consumer_handle, options,
                                           elements, num_bytes);
}

MojoResult MojoBeginReadData(MojoHandle data_pipe_consumer_handle,
                             const struct MojoBeginReadDataOptions *options,
                             const void **elements, uint32_t *num_bytes) {
  return mojo::core::Core::Get()->BeginReadData(data_pipe_consumer_handle,
                                                options, elements, num_bytes);
}

MojoResult MojoEndReadData(MojoHandle data_pipe_consumer_handle,
                           uint32_t num_bytes_read,
                           const struct MojoEndReadDataOptions *options) {
  return mojo::core::Core::Get()->EndReadData(data_pipe_consumer_handle,
                                              num_bytes_read, options);
}

#ifdef __cplusplus
}
#endif