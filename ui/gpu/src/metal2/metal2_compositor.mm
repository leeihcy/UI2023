#include "src/metal2/metal2_compositor.h"
#include "src/metal2/metal2_app.h"
#include "src/metal2/metal2_bridge.h"
#include "src/util.h"

namespace ui {


Metal2Compositor::Metal2Compositor() : m_pipeline(*static_cast<IMetal2Bridge*>(this)){
  m_delgate = [[Metal2CompositorDelegate alloc] initWith:this];
}
Metal2Compositor::~Metal2Compositor() {
  [m_delgate release];
}

std::shared_ptr<IGpuLayer> Metal2Compositor::CreateLayerTexture() {
  return std::shared_ptr<IGpuLayer>();
}

bool Metal2Compositor::Initialize(IGpuCompositorWindow *window) {
  assert(window->GetType() == GpuCompositorWindowType::MacOSNSView);
  m_bind_view = (NSView*)((IGpuCompositorWindowNSView*)window)->GetNSWindowRootView(); 
  NSWindow *nswindow = m_bind_view.window;

  id<MTLDevice> device = fetchDevice(m_bind_view);
  if (device == nil) {
    return false;
  }
  printf("fetch device: %s\n", device.name.UTF8String);
  m_device = device;
  [m_delgate registerScreenNotification:nswindow];

  onDeviceCreate();

  // id<MTLDevice> device = Metal2Application::GetInstance().m_device;
  // m_command_queue = [device newCommandQueue];
  // m_command_buffer = [m_command_queue commandBuffer];
  
  return true; 
}

// 从view控件，获取当前所在屏幕的显卡设备。
// 不要跨显卡渲染，会造成多余的数据拷贝。
//
// Ensure the system renders with the device that drives the display, 
// and avoids copying any drawables from one GPU to another.
//
id<MTLDevice> Metal2Compositor::fetchDevice(NSView *view) {
  NSWindow* window = view.window;
  if (!window) {
    return nil;
  }
  NSScreen *screen = window.screen;
  if (!screen) {
    return nil;
  }

  CGDirectDisplayID view_display_id =
      [screen.deviceDescription[@"NSScreenNumber"] unsignedIntegerValue];

  // 没有显示器，或者当前显卡不支持metal
  id<MTLDevice> device = CGDirectDisplayCopyCurrentMetalDevice(view_display_id);
  return device;
}

void Metal2Compositor::CheckDeviceChanged() {
  printf("TODO: Metal2Compositor::onDeviceChanged\n");
  id<MTLDevice> new_device = fetchDevice(m_bind_view);
  if (!new_device) {
    return;
  }

  if (m_device && m_device.registryID == new_device.registryID) {
    return;
  }
  printf("move to new device: %s\n", new_device.name.UTF8String);

  if (m_device) {
    onDeviceLost();
    [m_device release];
    m_device = nil;
  }

  m_device = new_device;
  onDeviceCreate();
}

void Metal2Compositor::onDeviceLost() {

}
void Metal2Compositor::onDeviceCreate() {
  m_pipeline.Create();
}

void Metal2Compositor::OnSwapChainCreated() {
  
}

bool Metal2Compositor::BeginCommit(GpuLayerCommitContext *) { return false; }
void Metal2Compositor::EndCommit(GpuLayerCommitContext *) {}
void Metal2Compositor::Resize(int width, int height) {
  if (m_width == width && m_height == height) {
    return;
  }

  ui::Log("Metal2Compositor Resize: width=%d, height=%d", width, height);
  
  m_width = width;
  m_height = height;

  // m_swapchain.MarkNeedReCreate();
}

}

@implementation Metal2CompositorDelegate {
  ui::Metal2Compositor *m_compositor;
  NSWindow* m_window;
}
- (Metal2CompositorDelegate*)initWith:(ui::Metal2Compositor*)compositor {
  m_compositor = compositor;
  m_window = nullptr;
  return self;
}

- (void) registerScreenNotification: (NSWindow*)window {
  m_window = window;

  [[NSNotificationCenter defaultCenter]
      addObserver:self
         selector:@selector(onDeviceChanged:)
             name:NSApplicationDidChangeScreenParametersNotification
           object:nil];

  if (window) {
    [[NSNotificationCenter defaultCenter]
        addObserver:self
           selector:@selector(onDeviceChanged:)
               name:NSWindowDidChangeScreenNotification
             object:window];
  }
}
- (void)unregisterScreenNotification {
  [[NSNotificationCenter defaultCenter]
      removeObserver:self
                name:NSApplicationDidChangeScreenParametersNotification
              object:nil];
  [[NSNotificationCenter defaultCenter]
      removeObserver:self
                name:NSWindowDidChangeScreenNotification
              object:m_window];
}

- (void)onDeviceChanged: (NSNotification*) notification {
  if (m_compositor) {
    m_compositor->CheckDeviceChanged();
  }
}
@end