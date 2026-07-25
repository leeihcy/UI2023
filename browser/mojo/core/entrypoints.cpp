#include "mojo/core/entrypoints.h"
#include "mojo/core/core.h"

#include <utility>

namespace {

mojo::core::Core* g_core;

}

namespace mojo {
namespace core {

// static
Core* Core::Get() {
  return g_core;
}
void InitializeCore() {
  g_core = new Core();
}

void ShutDownCore() {
  delete std::exchange(g_core, nullptr);
}

// const MojoSystemThunks2& GetSystemThunks() {
//   return g_thunks;
// }

}
}