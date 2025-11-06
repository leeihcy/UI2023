#include "vkcompositor.h"
#include "base/stopwatch.h"
#include "gpu/include/api.h"
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include "vkapp.h"
#include "vklayer.h"

#include "vulkan/vulkan.h"
#include <memory>
#if defined(OS_WIN)
#include "vulkan/vulkan_win32.h"
#include "windows.h"
#endif

#if defined(OS_MAC)
void *GetNSWindowRootView(void *window);
void GetNSWindowActureSize(void *window, int *width, int *height);
#endif

#if defined(OS_LINUX)
#include <vulkan/vulkan_wayland.h>
#endif

namespace ui {

static VulkanApplication &application() { return VulkanApplication::GetInstance(); }

VulkanCompositor::VulkanCompositor()
    : m_device_queue(*this),
      m_command_pool(*static_cast<vulkan::IVulkanBridge *>(this)),
      m_swapchain(*static_cast<vulkan::IVulkanBridge *>(this)),
      m_renderpass(*static_cast<vulkan::IVulkanBridge *>(this)),
      m_pipeline(*static_cast<vulkan::IVulkanBridge *>(this)) {
}

VulkanCompositor::~VulkanCompositor() { destory(); }

void VulkanCompositor::destory() {
  VkDevice device = m_device_queue.Device();
  VkPhysicalDevice pysical_device = m_device_queue.PhysicalDevice();
  vkDeviceWaitIdle(m_device_queue.Device());

  m_pipeline.Destroy();
  m_renderpass.Destroy();
  m_swapchain.Destroy();

  auto &app = application();

  m_command_pool.Destroy();
  m_device_queue.Destroy();

  vkDestroySurfaceKHR(app.GetVkInstance(), m_surface, nullptr);
}


static void ReleaseGpuLayer(IGpuLayer* p) {
  if (p) {
    static_cast<VulkanGpuLayer*>(p)->Release();
  }
}
std::shared_ptr<IGpuLayer> VulkanCompositor::CreateLayerTexture() {
  auto p = std::shared_ptr<IGpuLayer>(
      new VulkanGpuLayer(*static_cast<vulkan::IVulkanBridge *>(this)),
      ReleaseGpuLayer);
  // p->SetGpuCompositor(this);
  return p;
}

// GpuLayer *VulkanCompositor::GetRootLayerTexture() { return m_pRootTexture; }
// void VulkanCompositor::SetRootLayerTexture(IGpuLayer *p) {
//   m_pRootTexture = static_cast<GpuLayer *>(p);
// }

bool VulkanCompositor::Initialize(IGpuCompositorWindow* window) {
  if (!create_vulkan_surface(window)) {
     printf("create_vulkan_surface failed\n");
    return false;
  }

  if (!m_device_queue.Initialize(window)) {
    printf("m_device_queue initialize failed\n");
    return false;
  }

  if (!m_command_pool.Initialize()) {
    printf("create_commandpool failed\n");
    return false;
  }

  return true;
}

void VulkanCompositor::Resize(int width, int height) {
  m_width = width;
  m_height = height;

  // 等待设备空闲
  vkDeviceWaitIdle(GetVkDevice());
  
  m_swapchain.DestroyForResize();

  if (!m_swapchain.Initialize(m_surface, m_width, m_height)) {
    printf("createSwapchain failed\n");
    return /*false*/;
  }

  VkFormat image_format = m_swapchain.ImageFormat();


  if (m_renderpass.handle() == VK_NULL_HANDLE) {
    m_renderpass.Create(image_format);
  }

  // pipe line应该只需要创建一次就够了。
  // 和窗口大小相关的viewport/scissor在每次commit时进行设置，参见UpdateViewportScissor
  if (m_pipeline.handle() == VK_NULL_HANDLE) {
    if (!m_pipeline.Initialize(m_swapchain.Extent2D().width,
                               m_swapchain.Extent2D().height, image_format)) {
      printf("create_graphics_pipeline failed\n");
      return /*false*/;
    }
  }

  if (!m_swapchain.CreateFrameBuffer(GetVkRenderPass())) {
    printf("m_swapchain CreateFramebuffer failed\n");
    return /*false*/;
  }
}

bool VulkanCompositor::create_vulkan_surface(IGpuCompositorWindow* window) {
  auto &app = application();

#if defined(OS_WIN)
  assert(window->GetType() == GpuCompositorWindowType::WindowsHWND);

  VkWin32SurfaceCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  createInfo.hwnd = (HWND)(((IGpuCompositorWindowHWND*)window)->GetHWND());
  createInfo.hinstance = GetModuleHandle(nullptr);

  vkCreateWin32SurfaceKHR(app.GetVkInstance(), &createInfo, nullptr,
                          &m_surface);
#elif defined(OS_MAC)
  // https://github.com/glfw/glfw/blob/master/src/cocoa_window.m
  assert(window->GetType() == GpuCompositorWindowType::MacOSNSView);

  VkMacOSSurfaceCreateInfoMVK createInfo;
  createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
  createInfo.pNext = NULL;
  createInfo.flags = 0;
  createInfo.pView = ((IGpuCompositorWindowNSView*)window)->GetNSWindowRootView(); 
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
#elif defined(OS_LINUX)
  if (window->GetType() == GpuCompositorWindowType::LinuxWayland) {
    IGpuCompositorWindowWayland* wayland = static_cast<IGpuCompositorWindowWayland*>(window);

    VkWaylandSurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    createInfo.display = wayland->GetWaylandDisplay(); 
    createInfo.surface = wayland->GetWaylandSurface();
    
    VkSurfaceKHR surface;
    VkResult result = vkCreateWaylandSurfaceKHR(app.GetVkInstance(), &createInfo,
                                                nullptr, &m_surface);
    if (result != VK_SUCCESS) {
      return false;
    }
  } else {
    return false;
  }

#endif
  return true;
}

// bool VulkanCompositor::createSwapchain() {
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
  cb->BeginRecordCommand();
  {
    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(cb->handle(), srcBuffer, dstBuffer, 1, &copyRegion);
  }
  cb->EndRecordCommand();

  m_device_queue.Submit(cb.get());
  m_device_queue.WaitIdle();
  cb->Destroy();
}

VkDevice VulkanCompositor::GetVkDevice() { return m_device_queue.Device(); }
VkPhysicalDevice VulkanCompositor::GetVkPhysicalDevice() {
  return m_device_queue.PhysicalDevice();
}
VkRenderPass VulkanCompositor::GetVkRenderPass() {
  return m_renderpass.handle();
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