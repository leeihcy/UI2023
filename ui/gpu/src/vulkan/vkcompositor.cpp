#include "vkcompositor.h"
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include "vkapp.h"
#include "vklayer.h"
#include <_types/_uint32_t.h>
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

VulkanCompositor::VulkanCompositor()
    : m_device_queue(*this),
      m_command_pool(*static_cast<vulkan::IVulkanBridge *>(this)),
      m_swapchain(*static_cast<vulkan::IVulkanBridge *>(this)),
      m_pipeline(*static_cast<vulkan::IVulkanBridge *>(this)) {
  m_pRootTexture = nullptr;
}

VulkanCompositor::~VulkanCompositor() {
  VkDevice device = m_device_queue.Device();
  VkPhysicalDevice pysical_device = m_device_queue.PhysicalDevice();
  vkDeviceWaitIdle(m_device_queue.Device());

  auto &app = application();

  m_pipeline.Destroy();
  m_swapchain.Destroy();
  m_command_pool.Destroy();
  m_device_queue.Destroy();
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

  if (!m_device_queue.Initialize()) {
    printf("m_device_queue initialize\n");
    return false;
  }

  if (!m_swapchain.Initialize(m_surface, m_width, m_height)) {
    printf("create_swapchain failed\n");
    return false;
  }
  if (!m_pipeline.Initialize(m_swapchain.Extent2D().width,
                             m_swapchain.Extent2D().height,
                             m_swapchain.ImageFormat())) {
    printf("create_graphics_pipeline failed\n");
    return false;
  }
  if (!m_swapchain.CreateFrameBuffer()) {
    printf("m_swapchain CreateFramebuffer failed\n");
    return false;
  }
  if (!m_command_pool.Initialize()) {
    printf("create_commandpool failed\n");
    return false;;
  }
  if (!m_swapchain.CreateCommandBuffer()) {
    printf("CreateCommandBuffer failed\n");
    return false;;
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

// bool VulkanCompositor::create_swapchain() {
//   auto &app = VulkanApplication::Get();

//   SurfaceInfo surface_info = getSurfaceInfo();

//   VkSurfaceFormatKHR surfaceFormat =
//       chooseSwapSurfaceFormat(surface_info.formats);
//   VkPresentModeKHR presentMode =
//       chooseSwapPresentMode(surface_info.presentModes);
//   VkExtent2D extent =
//       chooseSwapExtent(surface_info.capabilities, m_width, m_height);

//   uint32_t imageCount = surface_info.capabilities.minImageCount + 1;
//   if (surface_info.capabilities.maxImageCount > 0) {
//     imageCount = std::min(imageCount, surface_info.capabilities.maxImageCount);
//   }

//   VkSwapchainCreateInfoKHR createInfo{};
//   createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
//   createInfo.surface          = m_surface;
//   createInfo.minImageCount    = imageCount;
//   createInfo.imageFormat      = surfaceFormat.format;
//   createInfo.imageColorSpace  = surfaceFormat.colorSpace;
//   createInfo.imageExtent      = extent;
//   createInfo.imageArrayLayers = 1;
//   createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
//   createInfo.preTransform     = surface_info.capabilities.currentTransform;
//   createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
//   createInfo.presentMode      = presentMode;
//   createInfo.clipped          = VK_TRUE;
//   createInfo.oldSwapchain     = VK_NULL_HANDLE;

//   int graphics_queue_family = m_device_queue.GraphicsQueueFamily();
//   int present_queue_family = m_device_queue.PresentQueueFamily();
//   if (graphics_queue_family != present_queue_family) {
//     uint32_t queueFamilyIndices[] = { 
//       (uint32_t)graphics_queue_family, 
//       (uint32_t)present_queue_family 
//     };
//     createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
//     createInfo.queueFamilyIndexCount = std::size(queueFamilyIndices);
//     createInfo.pQueueFamilyIndices = queueFamilyIndices;
//   } else {
//     createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
//   }

//   if (vkCreateSwapchainKHR(m_device_queue.Device(), &createInfo, nullptr,
//                            &m_swapchain) != VK_SUCCESS) {
//     printf("failed to create swap chain!");
//     return false;
//   }

//   VkResult result = vkGetSwapchainImagesKHR(m_device_queue.Device(), m_swapchain,
//                                             &imageCount, nullptr);
//   m_swapchain_images.resize(imageCount);
//   result = vkGetSwapchainImagesKHR(m_device_queue.Device(), m_swapchain,
//                                    &imageCount, m_swapchain_images.data());
//   if (result != VK_SUCCESS) {
//     printf("vkGetSwapchainImagesKHR failed, result = %d\n", result);
//     return false;
//   }

//   m_swapchain_image_format = surfaceFormat.format;
//   m_swapchain_extent = extent;
//   return true;
// }

 
bool VulkanCompositor::draw_frame() {
  //
  // 1. Wait for the previous frame to finish
  // 2. Acquire an image from the swap chain
  // 3. Record a command buffer which draws the scene onto that image
  // 4. Submit the recorded command buffer
  // 5. Present the swap chain image
  //

  draw_frame_wait_for_previous_frame_to_finish();
  draw_frame_acquire_image_from_swap_chain();
  draw_frame_record_command_buffer();
  draw_frame_submit_command_buffer();
  draw_frame_present_swap_chain();

#if 0
  vkWaitForFences(m_device_queue.Device(), 1, &m_inFlightFences[m_currentFrame],
                  VK_TRUE, UINT64_MAX);

  uint32_t imageIndex;
  vkAcquireNextImageKHR(m_device_queue.Device(), m_swapchain, UINT64_MAX,
                        m_imageAvailableSemaphores[m_currentFrame],
                        VK_NULL_HANDLE, &imageIndex);

  if (m_imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
    vkWaitForFences(m_device_queue.Device(), 1, &m_imagesInFlight[imageIndex], VK_TRUE,
                    UINT64_MAX);
  }
  m_imagesInFlight[imageIndex] = m_inFlightFences[m_currentFrame];

  // VkSubmitInfo submitInfo{};
  // submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  // VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
  // VkPipelineStageFlags waitStages[] = {
  //     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  // submitInfo.waitSemaphoreCount = 1;
  // submitInfo.pWaitSemaphores = waitSemaphores;
  // submitInfo.pWaitDstStageMask = waitStages;

  // submitInfo.commandBufferCount = 1;
  // submitInfo.pCommandBuffers = &m_command_buffers[imageIndex];

  // VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
  // submitInfo.signalSemaphoreCount = 1;
  // submitInfo.pSignalSemaphores = signalSemaphores;

  // vkResetFences(m_device_queue.Device(), 1, &m_inFlightFences[m_currentFrame]);

  // if (vkQueueSubmit(m_device_queue.GraphicsQueue(), 1, &submitInfo,
  //                   m_inFlightFences[m_currentFrame]) != VK_SUCCESS) {
  //   return false;
  // }

  // VkPresentInfoKHR presentInfo{};
  // presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  // presentInfo.waitSemaphoreCount = 1;
  // presentInfo.pWaitSemaphores = signalSemaphores;

  // VkSwapchainKHR swapChains[] = { m_swapchain.handle() };
  // presentInfo.swapchainCount = 1;
  // presentInfo.pSwapchains = swapChains;

  // presentInfo.pImageIndices = &imageIndex;

  // vkQueuePresentKHR(m_device_queue.PresentQueue(), &presentInfo);

  // m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
#endif
  return true;
}

void VulkanCompositor::VulkanCopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
  std::unique_ptr<vulkan::CommandBuffer> cb = m_command_pool.CreateBuffer();
  cb->Begin();
  {
    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(cb->handle(), srcBuffer, dstBuffer, 1, &copyRegion);
  }
  cb->End();

  m_device_queue.Submit(cb.get());
  m_device_queue.WaitIdle();
  cb->Destroy();
}

VkDevice VulkanCompositor::GetVkDevice() { return m_device_queue.Device(); }
VkPhysicalDevice VulkanCompositor::GetVkPhysicalDevice() {
  return m_device_queue.PhysicalDevice();
}
VkRenderPass VulkanCompositor::GetVkRenderPass() {
  return m_pipeline.GetVkRenderPass();
}
VkCommandPool VulkanCompositor::GetVkCommandPool() {
  return m_command_pool.handle();
}
VkPipeline VulkanCompositor::GetVkPipeline() {
  return m_pipeline.handle();
}
vulkan::CommandPool& VulkanCompositor::GetCommandPool() {
  return m_command_pool;
}
vulkan::SwapChain& VulkanCompositor::GetSwapChain() {
  return m_swapchain;
}
int VulkanCompositor::GetGraphicsQueueFamily() {
  return m_device_queue.GraphicsQueueFamily();
}
int VulkanCompositor::GetPresentQueueFamily() {
  return m_device_queue.PresentQueueFamily();
}

} // namespace ui