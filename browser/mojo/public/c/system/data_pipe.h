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


// Flags passed to |MojoWriteData()| via |MojoWriteDataOptions|. See values
// defined below.
typedef uint32_t MojoWriteDataFlags;

// No flags. Default behavior.
#define MOJO_WRITE_DATA_FLAG_NONE ((uint32_t)0)

// Requires that all provided data must fit into the pipe's available capacity
// in order for the write to succeed. Otherwise the write fails and no data is
// written into the pipe.
#define MOJO_WRITE_DATA_FLAG_ALL_OR_NONE ((uint32_t)1 << 0)

// Options passed to |MojoWriteData()|.
struct MojoWriteDataOptions {
  // The size of this structure, used for versioning.
  uint32_t struct_size;

  // See |MojoWriteDataFlags|.
  MojoWriteDataFlags flags;
};

// Flags passed to |MojoReadData()| via |MojoReadDataOptions|.
typedef uint32_t MojoReadDataFlags;

// No flags. Default behavior.
#define MOJO_READ_DATA_FLAG_NONE ((uint32_t)0)

// Requires that all request bytes can be read from the data pipe in order for
// the read to succeed. If that many bytes are not available for reading, the
// read will fail and no bytes will be read. Ignored of
// |MOJO_READ_DATA_FLAG_QUERY| is also set.
#define MOJO_READ_DATA_FLAG_ALL_OR_NONE ((uint32_t)1 << 0)

// Discards the data read rather than copying it into the caller's provided
// buffer. May not be combined with |MOJO_READ_DATA_FLAG_PEEK| or
// |MOJO_READ_DATA_FLAG_QUERY|.
#define MOJO_READ_DATA_FLAG_DISCARD ((uint32_t)1 << 1)

// Queries the number of bytes available for reading without actually reading
// the data. May not be combined with |MOJO_READ_DATA_FLAG_DISCARD| or
// |MOJO_READ_DATA_FLAG_PEEK|. |MOJO_READ_DATA_FLAG_ALL_OR_NONE| is ignored if
// this is set.
#define MOJO_READ_DATA_FLAG_QUERY ((uint32_t)1 << 2)

// Reads data from the pipe and copies it to the caller's provided buffer
// without actually removing the data from the pipe. May not be combined with
// |MOJO_READ_DATA_FLAG_DISCARD| or |MOJO_READ_DATA_FLAG_QUERY|.
#define MOJO_READ_DATA_FLAG_PEEK ((uint32_t)1 << 3)

// Options passed to |MojoReadData()|.
struct MojoReadDataOptions {
  // The size of this structure, used for versioning.
  uint32_t struct_size;

  // See |MojoReadDataFlags|.
  MojoReadDataFlags flags;
};

typedef uint32_t MojoBeginReadDataFlags;
#define MOJO_BEGIN_READ_DATA_FLAG_NONE ((uint32_t)0)
struct MojoBeginReadDataOptions {
  uint32_t struct_size;
  MojoBeginReadDataFlags flags;
};


typedef uint32_t MojoEndReadDataFlags;
#define MOJO_END_READ_DATA_FLAG_NONE ((uint32_t)0)
struct MojoEndReadDataOptions {
  uint32_t struct_size;
  MojoEndReadDataFlags flags;
};


#ifdef __cplusplus
extern "C" {
#endif

MOJO_EXPORT MojoResult MojoCreateDataPipe(const struct MojoCreateDataPipeOptions *options,
                              MojoHandle *data_pipe_producer_handle,
                              MojoHandle *data_pipe_consumer_handle);

// MojoResult MojoWriteData(MojoHandle data_pipe_producer_handle,
//                          const void *elements, uint32_t *num_bytes,
//                          const struct MojoWriteDataOptions *options);

// MojoResult MojoBeginWriteData(MojoHandle data_pipe_producer_handle,
//                               const struct MojoBeginWriteDataOptions *options,
//                               void **buffer, uint32_t *buffer_num_bytes);

// MojoResult MojoEndWriteData(MojoHandle data_pipe_producer_handle,
//                             uint32_t num_bytes_written,
//                             const struct MojoEndWriteDataOptions *options);

MOJO_EXPORT MojoResult MojoReadData(MojoHandle data_pipe_consumer_handle,
                        const struct MojoReadDataOptions *options,
                        void *elements, uint32_t *num_bytes);

MOJO_EXPORT MojoResult MojoBeginReadData(MojoHandle data_pipe_consumer_handle,
                             const struct MojoBeginReadDataOptions *options,
                             const void **buffer, uint32_t *buffer_num_bytes);

MOJO_EXPORT MojoResult MojoEndReadData(MojoHandle data_pipe_consumer_handle,
                           uint32_t num_bytes_read,
                           const struct MojoEndReadDataOptions *options);
#ifdef __cplusplus
} // extern "C"
#endif

#endif