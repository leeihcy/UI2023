#include "vkcompositor.h"
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include "vkapp.h"
#include "vklayer.h"
#include "base/stopwatch.h"

#include "vulkan/vulkan.h"
#if defined(OS_WIN)
#include "windows.h"
#include "vulkan/vulkan_win32.h"
#endif


#if defined(OS_MAC)
void *GetNSWindowRootView(void *window);
void GetNSWindowActureSize(void *window, int *width, int *height);
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
  destory();
}

void VulkanCompositor::destory() {
  VkDevice device = m_device_queue.Device();
  VkPhysicalDevice pysical_device = m_device_queue.PhysicalDevice();
  vkDeviceWaitIdle(m_device_queue.Device());

  destroy_swapchain();
  auto &app = application();

  
  m_command_pool.Destroy();
  m_device_queue.Destroy();

  vkDestroySurfaceKHR(app.GetVkInstance(), m_surface, nullptr);
}

void VulkanCompositor::destroy_swapchain() {
  m_pipeline.Destroy();
  m_swapchain.Destroy();
}

GpuLayer *VulkanCompositor::GetRootLayerTexture() { return m_pRootTexture; }

IGpuLayer *VulkanCompositor::CreateLayerTexture() {
  GpuLayer *p = new VulkanGpuLayer(*static_cast<vulkan::IVulkanBridge *>(this));
  p->SetGpuCompositor(this);
  return p;
}

void VulkanCompositor::SetRootLayerTexture(IGpuLayer *p) {
  m_pRootTexture = static_cast<GpuLayer *>(p);
}

//
// 1. Wait for the previous frame to finish
// 2. Acquire an image from the swap chain
// 3. Record a command buffer which draws the scene onto that image
// 使用command_buffer进行绘制。
// 4. Submit the recorded command buffer
// 5. Present the swap chain image
//
bool VulkanCompositor::BeginCommit(GpuLayerCommitContext *ctx) {
  assert(nullptr == m_current_command_buffer);

  StopWatch stop_watch;

  // 给每个tile一次更新descriptor set机会
  if (m_pRootTexture) {
    m_pRootTexture->OnBeginCommit(ctx);
  }

  draw_frame_wait_for_previous_frame_to_finish();
  draw_frame_acquire_image_from_swap_chain();
  m_current_command_buffer = draw_frame_begin_record_command_buffer();

  ctx->m_data = m_current_command_buffer;
  // vkCmdDraw(m_current_command_buffer->handle(), 3, 1, 0, 0);

  int ms = stop_watch.ElapseMicrosecondsSinceLast();
  printf("gpu begin commit cost %d ms\n", ms);

  return true;
}
void VulkanCompositor::EndCommit(GpuLayerCommitContext *) {

  StopWatch stop_watch;

  if (!m_current_command_buffer) {
    return;
  }
  draw_frame_end_record_command_buffer(m_current_command_buffer);
  m_current_command_buffer = nullptr;

  draw_frame_submit_command_buffer();
  draw_frame_present_swap_chain();

  int ms = stop_watch.ElapseMicrosecondsSinceLast();
  printf("gpu end commit cost %d 微秒\n", ms);
}

bool VulkanCompositor::Initialize(void *hWnd) {
  m_hWnd = hWnd;

  // 获取窗口大小
#if defined(OS_WIN)
  RECT rc;
  ::GetClientRect((HWND)m_hWnd, &rc);
  m_width = rc.right - rc.left;
  m_height = rc.bottom - rc.top;
#elif defined(OS_MAC)
  GetNSWindowActureSize(m_hWnd, &m_width, &m_height);
#endif

  create_vulkan_surface();

  if (!m_device_queue.Initialize()) {
    printf("m_device_queue initialize\n");
    return false;
  }

  if (!m_command_pool.Initialize()) {
    printf("create_commandpool failed\n");
    return false;
  }

  Resize(m_width, m_height);
  return true;
}

void VulkanCompositor::Resize(int width, int height) {
  m_width = width;
  m_height = height;

  vkDeviceWaitIdle(GetVkDevice());
  destroy_swapchain();
  
  if (!m_swapchain.Initialize(m_surface, m_width, m_height)) {
    printf("create_swapchain failed\n");
    return /*false*/;
  }

  VkFormat image_format = m_swapchain.ImageFormat();

  // ~~pipe line应该只需要创建一次就够了。~~
  if (m_pipeline.handle() == VK_NULL_HANDLE) {
    if (!m_pipeline.Initialize(m_swapchain.Extent2D().width,
                              m_swapchain.Extent2D().height,
                              image_format)) {
      printf("create_graphics_pipeline failed\n");
      return /*false*/;
    }
  }

  if (!m_swapchain.CreateFrameBuffer()) {
    printf("m_swapchain CreateFramebuffer failed\n");
    return /*false*/;
  }

  if (!m_swapchain.CreateCommandBuffer()) {
    printf("CreateCommandBuffer failed\n");
    return /*false*/;
  }
}

bool VulkanCompositor::create_vulkan_surface() {
  auto &app = application();

#if defined(OS_WIN)
  VkWin32SurfaceCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  createInfo.hwnd = (HWND)m_hWnd;
  createInfo.hinstance = GetModuleHandle(nullptr);

  vkCreateWin32SurfaceKHR(app.GetVkInstance(), &createInfo, nullptr, &m_surface);
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
//     imageCount = std::min(imageCount,
//     surface_info.capabilities.maxImageCount);
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

//   VkResult result = vkGetSwapchainImagesKHR(m_device_queue.Device(),
//   m_swapchain,
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

void VulkanCompositor::VulkanCopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                                        VkDeviceSize size) {
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
VkPipeline VulkanCompositor::GetVkPipeline() { return m_pipeline.handle(); }
vulkan::CommandPool &VulkanCompositor::GetCommandPool() {
  return m_command_pool;
}
vulkan::DeviceQueue &VulkanCompositor::GetDeviceQueue() {
  return m_device_queue;
}
vulkan::SwapChain &VulkanCompositor::GetSwapChain() { return m_swapchain; }
vulkan::Pipeline &VulkanCompositor::GetPipeline() { return m_pipeline; }
int VulkanCompositor::GetGraphicsQueueFamily() {
  return m_device_queue.GraphicsQueueFamily();
}
int VulkanCompositor::GetPresentQueueFamily() {
  return m_device_queue.PresentQueueFamily();
}

} // namespace ui