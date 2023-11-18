#include "vulkan_swap_chain.h"
#include "src/vulkan/vkapp.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include "src/vulkan/wrap/vulkan_image_view.h"
#include "vulkan/vulkan_core.h"
#include "vulkan_device_queue.h"
#include <_types/_uint32_t.h>
#include <memory>

namespace vulkan {

SwapChain::SwapChain(IVulkanBridge &bridge) : m_bridge(bridge) {
  memset(&m_info, 0, sizeof(m_info));
}

SwapChain::~SwapChain() {
  Destroy();
}

bool SwapChain::Initialize(VkSurfaceKHR surface, int width, int height) {
  if (!create_swapchain(surface, width, height)) {
    return false;
  }
  if (!init_swap_images()) {
    return false;
  }
  if (!init_sync()) {
    return false;
  }
  return true;
}

// 创建顺序依赖于render pass
bool SwapChain::CreateFrameBuffer() {
  for (auto& i : m_images) {
    i->CreateFrameBuffer(Extent2D().width, Extent2D().height);
  }
  return true;
}

// 创建顺序依赖于command pool
bool SwapChain::CreateCommandBuffer() {
  for (auto& i : m_sync_items) {
    i->CreateCommandBuffer();
  }
  return true;
}

void SwapChain::Destroy() {
  m_sync_items.clear();
  m_images.clear();

  if (m_swapchain != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(m_bridge.GetVkDevice(), m_swapchain, nullptr);
    m_swapchain = VK_NULL_HANDLE;
  }
}

bool SwapChain::create_swapchain(VkSurfaceKHR surface, int width, int height) {
  auto &app = ui::VulkanApplication::Get();

  m_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  m_info.surface = surface;

  if (!query_capabilities(surface, width, height)) {
    return false;
  }
  if (!query_formats(surface)) {
    return false;
  }
  if (!query_present_mode(surface)) {
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
    printf("failed to create swap chain!");
    return false;
  }

  return true;
}

bool SwapChain::query_capabilities(VkSurfaceKHR surface, int width,
                                   int height) {

  VkSurfaceCapabilitiesKHR capabilities = {0};
  VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      m_bridge.GetVkPhysicalDevice(), surface, &capabilities);
  if (result != VK_SUCCESS) {
    return false;
  }

  // image count
  const uint32_t desired_image_count = 3;
  uint32_t count = std::max(desired_image_count, capabilities.minImageCount);
  if (capabilities.maxImageCount > 0) {
    count = std::min(count, capabilities.maxImageCount);
  }

  m_info.minImageCount = count;

  // extent
  if (capabilities.currentExtent.width != UINT32_MAX) {
    m_info.imageExtent = capabilities.currentExtent;
  } else {
    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    m_info.imageExtent = actualExtent;
  }

  // transform
  m_info.preTransform = capabilities.currentTransform;

  return true;
}

bool SwapChain::query_formats(VkSurfaceKHR surface) {
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

bool SwapChain::query_present_mode(VkSurfaceKHR surface) {
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

  VkPresentModeKHR selected_mode = VK_PRESENT_MODE_FIFO_KHR;
  for (auto mode : modes) {
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      selected_mode = mode;
    }
  }

  m_info.presentMode = selected_mode;
  return true;
}

bool SwapChain::init_swap_images() {
  // 获取swap chain中的image列表
  uint32_t image_count = 0;
  VkResult result = vkGetSwapchainImagesKHR(m_bridge.GetVkDevice(), m_swapchain,
                                            &image_count, nullptr);
  if (result != VK_SUCCESS) {
    return false;
  }
  assert(image_count == m_info.minImageCount);

  // m_images.resize(image_count);
  std::vector<VkImage> images(image_count);

  result = vkGetSwapchainImagesKHR(m_bridge.GetVkDevice(), m_swapchain,
                                   &image_count, images.data());
  if (result != VK_SUCCESS) {
    printf("vkGetSwapchainImagesKHR failed, result = %d\n", result);
    return false;
  }

  for (uint32_t i = 0; i < image_count; ++i) {
    std::unique_ptr<SwapChainImage> image_item =
        std::make_unique<SwapChainImage>(m_bridge, images[i]);

    image_item->Create(m_info.imageFormat);

    m_images.push_back(std::move(image_item));
  }

  return true;
}

bool SwapChain::init_sync() {
  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    std::unique_ptr<SyncItem> item = std::make_unique<SyncItem>(m_bridge);
    item->Initialize();
    m_sync_items.push_back(std::move(item));
  }
  return true;
}

// bool SwapChain::AcquireNextImage() {
//   uint32_t imageIndex = 0;

//   // 当gpu能获取到image（使用完）的时候会激活acquire_semaphore
//   vkAcquireNextImageKHR(m_bridge.GetVkDevice(), m_swapchain, UINT64_MAX,
//                         m_images[m_current_image_index]->m_acquire_submit_semaphore,
//                         VK_NULL_HANDLE, &imageIndex);

//   return true;
// }

SwapChainImage* SwapChain::GetCurrentImage() {
  assert(m_current_image_index < m_images.size());

  return m_images[m_current_image_index].get();
}
SyncItem* SwapChain::GetCurrentSync() {
  assert(m_current_frame_index < m_sync_items.size());

  return m_sync_items[m_current_frame_index].get();
}

void SwapChain::IncCurrentFrame() {
  m_current_frame_index = (m_current_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
}
void SwapChain::SetCurrentImageIndex(uint32_t i) {
  m_current_image_index = i;
}
uint32_t SwapChain::GetCurrentImageIndex() {
  return m_current_frame_index;
}

} // namespace vulkan