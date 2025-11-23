#ifndef _UI_GPU_SRC_D3D10_D3D11GPU_H_
#define _UI_GPU_SRC_D3D10_D3D11GPU_H_
#include "include/api.h"
#include <memory>


namespace d3d10 {

bool Startup();
void Shutdown();
std::unique_ptr<ui::IGpuCompositor> CreateCompsitor(ui::IGpuCompositorWindow *window);

} // namespace d3d11
#endif