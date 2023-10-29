#ifndef _UI_GPU_SRC_VULKAN_GPU_COMPOSITOR_H_
#define _UI_GPU_SRC_VULKAN_GPU_COMPOSITOR_H_
#include <vector>
#include "gpu/include/api.h"
#include "gpu/src/gpu_layer.h"
#include "gpu/src/vulkan/vkapp.h"
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_command_pool.h"
#include "src/vulkan/wrap/vulkan_device_queue.h"
#include "src/vulkan/wrap/vulkan_pipe_line.h"
#include "src/vulkan/wrap/vulkan_swap_chain.h"
#include <vulkan/vulkan.h>

#if defined(OS_MAC)
#define VK_USE_PLATFORM_MACOS_MVK
// #define VK_USE_PLATFORM_METAL_EXT
#endif

namespace ui {

typedef void(*pfnCommandBufferRenderCallback)(VkCommandBuffer command_buffer, void* userdata);

// 一个compositor对应一个窗口
class VulkanCompositor : 
  public IGpuCompositor, 
  public vulkan::IVulkanBridge {

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

  VkSurfaceKHR Surface() { return m_surface; }
  
  vulkan::CommandPool& CommandPool() { return m_command_pool; }
  vulkan::DeviceQueue& DeviceQueue() { return m_device_queue; }

  bool create_commandbuffer(pfnCommandBufferRenderCallback callback, void* user_data);
  void VulkanCopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

public:
  VkDevice GetVkDevice() override;
  VkPhysicalDevice GetVkPhysicalDevice() override;
  VkRenderPass GetVkRenderPass() override;
  VkCommandPool GetVkCommandPool() override;
  VkPipeline GetVkPipeline() override;
  vulkan::CommandPool& GetCommandPool() override;
  vulkan::SwapChain& GetSwapChain() override;
  int GetGraphicsQueueFamily() override;
  int GetPresentQueueFamily() override;

private:
  bool create_vulkan_surface();

  bool draw_frame();
  void draw_frame_wait_for_previous_frame_to_finish();
  void draw_frame_acquire_image_from_swap_chain();
  bool draw_frame_record_command_buffer();
  void draw_frame_submit_command_buffer();
  void draw_frame_present_swap_chain();

private:
  void *m_hWnd;

  int m_width = 0;
  int m_height = 0;
  GpuLayer *m_pRootTexture;

  // 绑定到一个native窗口。
  VkSurfaceKHR m_surface;
  
  vulkan::DeviceQueue m_device_queue;
  vulkan::SwapChain m_swapchain;
  vulkan::Pipeline m_pipeline;

  vulkan::CommandPool m_command_pool;
  std::vector<VkCommandBuffer> m_command_buffers;

  // std::vector<VkFence> m_inFlightFences;
  // std::vector<VkFence> m_imagesInFlight;
  // size_t m_currentFrame = 0;
};

} // namespace ui
#endif