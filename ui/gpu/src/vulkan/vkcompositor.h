#ifndef _UI_GPU_SRC_VULKAN_GPU_COMPOSITOR_H_
#define _UI_GPU_SRC_VULKAN_GPU_COMPOSITOR_H_
#include "gpu/include/api.h"
#if defined(OS_MAC)
#define VK_USE_PLATFORM_MACOS_MVK
// #define VK_USE_PLATFORM_METAL_EXT
#endif
#include <vulkan/vulkan.h>

namespace ui {
class GpuLayer;
class VunkanCompositor : public IGpuCompositor {
public:
  VunkanCompositor(void* hWnd);
  ~VunkanCompositor();
  virtual void Release() override { delete this; }

  IGpuLayer *CreateLayerTexture() override;
  bool BeginCommit() override;
  void EndCommit() override;
  void Resize(int nWidth, int nHeight) override;

  void SetRootLayerTexture(IGpuLayer *p) override;
  GpuLayer *GetRootLayerTexture();

public:
  bool create_vulkan_surface();
  
private:
  GpuLayer *m_pRootTexture;
  void* m_hWnd;

  // SIZE m_sizeBackBuffer;

  VkSurfaceKHR m_surface;

};
} // namespace ui
#endif