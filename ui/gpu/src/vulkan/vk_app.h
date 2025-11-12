#ifndef _UI_GPU_SRC_VULKAN_VKAPP_H_
#define _UI_GPU_SRC_VULKAN_VKAPP_H_
#include <vector>

#if defined(OS_MAC)
#define VK_USE_PLATFORM_MACOS_MVK
#endif
#include <vulkan/vulkan.h>

namespace ui {

class VulkanApplication {
public:
  VulkanApplication();
  static VulkanApplication &GetInstance();

public:
  bool Startup();
  void Shutdown();

public:
  VkInstance &GetVkInstance();
  bool IsValidationLayersEnabled();

private:
  bool createVulkanInstance();

  void getRequiredExtensions(std::vector<const char *> &requiredExtensions);
  void getRequiredLayers(std::vector<const char *> &requiredLayers);

  bool isExtensionSupport(const std::vector<const char *> &extensions);
  bool isLayerSupport(const std::vector<const char *> &layers);

private:
  bool m_enable_validation_layers = false;

  VkInstance m_vk_instance;
  VkDebugUtilsMessengerEXT m_debug_messenger;
};
} // namespace ui

#endif