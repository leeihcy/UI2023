#include "vkapp.h"
#include <_types/_uint32_t.h>

namespace ui {
static VulkanApplication s_app;

VulkanApplication::VulkanApplication() {}
VulkanApplication::~VulkanApplication() {}

VulkanApplication &VulkanApplication::Get() { return s_app; }

bool VulkanApplication::Startup() {

  if (!create_vulkan_instance()) {
    return false;
  }

  // 选择一块显卡设备
  if (!pick_physical_device()) {
    return false;
  }

  if (!create_logical_device()) {
    return false;
  }

  return true;
}

void VulkanApplication::Shutdown() {
  vkDestroyDevice(m_logical_device, nullptr);
  vkDestroyInstance(m_vk_instance, nullptr);
}

bool VulkanApplication::create_vulkan_instance() {
  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

  VkApplicationInfo application_info = {};
  application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  application_info.pApplicationName = "UIGPU";
  application_info.applicationVersion = 1;
  application_info.apiVersion = VK_API_VERSION_1_0;
  create_info.pApplicationInfo = &application_info;

  std::vector<const char *> requiredExtensions;

  // VK_ERROR_EXTENSION_NOT_PRESENT
#if defined(OS_MAC)
  requiredExtensions.push_back("VK_KHR_surface");
  requiredExtensions.push_back("VK_MVK_macos_surface"); // "VK_EXT_metal_surface"
#endif

  VkResult result = vkCreateInstance(&create_info, nullptr, &m_vk_instance);
#if defined(OS_MAC)
  // 修正mac上的SDK异常。
  if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    requiredExtensions.emplace_back(
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    create_info.enabledExtensionCount = requiredExtensions.size();
    create_info.ppEnabledExtensionNames = requiredExtensions.data();

    result = vkCreateInstance(&create_info, nullptr, &m_vk_instance);
  }
#endif
  if (result != VK_SUCCESS) {
    return false;
  }

  return true;
}

bool VulkanApplication::pick_physical_device() {
  uint32_t count = 0;
  VkResult result = vkEnumeratePhysicalDevices(m_vk_instance, &count, nullptr);
  if (result != VK_SUCCESS || count == 0) {
    return false;
  }

  std::vector<VkPhysicalDevice> devices(count);
  result = vkEnumeratePhysicalDevices(m_vk_instance, &count, devices.data());
  if (result != VK_SUCCESS) {
    return false;
  }

  bool found = false;
  int highest_score = 0;
  for (auto device : devices) {
    int score = 0;

    VkPhysicalDeviceProperties deviceProperties = {0};
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures = {0};
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // Application can't function without geometry shaders
    // if (!deviceFeatures.geometryShader) {
    //   continue;
    // }
    if (!find_queue_families(device).has_value()) {
      continue;
    }

    // 优先选择独显
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    if (score > highest_score) {
      found = true;
      m_physical_device = device;
    }
  }

  return found;
}
std::optional<uint32_t>
VulkanApplication::find_queue_families(VkPhysicalDevice device) {
  std::optional<uint32_t> graphicsFamily;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphicsFamily = i;
      break;
    }

    i++;
  }
  return graphicsFamily;
}

bool VulkanApplication::create_logical_device() {
  m_queue_families = find_queue_families(m_physical_device).value();

  VkDeviceQueueCreateInfo queueCreateInfo = {};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = m_queue_families;
  queueCreateInfo.queueCount = 1;

  float queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkPhysicalDeviceFeatures deviceFeatures = {};

  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;
  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = 0;

  // if (enableValidationLayers) {
  //     createInfo.enabledLayerCount =
  //     static_cast<uint32_t>(validationLayers.size());
  //     createInfo.ppEnabledLayerNames = validationLayers.data();
  // } else {
  //     createInfo.enabledLayerCount = 0;
  // }
  if (vkCreateDevice(m_physical_device, &createInfo, nullptr,
                     &m_logical_device) != VK_SUCCESS) {
    return false;
  }

  vkGetDeviceQueue(m_logical_device, m_queue_families, 0, &m_graphics_queue);
  return true;
}

} // namespace ui
