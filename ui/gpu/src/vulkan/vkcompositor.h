#ifndef _UI_GPU_SRC_VULKAN_GPU_COMPOSITOR_H_
#define _UI_GPU_SRC_VULKAN_GPU_COMPOSITOR_H_
#include <vector>
#include "gpu/include/api.h"
#include "gpu/src/gpu_layer.h"
#include "gpu/src/vulkan/vkapp.h"

#if defined(OS_MAC)
#define VK_USE_PLATFORM_MACOS_MVK
// #define VK_USE_PLATFORM_METAL_EXT
#endif
#include <vulkan/vulkan.h>

namespace ui {

typedef void(*pfnCommandBufferRenderCallback)(VkCommandBuffer command_buffer, void* userdata);

// 一个compositor对应一个窗口
class VulkanCompositor : public IGpuCompositor {
public:
  VulkanCompositor();
  ~VulkanCompositor();

  bool Initialize(void *hWnd) override;
  void Release() override { delete this; }
  
  IGpuLayer *CreateLayerTexture() override;
  bool BeginCommit() override;
  void EndCommit() override;
  void Resize(int nWidth, int nHeight) override;

  void SetRootLayerTexture(IGpuLayer *p) override;
  GpuLayer *GetRootLayerTexture();

  VkPhysicalDevice GetVulkanPhysicalDevice() { return m_physical_device; }
  VkDevice GetVulkanDevice() { return m_logical_device; }
  VkQueue GetVulkanGraphicsQueue() { return m_graphics_queue; }
  VkQueue GetVulkanPresentQueue() { return m_present_queue; }
  VkCommandPool GetVulkanCommandPool() { return m_command_pool; }

  bool create_commandbuffer(pfnCommandBufferRenderCallback callback, void* user_data);
  void VulkanCopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  VkCommandBuffer BeginSingleTimeCommands();
  void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
  
private:
  bool create_vulkan_surface();
  bool pick_physical_device();
  bool update_queue_family();
  bool create_logical_device();
  bool create_swapchain();
  bool create_imageviews();
  bool create_renderpass();
  bool create_graphics_pipeline();
  bool create_framebuffers();
  bool create_commandpool();
  bool create_sync_objects();

  bool draw_frame();

  bool is_extension_support(VkPhysicalDevice physical_device, const std::vector<const char *>& extensions);
  struct SurfaceInfo {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };
  SurfaceInfo getSurfaceInfo();
  bool create_shader_module(char* code, int length, VkShaderModule* out);
  
private:
  void *m_hWnd;

  int m_width = 0;
  int m_height = 0;
  GpuLayer *m_pRootTexture;


  // 绑定到一个native窗口。
  VkSurfaceKHR m_surface;

  VkPhysicalDevice m_physical_device;
  VkDevice m_logical_device;

  int m_graphics_queue_family = -1;
  int m_present_queue_family = -1;
  VkQueue m_graphics_queue;
  VkQueue m_present_queue;

  VkSwapchainKHR m_swapchain;
  std::vector<VkImage> m_swapchain_images;
  VkFormat m_swapchain_image_format;
  VkExtent2D m_swapchain_extent;

  std::vector<VkImageView> m_swapchain_imageviews;
  std::vector<VkFramebuffer> m_swapchain_framebuffers;

  VkRenderPass m_renderpass;
  VkPipeline m_graphics_pipeline;
  VkPipelineLayout m_pipeline_layout;

  VkCommandPool m_command_pool;
  std::vector<VkCommandBuffer> m_command_buffers;

  std::vector<VkSemaphore> m_imageAvailableSemaphores;
  std::vector<VkSemaphore> m_renderFinishedSemaphores;
  std::vector<VkFence> m_inFlightFences;
  std::vector<VkFence> m_imagesInFlight;
  size_t m_currentFrame = 0;
};

} // namespace ui
#endif