#ifndef _UI_GPU_SRC_VULKAN_VKAPP_H_
#define _UI_GPU_SRC_VULKAN_VKAPP_H_
#include <_types/_uint32_t.h>
#include <optional>
#include <vector>

#if defined(OS_MAC)
#define VK_USE_PLATFORM_MACOS_MVK
#endif
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
  VkInstance &GetVkInstance();

  bool IsValidationLayersEnabled();
  
private:
  bool create_vulkan_instance();
  
  void get_required_extensions(std::vector<const char *> &requiredExtensions);
  void get_required_layers(std::vector<const char *> &requiredLayers);

  bool is_extension_support(const std::vector<const char *>& extensions);
  bool is_layer_support(const std::vector<const char *>& layers);

private:
  bool m_enable_validation_layers = false;
  
  VkInstance m_vk_instance;
  VkDebugUtilsMessengerEXT m_debug_messenger;

};
} // namespace ui

#endif