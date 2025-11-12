#ifndef _UI_GPU_SRC_METAL_METALCOMPOSITOR_H_
#define _UI_GPU_SRC_METAL_METALCOMPOSITOR_H_

#include "include/api.h"
#include "src/metal2/import.h"
#include "src/metal2/metal2_pipeline.h"
#include "src/metal2/metal2_bridge.h"

namespace ui {
class Metal2Compositor;
}

#if defined(__OBJC__)
@class NSView;
#else
class NSView;
#endif

#if defined(__OBJC__)
@interface Metal2CompositorDelegate : NSObject
- (Metal2CompositorDelegate*)initWith:(ui::Metal2Compositor*)compositor;

- (void)registerScreenNotification:(NSWindow *)window;
- (void)unregisterScreenNotification;

- (void)onDeviceChanged: (NSNotification*) notification;
@end
#endif

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
#if defined(__OBJC__)
  id<MTLDevice> GetMetalDevice() override { return m_device; }
#endif

private:
#if defined(__OBJC__)
  id<MTLDevice> fetchDevice(NSView* view);
#endif

  void onDeviceLost();
  void onDeviceCreate();
  void OnSwapChainCreated() /*override*/;

public:
#if defined(__OBJC__)
  Metal2CompositorDelegate* m_delgate = nil;
  NSView*  m_bind_view = nullptr;

  id<MTLDevice> m_device = nil;

  id<MTLCommandQueue> m_command_queue = nil;
  id<MTLCommandBuffer> m_command_buffer = nil;
#endif

  metal2::PipeLine m_pipeline;

private:
  int m_width = 0;
  int m_height = 0;

};

}

#endif