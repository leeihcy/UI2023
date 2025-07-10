#include "vulkan_device_queue.h"
#include "vulkan/vulkan_core.h"
#include "src/vulkan/vkcompositor.h"
#include <cstdint>
#include <string>

namespace vulkan {

static ui::VulkanApplication &application() { return ui::VulkanApplication::Get(); }

DeviceQueue::DeviceQueue(ui::VulkanCompositor& compositor):m_compositor(compositor) {
}

bool DeviceQueue::Initialize() {
  // 选择一块显卡设备
  if (!pick_physical_device()) {
    printf("pick_physical_device failed\n");
    return false;
  }
  // 查找相应队列
  if (!update_queue_family()) {
    printf("update_queue_family failed\n");
    return false;
  }
  // 创建设备对象
  if (!create_logical_device()) {
    printf("create_logical_device failed\n");
    return false;
  }
  return true;
}

void DeviceQueue::Destroy() {
  vkDestroyDevice(m_logical_device, nullptr);
}

bool DeviceQueue::Submit(CommandBuffer* buffer) {
  if (!buffer) { 
    return false;
  }
  VkCommandBuffer b = buffer->handle();
  return Submit(&b, 1);
}

bool DeviceQueue::Submit(VkCommandBuffer *buffers, unsigned int count) {

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = count;
  submitInfo.pCommandBuffers = buffers;

  vkQueueSubmit(m_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
  return true;
}
bool DeviceQueue::WaitIdle() {
  vkQueueWaitIdle(m_graphics_queue);
  return true;
}

// 
// 设备名称： llvmpipe (LLVM 19.1.1, 256 bits)
//  driver:  /usr/lib/x86_64-linux-gnu/libvulkan_lvp.so
// 设备名称：Intel(R) Iris(R) Pro Graphics P5200 (HSW GT3)
//  driver:  /usr/lib/x86_64-linux-gnu/libvulkan_intel_hasvk.so
//
bool DeviceQueue::pick_physical_device() {
  auto &app = application();
  uint32_t count = 0;
  VkResult result =
      vkEnumeratePhysicalDevices(app.GetVkInstance(), &count, nullptr);
  if (result != VK_SUCCESS || count == 0) {
    return false;
  }

  std::vector<VkPhysicalDevice> devices(count);
  result =
      vkEnumeratePhysicalDevices(app.GetVkInstance(), &count, devices.data());
  if (result != VK_SUCCESS) {
    return false;
  }

  std::vector<const char *> required_extensions;
  required_extensions.push_back(
      VK_KHR_SWAPCHAIN_EXTENSION_NAME); // VK_KHR_swapchain

  bool found = false;
  int highest_score = 0;
  std::string device_name;

  for (auto device : devices) {
    int score = 0;

    VkPhysicalDeviceProperties deviceProperties = {0};
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    printf("found graphics device: %s\n", deviceProperties.deviceName);

    VkPhysicalDeviceFeatures deviceFeatures = {0};
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // Application can't function without geometry shaders
    // if (!deviceFeatures.geometryShader) {
    //   continue;
    // }

    // 检测这个设备是否支持我们需要的所有扩展
    if (!is_extension_support(device, required_extensions)) {
      continue;
    }

    // 优先选择独显
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

#if defined(OS_LINUX)
    // llvmpipie是Mesa模拟的软件渲染器，为CPU实现，降低分数。
    if (strstr(deviceProperties.deviceName, "llvmpipe") != nullptr) {
      score = score >> 2;
    }
#endif

    if (score > highest_score) {
      found = true;
      m_physical_device = device;
      device_name = deviceProperties.deviceName;
      highest_score = score;
    }
  }

  if (found) {
    printf("select pysical gpu: %s\n", device_name.c_str());
  }
  return found;
}

bool DeviceQueue::update_queue_family() {
  auto &app = application();

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queueFamilyCount,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      m_graphics_queue_family = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(m_physical_device, i, m_compositor.Surface(),
                                         &presentSupport);
    if (presentSupport) {
      m_present_queue_family = i;
    }

    if (m_graphics_queue_family >= 0 && m_present_queue_family >= 0) {
      return true;
    }
  }

  if (m_graphics_queue_family == -1 || m_present_queue_family == -1) {
    return false;
  }
  return true;
}

bool DeviceQueue::create_logical_device() {
  VkDeviceCreateInfo device_create_info = {};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  // queues
  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

  // 使用set进行去重。两个queue可能是同一个队列索引
  std::set<uint32_t> queue_family_set = {(uint32_t)m_graphics_queue_family,
                                         (uint32_t)m_present_queue_family};
  for (uint32_t queue_family : queue_family_set) {
    VkDeviceQueueCreateInfo queue_info = {};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.queueFamilyIndex = queue_family;
    queue_info.queueCount = 1;
    float queuePriority = 1.0f;
    queue_info.pQueuePriorities = &queuePriority;

    queue_create_infos.push_back(queue_info);
  }
  device_create_info.pQueueCreateInfos = queue_create_infos.data();
  device_create_info.queueCreateInfoCount = (uint32_t)queue_create_infos.size();

  // features
  VkPhysicalDeviceFeatures deviceFeatures = {};
  device_create_info.pEnabledFeatures = &deviceFeatures;

  // extension
  // 使用交换链需要VK_KHR_swapchain首先启用扩展
  std::vector<const char *> required_extensions;
  required_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#if defined(OS_MAC)
  required_extensions.push_back("VK_KHR_portability_subset");
#endif
  device_create_info.enabledExtensionCount = (uint32_t)required_extensions.size();
  device_create_info.ppEnabledExtensionNames = required_extensions.data();

  // layers
  // std::vector<const char *> validationLayers;
  // if (application().IsValidationLayersEnabled()) {
  //   validationLayers.push_back("VK_LAYER_KHRONOS_validation");
  //   device_create_info.enabledLayerCount =
  //       static_cast<uint32_t>(validationLayers.size());
  //   device_create_info.ppEnabledLayerNames = validationLayers.data();
  // } else {
  //   device_create_info.enabledLayerCount = 0;
  // }

  if (vkCreateDevice(m_physical_device, &device_create_info, nullptr,
                     &m_logical_device) != VK_SUCCESS) {
    printf("vkCreateDevice failed\n");
    return false;
  }

  vkGetDeviceQueue(m_logical_device, m_graphics_queue_family, 0,
                   &m_graphics_queue);
  vkGetDeviceQueue(m_logical_device, m_present_queue_family, 0,
                   &m_present_queue);

  return true;
}


bool DeviceQueue::is_extension_support(
    VkPhysicalDevice physical_device,
    const std::vector<const char *> &extensions) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr,
                                       &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(
      physical_device, nullptr, &extensionCount, availableExtensions.data());

  std::set<std::string> set_extensions; // 使用std::string进行字符串相等的比较。
  for (auto iter : extensions) {
    set_extensions.insert(iter);
  }
  for (const auto &extension : availableExtensions) {
    set_extensions.erase(extension.extensionName);
  }

  return set_extensions.empty();
}

}