#include "vulkan_frame_buffer.h"
#include "src/vulkan/vkapp.h"
#include "vulkan_device_queue.h"

namespace vulkan {

FrameBuffer::FrameBuffer() {}

FrameBuffer::~FrameBuffer() {
  Destroy();
}

bool FrameBuffer::Initialize(VkImage image, VkFormat image_format) {
  
  return true;

}

void FrameBuffer::Destroy() {

}

} // namespace vulkan