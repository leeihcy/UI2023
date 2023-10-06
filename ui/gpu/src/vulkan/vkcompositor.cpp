#include "vkcompositor.h"
#include "vkapp.h"
#include "vklayer.h"
#include "vkvertext.h"
#include <cstdio>
#include <fstream>
#include <set>
#include <string>


#if defined(OS_MAC)
void *GetNSWindowRootView(void *window);
#endif

#define MAX_FRAMES_IN_FLIGHT 2

namespace ui {

static VulkanApplication &application() { return VulkanApplication::Get(); }

VulkanCompositor::VulkanCompositor() { m_pRootTexture = nullptr; }

VulkanCompositor::~VulkanCompositor() {
  vkDeviceWaitIdle(m_logical_device);

  auto &app = application();

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (m_renderFinishedSemaphores.size() <= i) {
      break;
    }
    vkDestroySemaphore(m_logical_device, m_renderFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(m_logical_device, m_imageAvailableSemaphores[i], nullptr);
    vkDestroyFence(m_logical_device, m_inFlightFences[i], nullptr);
  }

  vkDestroyCommandPool(m_logical_device, m_command_pool, nullptr);

  for (auto framebuffer : m_swapchain_framebuffers) {
    vkDestroyFramebuffer(m_logical_device, framebuffer, nullptr);
  }

  vkDestroyPipeline(m_logical_device, m_graphics_pipeline, nullptr);
  vkDestroyPipelineLayout(m_logical_device, m_pipeline_layout, nullptr);
  vkDestroyRenderPass(m_logical_device, m_renderpass, nullptr);

  for (auto imageView : m_swapchain_imageviews) {
    vkDestroyImageView(m_logical_device, imageView,
                       nullptr);
  }
  vkDestroySwapchainKHR(m_logical_device, m_swapchain, nullptr);

  vkDestroyDevice(m_logical_device, nullptr);
  vkDestroySurfaceKHR(app.GetVkInstance(), m_surface, nullptr);
}

GpuLayer *VulkanCompositor::GetRootLayerTexture() { return m_pRootTexture; }

void VulkanCompositor::Resize(int width, int height) {
  m_width = width;
  m_height = height;
}

IGpuLayer *VulkanCompositor::CreateLayerTexture() {
  GpuLayer *p = new VulkanGpuLayer(*this);
  p->SetGpuCompositor(this);
  return p;
}

void VulkanCompositor::SetRootLayerTexture(IGpuLayer *p) {
  m_pRootTexture = static_cast<GpuLayer *>(p);
}

bool VulkanCompositor::BeginCommit() { return true; }
void VulkanCompositor::EndCommit() {
  draw_frame();
}

bool VulkanCompositor::Initialize(void *hWnd) {
  m_hWnd = hWnd;

  create_vulkan_surface();

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
  if (!create_swapchain()) {
    printf("create_swapchain failed\n");
    return false;
  }
  if (!create_imageviews()) {
    printf("create_imageviews failed\n");
    return false;
  }
  if (!create_renderpass()) {
    printf("create_renderpass failed\n");
    return false;
  }
  if (!create_graphics_pipeline()) {
    printf("create_graphics_pipeline failed\n");
    return false;
  }
  if (!create_framebuffers()) {
    printf("create_framebuffers failed\n");
    return false;
  }
  if (!create_commandpool()) {
    printf("create_commandpool failed\n");
    return false;;
  }
  // if (!create_commandbuffer()) {
  //   printf("create_commandbuffer failed\n");
  //   return false;;
  // }
  if (!create_sync_objects()) {
    printf("create_sync_objects failed\n");
    return false;
  }
  return true;
}

bool VulkanCompositor::create_vulkan_surface() {
  auto &app = application();

#if defined(OS_WIN)
  VkWin32SurfaceCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  createInfo.hwnd = m_hWnd;
  createInfo.hinstance = GetModuleHandle(nullptr);

  vkCreateWin32SurfaceKHR(app.m_vk_instance, &createInfo, nullptr, &m_surface);
#elif defined(OS_MAC)
  // https://github.com/glfw/glfw/blob/master/src/cocoa_window.m

  VkMacOSSurfaceCreateInfoMVK createInfo;
  createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
  createInfo.pNext = NULL;
  createInfo.flags = 0;
  createInfo.pView = GetNSWindowRootView(m_hWnd);
  VkResult err = vkCreateMacOSSurfaceMVK(app.GetVkInstance(), &createInfo,
                                         nullptr, &m_surface);
  if (err != VK_SUCCESS) {
    return false;
  }

  // VkMetalSurfaceCreateInfoEXT createInfo;
  // createInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
  // createInfo.pNext = NULL;
  // createInfo.flags = 0;
  // // createInfo.pLayer = demo->caMetalLayer;

  // VkResult err =
  // vkCreateMetalSurfaceEXT(app.m_vk_instance,
  //                                        &createInfo, nullptr, &m_surface);

#endif
  return true;
}

bool VulkanCompositor::is_extension_support(
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

bool VulkanCompositor::pick_physical_device() {
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

bool VulkanCompositor::update_queue_family() {
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
    vkGetPhysicalDeviceSurfaceSupportKHR(m_physical_device, i, m_surface,
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

bool VulkanCompositor::create_logical_device() {
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
  device_create_info.queueCreateInfoCount = queue_create_infos.size();

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
  device_create_info.enabledExtensionCount = required_extensions.size();
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

VulkanCompositor::SurfaceInfo
VulkanCompositor::getSurfaceInfo() {
  SurfaceInfo details;

  // capabilities
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical_device, m_surface,
                                            &details.capabilities);

  // formats
  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, m_surface,
                                       &formatCount, nullptr);
  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, m_surface,
                                         &formatCount, details.formats.data());
  }

  // presentModes
  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical_device, m_surface,
                                            &presentModeCount, nullptr);
  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical_device, m_surface,
                                              &presentModeCount,
                                              details.presentModes.data());
  }

  return details;
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}
VkPresentModeKHR chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {  // mailbox
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                            int width, int height) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  } else {
    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

bool VulkanCompositor::create_swapchain() {
  auto &app = VulkanApplication::Get();

  SurfaceInfo surface_info = getSurfaceInfo();

  VkSurfaceFormatKHR surfaceFormat =
      chooseSwapSurfaceFormat(surface_info.formats);
  VkPresentModeKHR presentMode =
      chooseSwapPresentMode(surface_info.presentModes);
  VkExtent2D extent =
      chooseSwapExtent(surface_info.capabilities, m_width, m_height);

  uint32_t imageCount = surface_info.capabilities.minImageCount + 1;
  if (surface_info.capabilities.maxImageCount > 0) {
    imageCount = std::min(imageCount, surface_info.capabilities.maxImageCount);
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface          = m_surface;
  createInfo.minImageCount    = imageCount;
  createInfo.imageFormat      = surfaceFormat.format;
  createInfo.imageColorSpace  = surfaceFormat.colorSpace;
  createInfo.imageExtent      = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  createInfo.preTransform     = surface_info.capabilities.currentTransform;
  createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode      = presentMode;
  createInfo.clipped          = VK_TRUE;
  createInfo.oldSwapchain     = VK_NULL_HANDLE;

  if (m_graphics_queue_family != m_present_queue_family) {
    uint32_t queueFamilyIndices[] = { 
      (uint32_t)m_graphics_queue_family, 
      (uint32_t)m_present_queue_family 
    };
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = std::size(queueFamilyIndices);
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  if (vkCreateSwapchainKHR(m_logical_device, &createInfo, nullptr,
                           &m_swapchain) != VK_SUCCESS) {
    printf("failed to create swap chain!");
    return false;
  }

  VkResult result = vkGetSwapchainImagesKHR(m_logical_device, m_swapchain,
                                            &imageCount, nullptr);
  m_swapchain_images.resize(imageCount);
  result = vkGetSwapchainImagesKHR(m_logical_device, m_swapchain,
                                   &imageCount, m_swapchain_images.data());
  if (result != VK_SUCCESS) {
    printf("vkGetSwapchainImagesKHR failed, result = %d\n", result);
    return false;
  }

  m_swapchain_image_format = surfaceFormat.format;
  m_swapchain_extent = extent;
  return true;
}

bool VulkanCompositor::create_imageviews() {
  m_swapchain_imageviews.resize(m_swapchain_images.size());

  for (size_t i = 0; i < m_swapchain_images.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = m_swapchain_images[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = m_swapchain_image_format;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_logical_device,
                          &createInfo, nullptr,
                          &m_swapchain_imageviews[i]) != VK_SUCCESS) {
      printf("failed to create image views!");
      return false;
    }
  }
  return true;
}

bool VulkanCompositor::create_renderpass() {
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = m_swapchain_image_format;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  if (vkCreateRenderPass(m_logical_device, &renderPassInfo, nullptr, &m_renderpass) !=
      VK_SUCCESS) {
    return false;
  }
  return true;
}

static std::vector<char> read_file_content(const char* filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    return std::vector<char>();
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}

bool VulkanCompositor::create_shader_module(char *code, int length,
                                            VkShaderModule *out_module) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = length;
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code);

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(m_logical_device, &createInfo, nullptr, out_module) !=
      VK_SUCCESS) {
    return false;
  }

  return true;
}

bool VulkanCompositor::create_graphics_pipeline() { 
  auto vertShaderCode = read_file_content("shaders/vert.spv");
  auto fragShaderCode = read_file_content("shaders/frag.spv");

  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;
  if (!create_shader_module(vertShaderCode.data(), vertShaderCode.size(),
                            &vertShaderModule)) {
    return false;
  }
  if (!create_shader_module(fragShaderCode.data(), fragShaderCode.size(),
                            &fragShaderModule)) {
    return false;
  }

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  // ---
  // 顶点格式设置
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  // vertexInputInfo.vertexBindingDescriptionCount = 0;
  // vertexInputInfo.vertexAttributeDescriptionCount = 0;
  auto bindingDescription = ShaderVertex::getBindingDescription();
  auto attributeDescriptions = ShaderVertex::getAttributeDescriptions();

  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescriptions.size());
  vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

  // ---
  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)m_swapchain_extent.width;
  viewport.height = (float)m_swapchain_extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = m_swapchain_extent;

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pushConstantRangeCount = 0;

  bool result = true;
  if (vkCreatePipelineLayout(m_logical_device, &pipelineLayoutInfo, nullptr,
                             &m_pipeline_layout) != VK_SUCCESS) {
    result = false;
  }

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.layout = m_pipeline_layout;
  pipelineInfo.renderPass = m_renderpass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  if (result && vkCreateGraphicsPipelines(m_logical_device, VK_NULL_HANDLE, 1,
                                          &pipelineInfo, nullptr,
                                          &m_graphics_pipeline) != VK_SUCCESS) {
    result = false;
  }

  vkDestroyShaderModule(m_logical_device, fragShaderModule, nullptr);
  vkDestroyShaderModule(m_logical_device, vertShaderModule, nullptr);
  return result; 
}

bool VulkanCompositor::create_framebuffers() {
  m_swapchain_framebuffers.resize(m_swapchain_imageviews.size());

  for (size_t i = 0; i < m_swapchain_imageviews.size(); i++) {
    VkImageView attachments[] = {m_swapchain_imageviews[i]};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_renderpass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = m_swapchain_extent.width;
    framebufferInfo.height = m_swapchain_extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(m_logical_device, &framebufferInfo, nullptr,
                            &m_swapchain_framebuffers[i]) != VK_SUCCESS) {
      return false;
    }
  }
  return true;
}

bool VulkanCompositor::create_commandpool() {

  VkCommandPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = m_graphics_queue_family;

  if (vkCreateCommandPool(m_logical_device, &poolInfo, nullptr,
                          &m_command_pool) != VK_SUCCESS) {
    return false;
  }

  return true;
}


bool VulkanCompositor::create_commandbuffer(pfnCommandBufferRenderCallback callback, void* user_data) {
  m_command_buffers.resize(m_swapchain_framebuffers.size());

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = m_command_pool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t)m_command_buffers.size();

  if (vkAllocateCommandBuffers(m_logical_device, &allocInfo,
                               m_command_buffers.data()) != VK_SUCCESS) {
    return false;
  }

  for (size_t i = 0; i < m_command_buffers.size(); i++) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(m_command_buffers[i], &beginInfo) != VK_SUCCESS) {
      return false;
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderpass;
    renderPassInfo.framebuffer = m_swapchain_framebuffers[i];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_swapchain_extent;

    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(m_command_buffers[i], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

      vkCmdBindPipeline(m_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                        m_graphics_pipeline);

      callback(m_command_buffers[i], user_data);

    vkCmdEndRenderPass(m_command_buffers[i]);

    if (vkEndCommandBuffer(m_command_buffers[i]) != VK_SUCCESS) {
      return false;;
    }
  }
  return true;
}

bool VulkanCompositor::create_sync_objects() {
  m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
  m_imagesInFlight.resize(m_swapchain_images.size(), VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(m_logical_device, &semaphoreInfo, nullptr,
                          &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(m_logical_device, &semaphoreInfo, nullptr,
                          &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
        vkCreateFence(m_logical_device, &fenceInfo, nullptr,
                      &m_inFlightFences[i]) != VK_SUCCESS) {
      return false;
    }
  }
  return true;
}

bool VulkanCompositor::draw_frame() {
  vkWaitForFences(m_logical_device, 1, &m_inFlightFences[m_currentFrame],
                  VK_TRUE, UINT64_MAX);

  uint32_t imageIndex;
  vkAcquireNextImageKHR(m_logical_device, m_swapchain, UINT64_MAX,
                        m_imageAvailableSemaphores[m_currentFrame],
                        VK_NULL_HANDLE, &imageIndex);

  if (m_imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
    vkWaitForFences(m_logical_device, 1, &m_imagesInFlight[imageIndex], VK_TRUE,
                    UINT64_MAX);
  }
  m_imagesInFlight[imageIndex] = m_inFlightFences[m_currentFrame];

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &m_command_buffers[imageIndex];

  VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  vkResetFences(m_logical_device, 1, &m_inFlightFences[m_currentFrame]);

  if (vkQueueSubmit(m_graphics_queue, 1, &submitInfo,
                    m_inFlightFences[m_currentFrame]) != VK_SUCCESS) {
    return false;
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {m_swapchain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &imageIndex;

  vkQueuePresentKHR(m_present_queue, &presentInfo);

  m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

  return true;
}

VkCommandBuffer VulkanCompositor::BeginSingleTimeCommands() {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = m_command_pool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(m_logical_device, &allocInfo,
                           &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}

void VulkanCompositor::EndSingleTimeCommands(VkCommandBuffer commandBuffer) {
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(m_graphics_queue, 1, &submitInfo,
                VK_NULL_HANDLE);
  vkQueueWaitIdle(m_graphics_queue);

  vkFreeCommandBuffers(m_logical_device,
                       m_command_pool, 1, &commandBuffer);
}


void VulkanCompositor::VulkanCopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
  VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

  VkBufferCopy copyRegion{};
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  EndSingleTimeCommands(commandBuffer);
}

} // namespace ui