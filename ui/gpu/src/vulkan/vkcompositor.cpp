#include "vkcompositor.h"
#include "src/util.h"
#include "include/api.h"
#include "src/vulkan/vkbridge.h"
#include "vkapp.h"
#include "vklayer.h"

#if defined(OS_WIN)
#include "vulkan/vulkan_win32.h"
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
      m_swapchain(*static_cast<vulkan::IVulkanBridge *>(this)),
      m_pipeline(*static_cast<vulkan::IVulkanBridge *>(this)) {
}

VulkanCompositor::~VulkanCompositor() { destory(); }

void VulkanCompositor::destory() {
  VkDevice device = m_device_queue.Device();
  VkPhysicalDevice pysical_device = m_device_queue.PhysicalDevice();
  vkDeviceWaitIdle(m_device_queue.Device());

  m_uniform_descriptor_pool.Destroy(device);
  m_texture_descriptor_pool.Destroy(device);

  m_pipeline.Destroy();
  m_renderpass.Destroy(device);
  m_swapchain.Destroy();

  auto &app = application();

  m_command_pool.Destroy(device);
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
  return p;
}

bool VulkanCompositor::Initialize(IGpuCompositorWindow* window) {
  if (!createVulkanSurface(window)) {
     printf("createVulkanSurface failed\n");
    return false;
  }

  if (!m_device_queue.Initialize(window)) {
    printf("m_device_queue initialize failed\n");
    return false;
  }

  if (!m_command_pool.Create(*static_cast<vulkan::IVulkanBridge *>(this))) {
    printf("create_commandpool failed\n");
    return false;
  }
  return true;
}

void VulkanCompositor::Resize(int width, int height) {
  m_width = width;
  m_height = height;

  if (GetSwapChain().handle()) {
    return;
  }

  // 等待设备空闲
  vkDeviceWaitIdle(GetVkDevice());
  
  m_swapchain.DestroyForResize();

  if (!m_swapchain.Initialize(m_surface, m_width, m_height)) {
    printf("createSwapchain failed\n");
    return /*false*/;
  }

  VkFormat image_format = m_swapchain.ImageFormat();
  if (!m_renderpass) {
    m_renderpass.Create(GetVkDevice(), image_format);
  }

  // pipe line应该只需要创建一次就够了。
  // 和窗口大小相关的viewport/scissor在每次commit时进行设置，参见UpdateViewportScissor
  if (m_pipeline.handle() == VK_NULL_HANDLE) {
    if (!m_pipeline.Create(m_swapchain.Extent2D().width,
                               m_swapchain.Extent2D().height, image_format)) {
      printf("createGraphicsPipeline failed\n");
      return /*false*/;
    }
  }
  
  createUniformDescriptorPool();
  createTextureDescriptorPool();

  if (!m_swapchain.CreateFrameBuffer(GetVkRenderPass())) {
    printf("m_swapchain CreateFramebuffer failed\n");
    return /*false*/;
  }
}


bool VulkanCompositor::createUniformDescriptorPool() {
  if (m_uniform_descriptor_pool) {
    return true;
  }
  return m_uniform_descriptor_pool.CreateUniformBufferPool(GetVkDevice(), m_swapchain.Size());
}

bool VulkanCompositor::createTextureDescriptorPool() {
  if (m_texture_descriptor_pool) {
    return true;
  }
  // TODO:
  // 分配太多，会直接占用太多内存。
  // 分配太少，可能会导致pool资源耗尽。
  // 因此还需要动态管理多个pool
  const int max_count = 200;
  return m_texture_descriptor_pool.CreateTextureSamplePool(GetVkDevice(), max_count);
}

bool VulkanCompositor::createVulkanSurface(IGpuCompositorWindow* window) {
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

void VulkanCompositor::VulkanCopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                                        VkDeviceSize size) {
  Vk::CommandBuffer cb;
  cb.Attach(m_command_pool.AllocateCommandBuffer(GetVkDevice()));

  cb.BeginRecordCommand();
  {
    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(cb, srcBuffer, dstBuffer, 1, &copyRegion);
  }
  cb.EndRecordCommand();

  m_device_queue.Submit(cb);
  m_device_queue.WaitIdle();

  m_command_pool.ReleaseCommandBuffer(GetVkDevice(), cb.Detach());
}

VkDevice VulkanCompositor::GetVkDevice() { return m_device_queue.Device(); }
VkPhysicalDevice VulkanCompositor::GetVkPhysicalDevice() {
  return m_device_queue.PhysicalDevice();
}
VkCommandBuffer VulkanCompositor::GetCurrentCommandBuffer() {
  return m_swapchain.GetCurrentInflightFrame()->m_command_buffer;
}
int VulkanCompositor::GetGraphicsQueueFamily() {
  return m_device_queue.GraphicsQueueFamily();
}
int VulkanCompositor::GetPresentQueueFamily() {
  return m_device_queue.PresentQueueFamily();
}


} // namespace ui