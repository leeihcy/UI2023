#ifndef MOJO_PUBLIC_C_SYSTEM_DATA_PIPE_H_
#define MOJO_PUBLIC_C_SYSTEM_DATA_PIPE_H_

#include "mojo/public/c/system/types.h"

#include <stdint.h>
#include "mojo/mojo_export.h"

typedef uint32_t MojoCreateDataPipeFlags;
#define MOJO_CREATE_DATA_PIPE_FLAG_NONE ((uint32_t)0)

// Options passed to |MojoCreateDataPipe()|.
struct MojoCreateDataPipeOptions {
  uint32_t struct_size;
  MojoCreateDataPipeFlags flags;
  uint32_t element_num_bytes;
  uint32_t capacity_num_bytes;
};


#ifdef __cplusplus
extern "C" {
#endif

MOJO_EXPORT MojoResult MojoCreateDataPipe(const struct MojoCreateDataPipeOptions *options,
                              MojoHandle *data_pipe_producer_handle,
                              MojoHandle *data_pipe_consumer_handle);

MojoResult MojoWriteData(MojoHandle data_pipe_producer_handle,
                         const void *elements, uint32_t *num_bytes,
                         const struct MojoWriteDataOptions *options);

MojoResult MojoBeginWriteData(MojoHandle data_pipe_producer_handle,
                              const struct MojoBeginWriteDataOptions *options,
                              void **buffer, uint32_t *buffer_num_bytes);

MojoResult MojoEndWriteData(MojoHandle data_pipe_producer_handle,
                            uint32_t num_bytes_written,
                            const struct MojoEndWriteDataOptions *options);

MojoResult MojoReadData(MojoHandle data_pipe_consumer_handle,
                        const struct MojoReadDataOptions *options,
                        void *elements, uint32_t *num_bytes);

MojoResult MojoBeginReadData(MojoHandle data_pipe_consumer_handle,
                             const struct MojoBeginReadDataOptions *options,
                             const void **buffer, uint32_t *buffer_num_bytes);

MojoResult MojoEndReadData(MojoHandle data_pipe_consumer_handle,
                           uint32_t num_bytes_read,
                           const struct MojoEndReadDataOptions *options);
#ifdef __cplusplus
} // extern "C"
#endif

#endif