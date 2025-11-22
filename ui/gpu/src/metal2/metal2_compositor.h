#ifndef _UI_GPU_SRC_METAL_METALCOMPOSITOR_H_
#define _UI_GPU_SRC_METAL_METALCOMPOSITOR_H_

#include "include/api.h"
#include "src/metal2/import.h"
#include "src/metal2/metal2_objects.h"
#include "src/metal2/metal2_pipeline.h"
#include "src/metal2/metal2_bridge.h"
#include "src/metal2/metal2_swapchain.h"
#include "src/metal2/shaders/shader_types.h"

namespace ui {
class Metal2Compositor;
}


@interface Metal2CompositorDelegate : NSObject
- (Metal2CompositorDelegate*)initWith:(ui::Metal2Compositor*)compositor;

- (void)registerScreenNotification:(NSWindow *)window;
- (void)unregisterScreenNotification;

- (void)onDeviceChanged: (NSNotification*) notification;
@end

namespace ui {

class Metal2Compositor : public IGpuCompositor, public IMetal2Bridge {
public:
  Metal2Compositor();
  ~Metal2Compositor() override;

  bool Initialize(IGpuCompositorWindow *window);
  void CheckDeviceChanged();

protected:
  std::shared_ptr<IGpuLayer> CreateLayerTexture() override;

  bool BeginCommit(GpuLayerCommitContext *) override;
  void EndCommit(GpuLayerCommitContext *) override;
  void Resize(int nWidth, int nHeight) override;

public:
  id<MTLDevice> GetMetalDevice() override { return m_device; }
  id<MTLCommandQueue> GetCommandQueue() override { return m_command_queue; }

  metal2::PipeLine& GetPipeline() override { return m_pipeline; }
  metal2::RenderPass& GetRenderPass() override { return m_render_pass; }
  metal2::SwapChain& GetSwapchain() override { return m_swapchain; }

  void Present();
  
private:
  id<MTLDevice> fetchDevice(NSView* view);
  void buildFrameData();

  void onDeviceLost();
  void onDeviceCreate();

private:
  int m_width = 0;
  int m_height = 0;
  metal2::FrameData m_frame_data;

  id<MTLDevice> m_device = nil;
  id<MTLCommandQueue> m_command_queue = nil;
  Metal2CompositorDelegate* m_delgate = nil;

  NSView*  m_bind_view = nullptr;
  // CAMetalLayer 本质上就是一个内置了交换链的图层，不需要我们再实现swapchain
  CAMetalLayer* m_bind_layer = nullptr;
  
  metal2::PipeLine m_pipeline;
  metal2::RenderPass m_render_pass;
  metal2::SwapChain m_swapchain;

  NSAutoreleasePool* m_auto_pool = nullptr;
  id<CAMetalDrawable> m_currentDrawable;
};

}

#endif