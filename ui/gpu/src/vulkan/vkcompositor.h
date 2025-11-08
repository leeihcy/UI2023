#ifndef _UI_GPU_SRC_VULKAN_GPU_COMPOSITOR_H_
#define _UI_GPU_SRC_VULKAN_GPU_COMPOSITOR_H_
#include "gpu/include/api.h"
#include "gpu/src/gpu_layer.h"
#include "gpu/src/vulkan/vkapp.h"
#include "src/vulkan/vkswapchain.h"
#include "src/vulkan/vkbridge.h"
#include "src/vulkan/vulkan_device_queue.h"
#include "src/vulkan/vulkan_pipe_line.h"


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
  VkCommandBuffer GetCurrentCommandBuffer() override;
  vulkan::CommandPool &GetCommandPool() override;
  vulkan::DeviceQueue &GetDeviceQueue() override;
  vulkan::SwapChain &GetSwapChain() override;
  vulkan::Pipeline& GetPipeline() override;
  int GetGraphicsQueueFamily() override;
  int GetPresentQueueFamily() override;

private:
  void destory();
  
  bool createVulkanSurface(IGpuCompositorWindow* window);

  void drawFrame_acquireNextCommandBuffer();
  void drawFrame_acquireNextSwapChainImage();
  void drawFrame_beginRecordCommandBuffer();
  void drawFrame_endRecordCommandBuffer(vulkan::CommandBuffer *);
  void drawFrame_submitCommandBuffer();
  void drawFrame_presentSwapChain();

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