#ifndef SERVICES_NETWORK_PUBLIC_CPP_NET_ADAPTERS_H_
#define SERVICES_NETWORK_PUBLIC_CPP_NET_ADAPTERS_H_

#include <memory>
#include "net/base/io_buffer.h"
#include "mojo/public/c/system/types.h"
#include "mojo/public/cpp/system/data_pipe.h"

// These adapters are used to transfer data between a Mojo pipe and the net
// library.
//
//   Mojo pipe              Data flow    Network library
//   ----------------------------------------------------------
//   MojoToNetPendingBuffer    --->      MojoToNetIOBuffer
//   NetToMojoPendingBuffer    <---      NetToMojoIOBuffer
//

namespace network {

class NetToMojoPendingBuffer {
public:
  static MojoResult BeginWrite(mojo::ScopedDataPipeProducerHandle* handle,
                               std::shared_ptr<NetToMojoPendingBuffer>* pending);

  NetToMojoPendingBuffer(mojo::ScopedDataPipeProducerHandle handle,
                         std::span<char> buffer);

  char* buffer() { return buffer_.data(); }
  uint32_t size() const { return static_cast<uint32_t>(buffer_.size()); }

private:
  mojo::ScopedDataPipeProducerHandle handle_;
  std::span<char> buffer_;
};


class NetToMojoIOBuffer : public net::WrappedIOBuffer {
public:
  explicit NetToMojoIOBuffer(
      std::shared_ptr<NetToMojoPendingBuffer> pending_buffer,
      size_t offset = 0);
   ~NetToMojoIOBuffer() override;

  std::shared_ptr<NetToMojoPendingBuffer> pending_buffer_;
};

}

#endif