#ifndef SERVICES_NETWORK_PUBLIC_CPP_NET_ADAPTERS_H_
#define SERVICES_NETWORK_PUBLIC_CPP_NET_ADAPTERS_H_

#include <memory>
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
private:
  mojo::ScopedDataPipeProducerHandle handle_;
  std::span<char> buffer_;
};

}

#endif