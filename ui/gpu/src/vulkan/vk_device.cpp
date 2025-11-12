#include "vk_device.h"
#include <vulkan/vulkan_core.h>
#include <string>

#include "src/util.h"
#include "src/vulkan/vk_compositor.h"


#if defined(OS_MAC) 
std::string GetNSViewMetalDeviceName(/*NSView*/void *view);
#endif

namespace vulkan {

static ui::VulkanApplication &application() { return ui::VulkanApplication::GetInstance(); }

DeviceQueue::DeviceQueue(ui::VulkanCompositor& compositor):m_compositor(compositor) {
}

bool DeviceQueue::Initialize(ui::IGpuCompositorWindow* window) {
  // 选择一块显卡设备
  if (!pickPhysicalDevice(window)) {
    ui::Log("pickPhysicalDevice failed\n");
    return false;
  }
  // 查找相应队列
  if (!update_queue_family()) {
    ui::Log("update_queue_family failed\n");
    return false;
  }
  // 创建设备对象
  if (!create_logical_device()) {
    ui::Log("create_logical_device failed\n");
    return false;
  }
  return true;
}

void DeviceQueue::Destroy() {
  if (m_logical_device != VK_NULL_HANDLE) {
    ui::Log("vkDestroyDevice %p", m_logical_device);

    vkDestroyDevice(m_logical_device, nullptr);
    m_logical_device = VK_NULL_HANDLE;
  }
}

bool DeviceQueue::Submit(VkCommandBuffer buffer) {
  if (buffer == VK_NULL_HANDLE) { 
    return false;
  }
  VkCommandBuffer b = {buffer};
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
// 在macOS上，需要选择view所以屏幕对应的显卡，而不是强制独显。
// macboolpro 2015测试对比，内置屏幕默认是集显。此时
// 使用集显CPU消耗是13.0%, GPU 1.5%
// 使用独显CPU消耗是14.5%, GPU 11.0%
//
bool DeviceQueue::pickPhysicalDevice(ui::IGpuCompositorWindow* window) {
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

#if defined(OS_MAC)
  assert(window->GetType() == ui::GpuCompositorWindowType::MacOSNSView);
  std::string optimal_device_name = GetNSViewMetalDeviceName(
      ((ui::IGpuCompositorWindowNSView *)window)->GetNSWindowRootView());
#endif

  bool found = false;
  int highest_score = 0;
  std::string device_name;

  for (auto device : devices) {
    int score = 0;

    VkPhysicalDeviceProperties properties = {0};
    vkGetPhysicalDeviceProperties(device, &properties);
    printf("found graphics device: %s\n", properties.deviceName);

    VkPhysicalDeviceFeatures deviceFeatures = {0};
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // Application can't function without geometry shaders
    // if (!deviceFeatures.geometryShader) {
    //   continue;
    // }

    // 检测这个设备是否支持我们需要的所有扩展
    if (!isExtensionSupport(device, required_extensions)) {
      continue;
    }

#if defined(OS_MAC)
    // mac上选择当前窗口所在屏幕对应的显卡，才是最合适的。
    if (!optimal_device_name.empty() &&
        optimal_device_name == properties.deviceName) {
      score += 10000;
    }
#endif

    // 优先选择独显
    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += properties.limits.maxImageDimension2D;

#if defined(OS_LINUX)
    // llvmpipie是Mesa模拟的软件渲染器，是CPU模拟实现的，会消耗大量CPU。
    // 在这里降低它的分数权重。
    if (strstr(properties.deviceName, "llvmpipe") != nullptr) {
      score = score >> 2;
    }
#endif

    if (score > highest_score) {
      found = true;
      m_physical_device = device;
      device_name = properties.deviceName;
      highest_score = score;
    }
  }

  if (found) {
    printf("select pysical gpu: %s\n", device_name.c_str());
  }
  return found;
}


// Q: 为什么没有VK_QUEUE_PRESENT_BIT?
// A: present不涉及命令缓存，它只是一个动作，将图像转交给显卡内的Display Controller。
//    通过vkQueuePresentKHR 单独的API完成 Present，而不是放在CommandBuffer中。
//    
//    目前99%的显卡，graphics 和 present 都由同一个硬件队列来完成的。Metal / D3D12中
//    就不区分 graphics和present。
//
//    
bool DeviceQueue::update_queue_family() {
  auto &app = application();

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queueFamilyCount,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queueFamilyCount,
                                           queueFamilies.data());

  for (size_t i = 0; i < queueFamilies.size(); i++) {
    auto queueFamily = queueFamilies[i];
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      m_graphics_queue_family = (int)i;
    }

    // 查询当前队列族，是否支持present到我们的窗口(surface).
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(m_physical_device, (uint32_t)i, m_compositor.GetSurface(),
                                         &presentSupport);
    if (presentSupport) {
      m_present_queue_family = (int)i;
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
    ui::Log("vkCreateDevice failed\n");
    return false;
  }

  vkGetDeviceQueue(m_logical_device, m_graphics_queue_family, 0,
                   &m_graphics_queue);
  vkGetDeviceQueue(m_logical_device, m_present_queue_family, 0,
                   &m_present_queue);
  
  ui::Log("vkCreateDevice %p", m_logical_device);
  return true;
}


bool DeviceQueue::isExtensionSupport(
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