#ifndef _UI_GPU_SRC_VULKAN_VKAPP_H_
#define _UI_GPU_SRC_VULKAN_VKAPP_H_
#include <_types/_uint32_t.h>
#include <vector>
#include <optional>
#include <vulkan/vulkan.h>

namespace ui {

class VulkanApplication {
public:
  VulkanApplication();
  ~VulkanApplication();

public:
  bool Startup();
  void Shutdown();

public:
  static VulkanApplication &Get();

private:
  bool create_vulkan_instance();
  bool pick_physical_device();
  bool create_logical_device();
  std::optional<uint32_t> find_queue_families(VkPhysicalDevice);

public:
  VkInstance m_vk_instance;
  VkPhysicalDevice m_physical_device;
  VkDevice m_logical_device;
  uint32_t m_queue_families = 0;
  VkQueue m_graphics_queue;

};
} // namespace ui

#endif