#include "src/metal2/metal2_compositor.h"
#include "src/metal2/metal2_app.h"
#include "src/metal2/metal2_bridge.h"
#include "src/metal2/metal2_layer.h"
#include "src/util.h"

namespace ui {


typedef enum AAPLVertexInputIndex
{
    AAPLVertexInputIndexVertices = 0,
    AAPLVertexInputIndexUniforms = 1,
} AAPLVertexInputIndex;

typedef struct
{
    // Positions in pixel space (i.e. a value of 100 indicates 100 pixels from the origin/center)
    vector_float2 position;

    // 2D texture coordinate
    vector_float3 color;
} AAPLVertex;

typedef struct
{
    float scale;
    vector_uint2 viewportSize;
} Uniforms;


Metal2Compositor::Metal2Compositor() : m_pipeline(*static_cast<IMetal2Bridge*>(this)){
  m_delgate = [[Metal2CompositorDelegate alloc] initWith:this];
}
Metal2Compositor::~Metal2Compositor() {
  if (m_delgate) {
    [m_delgate release];
    m_delgate = nullptr;
  }
}

std::shared_ptr<IGpuLayer> Metal2Compositor::CreateLayerTexture() {
  return std::shared_ptr<IGpuLayer>(new Metal2GpuLayer());
}

bool Metal2Compositor::Initialize(IGpuCompositorWindow *window) {
  assert(window->GetType() == GpuCompositorWindowType::MacOSNSView);
  NSView* view = (NSView*)((IGpuCompositorWindowNSView*)window)->GetNSWindowRootView(); 
  m_bind_view = view;
  m_bind_layer = (CAMetalLayer*)view.layer;
  NSWindow *nswindow = m_bind_view.window;

  id<MTLDevice> device = fetchDevice(m_bind_view);
  if (device == nil) {
    return false;
  }
  printf("fetch device: %s\n", device.name.UTF8String);
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
  // Set the device for the layer so the layer can create drawable textures that can be rendered to
    // on this device.
  m_bind_layer.device = m_device;


  m_command_queue = [m_device newCommandQueue];
  m_command_buffer.Create(m_command_queue);
  m_pipeline.Create();
  m_render_pass.Create();

  // Set up a simple MTLBuffer with the vertices, including position and texture
  // coordinates
  static const AAPLVertex quadVertices[] = {
      // Pixel positions, Color coordinates
      {{250, -250}, {1.f, 0.f, 0.f}}, {{-250, -250}, {0.f, 1.f, 0.f}},
      {{-250, 250}, {0.f, 0.f, 1.f}},

      {{250, -250}, {1.f, 0.f, 0.f}}, {{-250, 250}, {0.f, 0.f, 1.f}},
      {{250, 250}, {1.f, 0.f, 1.f}},
  };

  // Create a vertex buffer, and initialize it with the vertex data.
  _vertices = [m_device newBufferWithBytes:quadVertices
                                    length:sizeof(quadVertices)
                                   options:MTLResourceStorageModeShared];
}

void Metal2Compositor::OnSwapChainCreated() {
  
}

bool Metal2Compositor::BeginCommit(GpuLayerCommitContext *) { return true; }
void Metal2Compositor::EndCommit(GpuLayerCommitContext *) {
  Present();
}
void Metal2Compositor::Resize(int width, int height) {
  if (m_width == width && m_height == height) {
    return;
  }

  ui::Log("Metal2Compositor Resize: width=%d, height=%d", width, height);
  
  m_width = width;
  m_height = height;

  // m_swapchain.MarkNeedReCreate();
}


void Metal2Compositor::Present() {
  // Create a new command buffer for each render pass to the current drawable.
  id<MTLCommandBuffer> commandBuffer = [m_command_queue commandBuffer];
  id<CAMetalDrawable> currentDrawable = [m_bind_layer nextDrawable];
  // If the current drawable is nil, skip rendering this frame
    if(!currentDrawable)
    {
        return;
    }

   m_render_pass.SetTexture(currentDrawable.texture);
    
    id <MTLRenderCommandEncoder> renderEncoder =
        [commandBuffer renderCommandEncoderWithDescriptor:m_render_pass.m_descriptor];


    [renderEncoder setRenderPipelineState:m_pipeline.m_state];

    [renderEncoder setVertexBuffer:_vertices
                            offset:0
                           atIndex:AAPLVertexInputIndexVertices ];

    {
        Uniforms uniforms;

#if ANIMATION_RENDERING
        uniforms.scale = 0.5 + (1.0 + 0.5 * sin(_frameNum * 0.1));
#else
        uniforms.scale = 1.0;
#endif
        uniforms.viewportSize.x = m_width;
        uniforms.viewportSize.y = m_height;

        [renderEncoder setVertexBytes:&uniforms
                               length:sizeof(uniforms)
                              atIndex:AAPLVertexInputIndexUniforms ];
    }

    [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:6];

    [renderEncoder endEncoding];

    [commandBuffer presentDrawable:currentDrawable];

    [commandBuffer commit];
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