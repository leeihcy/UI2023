#include "vk_compositor.h"
#include "src/util.h"
#include "include/api.h"
#include "src/vulkan/vk_bridge.h"
#include "vk_app.h"
#include "vk_layer.h"
#include "vulkan/vulkan_core.h"

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
      m_pipeline(*static_cast<vulkan::IVulkanBridge *>(this)),
      m_texture_tile_staging_buffer(*static_cast<vulkan::IVulkanBridge *>(this))
       {
}

VulkanCompositor::~VulkanCompositor() { destory(); }

void VulkanCompositor::destory() {
  VkDevice device = m_device_queue.Device();
  VkPhysicalDevice pysical_device = m_device_queue.PhysicalDevice();
  vkDeviceWaitIdle(m_device_queue.Device());

  m_texture_tile_staging_buffer.Destroy(false);

  m_pipeline.Destroy();
  m_renderpass.Destroy(device);
  m_swapchain.Destroy();

  auto &app = application();

  m_uniform_descriptor_pool.Destroy(device);
  m_texture_descriptor_pool.Destroy(device);
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
    ui::Log("createVulkanSurface failed");
    return false;
  }

  if (!m_device_queue.Initialize(window)) {
    ui::Log("m_device_queue initialize failed");
    return false;
  }

  if (!m_command_pool.Create(*static_cast<vulkan::IVulkanBridge *>(this))) {
    ui::Log("create_commandpool failed");
    return false;
  }

  createUniformDescriptorPool();
  if (!createTextureDescriptorPool()) {
    ui::Log("createTextureDescriptorPool failed");
    return false;
  }

  VkDeviceSize imageSize = TILE_SIZE * TILE_SIZE * 4;
  m_texture_tile_staging_buffer.CreateStaingBuffer(imageSize);

  VkFormat image_format = VK_FORMAT_B8G8R8A8_UNORM; //m_swapchain.ImageFormat();
  if (!m_renderpass) {
    m_renderpass.Create(GetVkDevice(), image_format);
  }

  // pipe line应该只需要创建一次就够了。
  // 和窗口大小相关的viewport/scissor在每次commit时进行设置，参见UpdateViewportScissor
  if (!m_pipeline.Create(1, 1, image_format)) {
    ui::Log("createGraphicsPipeline failed");
    return false;
  }

  return true;
}

void VulkanCompositor::Resize(int width, int height) {
  if (m_width == width && m_height == height) {
    return;
  }

  ui::Log("VulkanCompositor Resize: width=%d, height=%d", width, height);
  
  m_width = width;
  m_height = height;

  m_swapchain.MarkNeedReCreate();
}

void VulkanCompositor::OnSwapChainCreated() {
  if (!m_swapchain.CreateFrameBuffer(GetVkRenderPass())) {
    ui::Log("m_swapchain CreateFramebuffer failed");
    return /*false*/;
  }
}

bool VulkanCompositor::createUniformDescriptorPool() {
  if (m_uniform_descriptor_pool) {
    return true;
  }

  // 不依赖于swapchain，随便定义一个足够的数值5
  // int size = m_swapchain.Size();
  int size = 5;
  
  return m_uniform_descriptor_pool.CreateUniformBufferPool(GetVkDevice(), size);
}

bool VulkanCompositor::createTextureDescriptorPool() {
  if (m_texture_descriptor_pool) {
    return true;
  }

  // TODO:
  // 分配太多，会直接占用太多内存。 分配太少，可能会导致pool资源耗尽。
  // 因此还需要动态管理多个pool，当创建descriptor失败时创建一个新的pool。
  //
  // When a descriptor pool is allocated, you have to tell the 
  // driver how many descriptors sets, and what number of 
  // resources you will be using. A common thing to do is to 
  // default to some high numbers, like 1000 descriptors, and 
  // when the descriptor pool is out of space, allocating a 
  // new descriptor will return with an error. Then you can just 
  // go and create a new pool to hold more descriptors.
  //
  const int max_count = 100;
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