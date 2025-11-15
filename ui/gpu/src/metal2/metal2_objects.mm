#include "src/metal2/metal2_objects.h"

namespace metal2 {

bool CommandQueue::Create() {
  return true;
}

bool RenderPass::Create() {
  m_descriptor = [MTLRenderPassDescriptor new];
  m_descriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
  m_descriptor.colorAttachments[0].clearColor = MTLClearColorMake(1, 1, 1, 1);
  m_descriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
  return true;
}
bool RenderPass::Destroy() {
  if (m_descriptor) {
    [m_descriptor release];
    m_descriptor = nullptr;
  }
  return true;
}
void RenderPass::SetTexture(id<MTLTexture> texture) {
  m_descriptor.colorAttachments[0].texture = texture;
}

bool CommandBuffer::Create(id<MTLCommandQueue> command_queue) {
  m_command_buffer = [command_queue commandBuffer];
  return true;
}

}