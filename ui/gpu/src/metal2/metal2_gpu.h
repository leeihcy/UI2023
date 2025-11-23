#ifndef _UI_GPU_SRC_METAL2_METAL2GPU_H_
#define _UI_GPU_SRC_METAL2_METAL2GPU_H_
#include "include/api.h"
#include <memory>

namespace metal2 {

bool Startup();
void Shutdown();
std::unique_ptr<ui::IGpuCompositor> CreateCompsitor(ui::IGpuCompositorWindow *window);

} // namespace d3d12
#endif