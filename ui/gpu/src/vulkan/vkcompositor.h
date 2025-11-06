#ifndef _UI_GPU_SRC_VULKAN_GPU_COMPOSITOR_H_
#define _UI_GPU_SRC_VULKAN_GPU_COMPOSITOR_H_
#include "gpu/include/api.h"
#include "gpu/src/gpu_layer.h"
#include "gpu/src/vulkan/vkapp.h"
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_command_pool.h"
#include "src/vulkan/wrap/vulkan_device_queue.h"
#include "src/vulkan/wrap/vulkan_pipe_line.h"
#include "src/vulkan/wrap/vulkan_renderpass.h"
#include "src/vulkan/wrap/vulkan_swap_chain.h"

#if defined(OS_WIN)
#include <Windows.h>
#endif

#include <vector>
#include <vulkan/vulkan.h>

#if defined(OS_MAC)
#define VK_USE_PLATFORM_MACOS_MVK
// #define VK_USE_PLATFORM_METAL_EXT
#endif

namespace ui {

// 一个compositor对应一个窗口
class VulkanCompositor : public IGpuCompositor, public vulkan::IVulkanBridge {

public:
  VulkanCompositor();
  ~VulkanCompositor();

  bool Initialize(IGpuCompositorWindow*);

  std::shared_ptr<IGpuLayer> CreateLayerTexture() override;
  bool BeginCommit(GpuLayerCommitContext *) override;
  void EndCommit(GpuLayerCommitContext *) override;
  void Resize(int nWidth, int nHeight) override;

  GpuLayer *GetRootLayerTexture();

  VkSurfaceKHR Surface() { return m_surface; }

  void VulkanCopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                        VkDeviceSize size);

public:
  VkDevice GetVkDevice() override;
  VkPhysicalDevice GetVkPhysicalDevice() override;
  VkRenderPass GetVkRenderPass() override;
  VkCommandPool GetVkCommandPool() override;
  VkPipeline GetVkPipeline() override;
  vulkan::CommandPool &GetCommandPool() override;
  vulkan::DeviceQueue &GetDeviceQueue() override;
  vulkan::SwapChain &GetSwapChain() override;
  vulkan::Pipeline& GetPipeline() override;
  int GetGraphicsQueueFamily() override;
  int GetPresentQueueFamily() override;

private:
  void destory();
  
  bool create_vulkan_surface(IGpuCompositorWindow* window);

  void drawFrame_waitForCommandBufferIdle();
  void drawFrame_acquireImageFromSwapChain();
  vulkan::CommandBuffer *drawFrame_beginRecordCommandBuffer();
  void draw_frame_end_record_command_buffer(vulkan::CommandBuffer *);
  void draw_frame_submit_command_buffer();
  void draw_frame_present_swap_chain();

private:
  int m_width = 0;
  int m_height = 0;

  vulkan::DeviceQueue m_device_queue;
  VkSurfaceKHR m_surface;
  
  vulkan::SwapChain m_swapchain;
  vulkan::RenderPass m_renderpass;
  vulkan::Pipeline m_pipeline;
  vulkan::CommandPool m_command_pool;
};

} // namespace ui
#endif