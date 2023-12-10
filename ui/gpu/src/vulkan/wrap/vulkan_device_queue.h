#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_DEVIDE_QUEUE_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_DEVIDE_QUEUE_H_
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <set>

namespace ui {
class VulkanCompositor;
}

namespace vulkan {

class DeviceQueue {
public:
  DeviceQueue(ui::VulkanCompositor& compositor);
  
public:
  bool Initialize();
  void Destroy();

  bool Submit(CommandBuffer* buffer);
  bool Submit(VkCommandBuffer* buffers, unsigned int count);
  bool WaitIdle();

  VkDevice Device() { return m_logical_device; }
  VkPhysicalDevice PhysicalDevice() { return m_physical_device; }
  VkQueue GraphicsQueue() { return m_graphics_queue; }
  VkQueue PresentQueue() { return m_present_queue; }
  int GraphicsQueueFamily() { return m_graphics_queue_family; }
  int PresentQueueFamily() { return m_present_queue_family; }

private:
  bool pick_physical_device();
  bool update_queue_family();
  bool create_logical_device();

  bool is_extension_support(VkPhysicalDevice physical_device, 
    const std::vector<const char *>& extensions);

private:
  ui::VulkanCompositor& m_compositor;
  
  VkPhysicalDevice m_physical_device;
  VkDevice m_logical_device;

  int m_graphics_queue_family = -1;
  int m_present_queue_family = -1;
  VkQueue m_graphics_queue;
  VkQueue m_present_queue;
};

}

#endif