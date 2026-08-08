#ifndef MOJO_PUBLIC_CPP_SYSTEM_DATA_PIPE_H_
#define MOJO_PUBLIC_CPP_SYSTEM_DATA_PIPE_H_

#include <memory>
#include <span>
#include "mojo/public/c/system/data_pipe.h"
#include "mojo/public/c/system/types.h"
#include "mojo/public/c/system/data_pipe.h"
#include "mojo/public/cpp/system/handle.h"


namespace mojo {

class DataPipeProducerHandle : public Handle {
public:
 MojoResult BeginWriteData(size_t write_size_hint,
                            MojoBeginWriteDataFlags flags,
                            std::span<uint8_t>& buffer) const {
    MojoBeginWriteDataOptions options;
    options.struct_size = sizeof(options);
    options.flags = flags;

    void* buffer_ptr = nullptr;
    uint32_t buffer_num_bytes = (uint32_t)(write_size_hint);
    MojoResult result =
        MojoBeginWriteData(value(), &options, &buffer_ptr, &buffer_num_bytes);
    if (result == MOJO_RESULT_OK) {
      buffer = std::span(static_cast<uint8_t*>(buffer_ptr),
                                         size_t{buffer_num_bytes});
    }
    return result;
  }
};

class DataPipeConsumerHandle : public Handle {
public:
   MojoResult ReadData(MojoReadDataFlags flags,
                      std::span<uint8_t> buffer,
                      size_t& bytes_read) const {
    MojoReadDataOptions options;
    options.struct_size = sizeof(options);
    options.flags = flags;

    uint32_t num_bytes = (uint32_t)(buffer.size());
    MojoResult result =
        MojoReadData(value(), &options, buffer.data(), &num_bytes);
    bytes_read = size_t{num_bytes};
    return result;
  }

  MojoResult BeginReadData(MojoBeginReadDataFlags flags,
                           std::span<const uint8_t>& buffer) const {
    MojoBeginReadDataOptions options;
    options.struct_size = sizeof(options);
    options.flags = flags;

    const void* data = nullptr;
    uint32_t buffer_num_bytes = 0;
    MojoResult result =
        MojoBeginReadData(value(), &options, &data, &buffer_num_bytes);
    if (result == MOJO_RESULT_OK) {
      // SAFETY: Relying on the contract of the `MojoBeginReadData` C API which
      // says: "On success, |*buffer| will be a pointer from which the caller
      // can read up to |*buffer_num_bytes| bytes of data."
      buffer = std::span(static_cast<const uint8_t*>(data),
                                         size_t{buffer_num_bytes});
    }
    return result;
  }

  // Completes a two-phase read from a data pipe. See |MojoEndReadData()| for
  // complete documentation.
  MojoResult EndReadData(size_t num_bytes_read) const {
    uint32_t num_bytes_read_u32 = (uint32_t)(num_bytes_read);
    return MojoEndReadData(value(), num_bytes_read_u32, nullptr);
  }
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