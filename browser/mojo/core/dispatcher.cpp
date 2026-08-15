
#include "mojo/core/dispatcher.h"

namespace mojo {
namespace core {

Dispatcher::Dispatcher() = default;

Dispatcher::~Dispatcher() = default;


void Dispatcher::StartSerialize(uint32_t* num_bytes,
                                uint32_t* num_ports,
                                uint32_t* num_platform_handles) {
  *num_bytes = 0;
  *num_ports = 0;
  *num_platform_handles = 0;
}

bool Dispatcher::EndSerialize(void* destination,
                              ports::PortName* ports,
                              PlatformHandle* handles) {
  // LOG(ERROR) << "Attempting to serialize a non-transferrable dispatcher.";
  return true;
}

}  // namespace core
}  // namespace mojo
