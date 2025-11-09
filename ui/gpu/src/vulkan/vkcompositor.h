#ifndef _UI_GPU_SRC_VULKAN_GPU_COMPOSITOR_H_
#define _UI_GPU_SRC_VULKAN_GPU_COMPOSITOR_H_
#include "include/api.h"
#include "src/gpu_layer.h"
#include "src/vulkan/vkapp.h"
#include "src/vulkan/vkobjects.h"
#include "src/vulkan/vkswapchain.h"
#include "src/vulkan/vkbridge.h"
#include "src/vulkan/vulkan_device_queue.h"
#include "src/vulkan/vkpipeline.h"


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
  VkSurfaceKHR GetSurface() { return m_surface; }

  std::shared_ptr<IGpuLayer> CreateLayerTexture() override;
  bool BeginCommit(GpuLayerCommitContext *) override;
  void EndCommit(GpuLayerCommitContext *) override;
  void Resize(int nWidth, int nHeight) override;

public:
  VkDevice GetVkDevice() override;
  VkPhysicalDevice GetVkPhysicalDevice() override;
  VkRenderPass GetVkRenderPass() override { return m_renderpass; }
  VkCommandPool GetVkCommandPool() override { return m_command_pool; }
  VkPipeline GetVkPipeline() override { return m_pipeline.handle(); }
  VkCommandBuffer GetCurrentCommandBuffer() override;
  Vk::CommandPool &GetCommandPool() override { return m_command_pool; }
  Vk::DescriptorPool& GetUniformDescriptorPool() override { return m_uniform_descriptor_pool; }
  Vk::DescriptorPool& GetTextureDescriptorPool() override { return m_texture_descriptor_pool; }

  vulkan::DeviceQueue &GetDeviceQueue() override { return m_device_queue; }
  vulkan::SwapChain &GetSwapChain() override { return m_swapchain; }
  vulkan::PipeLine& GetPipeline() override { return m_pipeline; }
  int GetGraphicsQueueFamily() override;
  int GetPresentQueueFamily() override;
  
  void OnSwapChainCreated() override;

private:
  void destory();
  
  bool createVulkanSurface(IGpuCompositorWindow* window);
  bool createUniformDescriptorPool();
  bool createTextureDescriptorPool();

  void drawFrame_acquireNextCommandBuffer();
  bool drawFrame_acquireNextSwapChainImage();
  void drawFrame_beginRecordCommandBuffer();
  void drawFrame_endRecordCommandBuffer();
  void drawFrame_submitCommandBuffer();
  void drawFrame_presentSwapChain();

private:
  int m_width = 0;
  int m_height = 0;

  vulkan::DeviceQueue m_device_queue;
  
  VkSurfaceKHR m_surface;
  Vk::RenderPass m_renderpass;

  vulkan::SwapChain m_swapchain;
  vulkan::PipeLine m_pipeline;

  Vk::CommandPool m_command_pool;
  Vk::DescriptorPool m_uniform_descriptor_pool;
  Vk::DescriptorPool m_texture_descriptor_pool;
};

} // namespace ui
#endif