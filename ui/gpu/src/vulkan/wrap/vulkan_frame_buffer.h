#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_FRAME_BUFFER_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_FRAME_BUFFER_H_
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace vulkan {
class DeviceQueue;

class FrameBuffer {
public:
  FrameBuffer();
  ~FrameBuffer();

  bool Initialize(VkImage image, VkFormat image_format);
  void Destroy();

private:

private:

};

}

#endif