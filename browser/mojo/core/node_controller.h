#ifndef MOJO_CORE_NODE_CONTROLLER_H_
#define MOJO_CORE_NODE_CONTROLLER_H_

#include "mojo/mojo_export.h"
#include "base/memory/writable_shared_memory_region.h"

namespace mojo {
namespace core {

class MOJO_EXPORT NodeController {
public:
    // Creates a new shared buffer for use in the current process.
  base::WritableSharedMemoryRegion CreateSharedBuffer(size_t num_bytes);

private:
  // const ports::NodeName name_;
  // const std::unique_ptr<ports::Node> node_;
};

}  // namespace core
}  // namespace mojo


#endif