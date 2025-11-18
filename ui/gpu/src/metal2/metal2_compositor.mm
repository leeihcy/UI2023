#include "src/metal2/metal2_compositor.h"
#include "src/metal2/metal2_app.h"
#include "src/metal2/metal2_bridge.h"
#include "src/metal2/metal2_layer.h"
#include "src/metal2/shaders/shader_types.h"
#include "src/util.h"
#include <simd/matrix.h>

namespace ui {

Metal2Compositor::Metal2Compositor()
    : m_pipeline(*static_cast<IMetal2Bridge *>(this)) {
  m_delgate = [[Metal2CompositorDelegate alloc] initWith:this];
}
Metal2Compositor::~Metal2Compositor() {
  if (m_delgate) {
    [m_delgate release];
    m_delgate = nullptr;
  }
}

std::shared_ptr<IGpuLayer> Metal2Compositor::CreateLayerTexture() {
  return std::shared_ptr<IGpuLayer>(
      new Metal2GpuLayer(*static_cast<IMetal2Bridge *>(this)));
}

bool Metal2Compositor::Initialize(IGpuCompositorWindow *window) {
  assert(window->GetType() == GpuCompositorWindowType::MacOSNSView);
  NSView *view =
      (NSView *)((IGpuCompositorWindowNSView *)window)->GetNSWindowRootView();
  m_bind_view = view;
  m_bind_layer = (CAMetalLayer *)view.layer;
  NSWindow *nswindow = m_bind_view.window;

  id<MTLDevice> device = fetchDevice(m_bind_view);
  if (device == nil) {
    return false;
  }
  ui::Log("fetch device: %s", device.name.UTF8String);
  m_device = device;

  [m_delgate registerScreenNotification:nswindow];

  onDeviceCreate();

  return true;
}

// 从view控件，获取当前所在屏幕的显卡设备。
// 不要跨显卡渲染，会造成多余的数据拷贝。
//
// Ensure the system renders with the device that drives the display,
// and avoids copying any drawables from one GPU to another.
//
id<MTLDevice> Metal2Compositor::fetchDevice(NSView *view) {
  NSWindow *window = view.window;
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

void Metal2Compositor::onDeviceLost() {}
void Metal2Compositor::onDeviceCreate() {
  // Set the device for the layer so the layer can create drawable textures that
  // can be rendered to on this device.
  m_bind_layer.device = m_device;

  m_command_queue = [m_device newCommandQueue];
  // m_command_buffer.Create(m_command_queue);
  m_pipeline.Create();
  m_render_pass.Create();
}

void Metal2Compositor::OnSwapChainCreated() {}


bool Metal2Compositor::BeginCommit(GpuLayerCommitContext *) {
  m_currentDrawable = [m_bind_layer nextDrawable];
  // If the current drawable is nil, skip rendering this frame
  if (!m_currentDrawable) {
    return false;
  }
  
  // TODO: AutoReleasePool...

  // if (m_commandBuffer) {
  //   [m_commandBuffer release];
  //   m_commandBuffer = nullptr;
  // }
  // if (m_renderEncoder) {
  //   [m_renderEncoder release];
  //   m_renderEncoder = nullptr;
  // }

  m_commandBuffer = [m_command_queue commandBuffer];
  m_render_pass.SetTexture(m_currentDrawable.texture);

  m_renderEncoder = [m_commandBuffer
      renderCommandEncoderWithDescriptor:m_render_pass.m_descriptor];

  [m_renderEncoder setRenderPipelineState:m_pipeline.m_state];

  // pipe line ... frame ...
  {
    FrameData frame_data;
    frame_data.view = matrix_identity_float4x4;

    float left = 0;
    float right = m_width;
    float top = 0;
    float bottom = m_height;
    float near = 2000.f;
    float far = -2000.f;

    frame_data.ortho = matrix_identity_float4x4;
    frame_data.ortho.columns[0][0] = 2.0f / (right - left);
		frame_data.ortho.columns[1][1] = 2.0f / (top - bottom);
		frame_data.ortho.columns[2][2] = 1.0f / (far - near);
		frame_data.ortho.columns[3][0] = - (right + left) / (right - left);
		frame_data.ortho.columns[3][1] = - (top + bottom) / (top - bottom);
		frame_data.ortho.columns[3][2] = - near / (far - near);

    [m_renderEncoder setVertexBytes:&frame_data
                             length:sizeof(frame_data)
                            atIndex:(int)VertexShaderInput::FrameData];
  }

  return true;
}

void Metal2Compositor::EndCommit(GpuLayerCommitContext *) {
  [m_renderEncoder endEncoding];

  [m_commandBuffer presentDrawable:m_currentDrawable];

  [m_commandBuffer commit];
}

void Metal2Compositor::Resize(int width, int height) {
  if (m_width == width && m_height == height) {
    return;
  }

  ui::Log("Metal2Compositor Resize: width=%d, height=%d", width, height);

  m_width = width;
  m_height = height;

  // drawable size是像素坐标。需要换算成真实的像素。
  // 默认是view.bounds，会导致最终画面变虚一倍。
  CGSize size = { static_cast<CGFloat>(m_width), static_cast<CGFloat>(m_height) };
  [m_bind_layer setDrawableSize:size];

  // m_swapchain.MarkNeedReCreate();
}

}

@implementation Metal2CompositorDelegate {
  ui::Metal2Compositor *m_compositor;
  NSWindow *m_window;
}
- (Metal2CompositorDelegate *)initWith:(ui::Metal2Compositor *)compositor {
  m_compositor = compositor;
  m_window = nullptr;
  return self;
}

- (void)registerScreenNotification:(NSWindow *)window {
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

- (void)onDeviceChanged:(NSNotification *)notification {
  if (m_compositor) {
    m_compositor->CheckDeviceChanged();
  }
}
@end