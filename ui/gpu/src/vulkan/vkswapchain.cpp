#include "src/vulkan/vkswapchain.h"

#include <vulkan/vulkan.h>
#include <assert.h>

#include "src/util.h"
#include "src/vulkan/vkapp.h"

namespace vulkan {

InFlightFrame::InFlightFrame(IVulkanBridge &bridge) : m_bridge(bridge) {}

InFlightFrame::~InFlightFrame() { Destroy(); }

void InFlightFrame::Initialize() {
  VkFenceCreateInfo fenceInfo = {.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                 .flags = VK_FENCE_CREATE_SIGNALED_BIT};
  vkCreateFence(m_bridge.GetVkDevice(), &fenceInfo, nullptr, &m_command_buffer_fence);
}

void InFlightFrame::Destroy() {
  VkDevice device = m_bridge.GetVkDevice();

  m_command_buffer_fence.Destroy(device);
  m_bridge.GetCommandPool().ReleaseCommandBuffer(device, m_command_buffer.Detach());
}

bool InFlightFrame::CreateCommandBuffer() {
  assert (m_command_buffer.handle == VK_NULL_HANDLE);
  m_command_buffer.Attach(m_bridge.GetCommandPool().AllocateCommandBuffer(m_bridge.GetVkDevice()));
  return true;
}

// ---------------------------------------------------------------


GpuSemaphores::GpuSemaphores(IVulkanBridge &bridge) : m_bridge(bridge) {

}
GpuSemaphores::~GpuSemaphores() {
  Destroy();
}

GpuSemaphores::GpuSemaphores(GpuSemaphores&& o) : m_bridge(o.m_bridge) {
  this->m_semaphore_on_image_available = o.m_semaphore_on_image_available;
  this->m_semaphore_on_queue_submit_finish = o.m_semaphore_on_queue_submit_finish;
  o.m_semaphore_on_image_available = VK_NULL_HANDLE;
  o.m_semaphore_on_queue_submit_finish = VK_NULL_HANDLE;
}

void GpuSemaphores::Initialize() {
  VkDevice device = m_bridge.GetVkDevice();

  VkSemaphoreCreateInfo semaphoreInfo = {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  vkCreateSemaphore(m_bridge.GetVkDevice(), &semaphoreInfo, nullptr,
                    &m_semaphore_on_image_available);
  vkCreateSemaphore(m_bridge.GetVkDevice(), &semaphoreInfo, nullptr,
                    &m_semaphore_on_queue_submit_finish);
}

void GpuSemaphores::Destroy() {
    VkDevice device = m_bridge.GetVkDevice();

  // Destroy Semaphores.
  if (VK_NULL_HANDLE != m_semaphore_on_queue_submit_finish) {
    vkDestroySemaphore(device, m_semaphore_on_queue_submit_finish, nullptr);
    m_semaphore_on_queue_submit_finish = VK_NULL_HANDLE;
  }
  if (VK_NULL_HANDLE != m_semaphore_on_image_available) {
    vkDestroySemaphore(device, m_semaphore_on_image_available, nullptr);
    m_semaphore_on_image_available = VK_NULL_HANDLE;
  }
}

// -----------------------------------------------------------------------------------

SwapChain::SwapChain(IVulkanBridge &bridge) : m_bridge(bridge) {
  memset(&m_info, 0, sizeof(m_info));
}

SwapChain::~SwapChain() {
  Destroy();
}

bool SwapChain::Initialize(VkSurfaceKHR surface, int width, int height) {
  if (!createSwapchain(surface, width, height)) {
    return false;
  }
  if (!initSwapChainImages()) {
    return false;
  }
  if (!initInFlightFrames()) {
    return false;
  }
  return true;
}

// 创建顺序依赖于render pass
bool SwapChain::CreateFrameBuffer(VkRenderPass render_pass) {
  VkExtent2D extent = Extent2D();

  for (auto &image : m_images) {
    image->CreateFrameBuffer(extent.width, extent.height, render_pass);
    image->CreateDescriptorSet();
    image->CreateUniformBuffer();
  }
  return true;
}

void SwapChain::DestroyForResize() {
  m_images.clear();
  if (m_swapchain != VK_NULL_HANDLE) {
    ui::Log("Destroy Swapchain: %p", m_swapchain);
    
    vkDestroySwapchainKHR(m_bridge.GetVkDevice(), m_swapchain, nullptr);
    m_swapchain = VK_NULL_HANDLE;
  }
}

void SwapChain::Destroy() {
  m_inflight_frames.clear();
  m_gpu_semaphores.clear();
  m_images.clear();
  
  if (m_swapchain != VK_NULL_HANDLE) {
    ui::Log("Destroy Swapchain: %p", m_swapchain);

    vkDestroySwapchainKHR(m_bridge.GetVkDevice(), m_swapchain, nullptr);
    m_swapchain = VK_NULL_HANDLE;
  }
}

bool SwapChain::createSwapchain(VkSurfaceKHR surface, int width, int height) {
  auto &app = ui::VulkanApplication::GetInstance();

  m_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  m_info.surface = surface;

  if (!queryCapabilities(surface, width, height)) {
    return false;
  }
  if (!queryFormats(surface)) {
    return false;
  }
  if (!queryPresentMode(surface)) {
    return false;
  }

  m_info.imageArrayLayers = 1;
  m_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  m_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  m_info.clipped = VK_TRUE;
  m_info.oldSwapchain = VK_NULL_HANDLE;

  int graphics_queue_family = m_bridge.GetGraphicsQueueFamily();
  int present_queue_family = m_bridge.GetPresentQueueFamily();
  if (graphics_queue_family != present_queue_family) {
    uint32_t queueFamilyIndices[] = {(uint32_t)graphics_queue_family,
                                     (uint32_t)present_queue_family};
    m_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    m_info.queueFamilyIndexCount = std::size(queueFamilyIndices);
    m_info.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    m_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  if (vkCreateSwapchainKHR(m_bridge.GetVkDevice(), &m_info, nullptr,
                           &m_swapchain) != VK_SUCCESS) {
    ui::Log("failed to create swap chain!");
    return false;
  }
  ui::Log("Create Swapchain: %p", m_swapchain);

  return true;
}

bool SwapChain::queryCapabilities(VkSurfaceKHR surface, int width,
                                   int height) {

  VkSurfaceCapabilitiesKHR capabilities = {0};
  VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      m_bridge.GetVkPhysicalDevice(), surface, &capabilities);
  if (result != VK_SUCCESS) {
    return false;
  }

  // 查询swap chain管理的VkImage数量(minImageCount)：
  // 双缓冲：2个。一个用于当前显示的帧，另一个用于下一帧的渲染。
  // 三缓冲：3个。
  // Image数量越多，内存占用越多，但可能减少GPU空闲等待。
  const uint32_t desired_image_count = 3;
  m_info.minImageCount =
      std::clamp(desired_image_count, capabilities.minImageCount,
                 capabilities.maxImageCount);

  // extent
  if (width > 0 && height > 0) {
    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    m_info.imageExtent = actualExtent;
  } else if (capabilities.currentExtent.width != UINT32_MAX) {
    m_info.imageExtent = capabilities.currentExtent;
  }

  // 用于控制交换链图像（Swapchain Images）在呈现（Present）
  // 到屏幕之前的 几何变换，比如旋转、镜像或缩放。
  m_info.preTransform = capabilities.currentTransform;

  return true;
}

bool SwapChain::queryFormats(VkSurfaceKHR surface) {
  std::vector<VkSurfaceFormatKHR> formats;

  uint32_t count = 0;
  VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(
      m_bridge.GetVkPhysicalDevice(), surface, &count, nullptr);
  if (result != VK_SUCCESS || count == 0) {
    return false;
  }
  formats.resize(count);
  result = vkGetPhysicalDeviceSurfaceFormatsKHR(
      m_bridge.GetVkPhysicalDevice(), surface, &count, formats.data());
  if (result != VK_SUCCESS) {
    return false;
  }

  const VkSurfaceFormatKHR *selected_format = &formats[0];
  for (const auto &format : formats) {
    if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      selected_format = &format;
    }
  }

  m_info.imageFormat = selected_format->format;
  m_info.imageColorSpace = selected_format->colorSpace;

  return true;
}

bool SwapChain::queryPresentMode(VkSurfaceKHR surface) {
  std::vector<VkPresentModeKHR> modes;

  uint32_t count = 0;
  VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(
      m_bridge.GetVkPhysicalDevice(), surface, &count, nullptr);
  if (result != VK_SUCCESS || count == 0) {
    return false;
  }
  modes.resize(count);
  result = vkGetPhysicalDeviceSurfacePresentModesKHR(
      m_bridge.GetVkPhysicalDevice(), surface, &count, modes.data());
  if (result != VK_SUCCESS) {
    return false;
  }

  //
  // FIFO: 垂直同步。如果队列已满，vkQueuePresentKHR将阻塞。
  // FIFO_RELAXED: 当帧率小于刷新率时，立即显示。大于时垂直同步。
  // MAILBOX: 垂直同步，并覆盖队列中已有数据（队列长度为1）。追求低延迟的场景。
  // IMMEDIATE: 立即模式，关闭垂直同步。
  //
  VkPresentModeKHR selected_mode = VK_PRESENT_MODE_FIFO_KHR;
  for (auto mode : modes) {
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      selected_mode = mode;
    }
  }

  m_info.presentMode = selected_mode;
  return true;
}

bool SwapChain::initSwapChainImages() {
  assert (m_images.empty());

  // 获取swap chain中的image列表
  uint32_t image_count = 0;
  VkResult result = vkGetSwapchainImagesKHR(m_bridge.GetVkDevice(), m_swapchain,
                                            &image_count, nullptr);
  if (result != VK_SUCCESS) {
    return false;
  }
  assert(image_count == m_info.minImageCount);

  std::vector<VkImage> swapchain_images(image_count);
  result = vkGetSwapchainImagesKHR(m_bridge.GetVkDevice(), m_swapchain,
                                   &image_count, swapchain_images.data());
  if (result != VK_SUCCESS) {
    printf("vkGetSwapchainImagesKHR failed, result = %d\n", result);
    return false;
  }

  for (uint32_t i = 0; i < image_count; ++i) {
    std::unique_ptr<SwapChainFrame> image_item =
        std::make_unique<SwapChainFrame>(m_bridge, swapchain_images[i]);

    image_item->Create(m_info.imageFormat);

    m_images.push_back(std::move(image_item));
  }

  return true;
}

//
// swapchain image count指的是present队列，用于垂直同步。
// MAX_FRAMES_IN_FLIGHT 指的是渲染队列，这两不需要一致。
//
// 最常用的配置为：
// in flight = 2
// swap chain image count = 3
// 这样总能顺利的 vkAcquireNextImageKHR 拿到一个空闲交换链图像。
//
bool SwapChain::initInFlightFrames() {
  if (m_inflight_frames.empty()) {
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      std::unique_ptr<InFlightFrame> item = std::make_unique<InFlightFrame>(m_bridge);
      item->Initialize();
      item->CreateCommandBuffer();
      m_inflight_frames.push_back(std::move(item));
    }
  }

  if (m_gpu_semaphores.empty()) {
    size_t size = m_images.size();
    for (size_t i = 0; i < size; i++) {
      auto item = std::make_unique<GpuSemaphores>(m_bridge);
      item->Initialize();
      m_gpu_semaphores.push_back(std::move(item));
    }
  }
  return true;
}

SwapChainFrame* SwapChain::GetCurrentFrame() {
  assert(m_current_image_index < m_images.size());

  return m_images[m_current_image_index].get();
}
InFlightFrame* SwapChain::GetCurrentInflightFrame() {
    if (m_current_inflight_frame_index >= m_inflight_frames.size()) {
        int a = 0;
    }
  assert(m_current_inflight_frame_index < m_inflight_frames.size());

  return m_inflight_frames[m_current_inflight_frame_index].get();
}
GpuSemaphores* SwapChain::GetCurrentSemaphores() {
  assert (m_current_semaphore_index < m_images.size());

  return  m_gpu_semaphores[m_current_semaphore_index].get();
}

void SwapChain::IncCurrentInflightFrame() {
  m_current_inflight_frame_index = (m_current_inflight_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
}
void SwapChain::IncCurrentSemaphores() {
  size_t size = m_gpu_semaphores.size();
  if (0 == size) { 
    return; 
  }

  m_current_semaphore_index = (m_current_semaphore_index + 1) % size;
}

void SwapChain::SetCurrentImageIndex(uint32_t i) {
  m_current_image_index = i;
}
uint32_t SwapChain::GetCurrentImageIndex() {
  return m_current_image_index;
}

} // namespace vulkan