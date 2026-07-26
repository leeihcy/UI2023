#include "mojo/core/node_controller.h"

namespace mojo {
namespace core {

base::WritableSharedMemoryRegion NodeController::CreateSharedBuffer(size_t num_bytes) {
  return base::WritableSharedMemoryRegion::Create(num_bytes);
}

}
}