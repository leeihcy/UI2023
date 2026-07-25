#ifndef MOJO_PUBLIC_CPP_SYSTEM_DATA_PIPE_H_
#define MOJO_PUBLIC_CPP_SYSTEM_DATA_PIPE_H_

#include <memory>
#include "mojo/public/c/system/data_pipe.h"
#include "mojo/public/c/system/types.h"
#include "mojo/public/cpp/system/handle.h"

namespace mojo {

class DataPipeProducerHandle : public Handle {
};

class DataPipeConsumerHandle : public Handle {
};

typedef ScopedHandleBase<DataPipeConsumerHandle> ScopedDataPipeConsumerHandle;
typedef ScopedHandleBase<DataPipeProducerHandle> ScopedDataPipeProducerHandle;

// Creates a new data pipe. See |MojoCreateDataPipe()| for complete
// documentation.
inline MojoResult CreateDataPipe(
    const MojoCreateDataPipeOptions* options,
    ScopedDataPipeProducerHandle& data_pipe_producer,
    ScopedDataPipeConsumerHandle& data_pipe_consumer) {
  DataPipeProducerHandle producer_handle;
  DataPipeConsumerHandle consumer_handle;
  MojoResult rv = MojoCreateDataPipe(options, producer_handle.mutable_value(),
                                     consumer_handle.mutable_value());
  // Reset even on failure (reduces the chances that a "stale"/incorrect handle
  // will be used).
  data_pipe_producer.reset(producer_handle);
  data_pipe_consumer.reset(consumer_handle);
  return rv;
}


// Creates a new data pipe with a specified capacity size. For setting
// additional options, see |CreateDataPipe()| above.
inline MojoResult CreateDataPipe(
    uint32_t capacity_num_bytes,
    ScopedDataPipeProducerHandle& data_pipe_producer,
    ScopedDataPipeConsumerHandle& data_pipe_consumer) {
  MojoCreateDataPipeOptions options;
  options.struct_size = sizeof(MojoCreateDataPipeOptions);
  options.flags = MOJO_CREATE_DATA_PIPE_FLAG_NONE;
  options.element_num_bytes = 1;
  options.capacity_num_bytes = capacity_num_bytes;
  return CreateDataPipe(&options, data_pipe_producer, data_pipe_consumer);
}

}
#endif