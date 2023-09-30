#include "vkcompositor.h"
#include "vkapp.h"
#include "vklayer.h"

#if defined(OS_MAC)
void *GetNSWindowRootView(void *window);
#endif

namespace ui {

VunkanCompositor::VunkanCompositor(void *hWnd) {
  m_pRootTexture = nullptr;
  m_hWnd = hWnd;

  create_vulkan_surface();
}

VunkanCompositor::~VunkanCompositor() {
  vkDestroySurfaceKHR(VulkanApplication::Get().m_vk_instance, m_surface,
                      nullptr);
}

bool VunkanCompositor::create_vulkan_surface() {
  auto &app = VulkanApplication::Get();

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
  VkResult err = vkCreateMacOSSurfaceMVK(app.m_vk_instance, &createInfo,
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

  VkBool32 presentSupport = false;
  vkGetPhysicalDeviceSurfaceSupportKHR(
      app.m_physical_device, app.m_queue_families, m_surface, &presentSupport);
  if (!presentSupport) {
    vkDestroySurfaceKHR(app.m_vk_instance, m_surface, nullptr);
    return false;
  }
  return true;
}

IGpuLayer *VunkanCompositor::CreateLayerTexture() {
  GpuLayer *p = new VulkanGpuLayer;
  p->SetGpuCompositor(this);
  return p;
}

void VunkanCompositor::SetRootLayerTexture(IGpuLayer *p) {
  m_pRootTexture = static_cast<GpuLayer *>(p);
}

GpuLayer *VunkanCompositor::GetRootLayerTexture() { return m_pRootTexture; }

void VunkanCompositor::Resize(int width, int height) {}

bool VunkanCompositor::BeginCommit() { return true; }
void VunkanCompositor::EndCommit() {}

} // namespace ui