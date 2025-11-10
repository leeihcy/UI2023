#include "src/vulkan/vkcompositor.h"
#include "src/vulkan/vulkan_swap_chain_image.h"
#include "src/util.h"
#include "vulkan/vulkan_core.h"


namespace ui {

// 1. Wait for the previous frame to finish
// 2. Acquire an image from the swap chain
// 3. Record a command buffer which draws the scene onto that image
// 4. Submit the recorded command buffer
// 5. Present the swap chain image
//
bool VulkanCompositor::BeginCommit(GpuLayerCommitContext *ctx) {
  if (m_swapchain.NeedReCreated()) {
    m_swapchain.Create(GetSurface(), m_width, m_height);
  }

  if (!drawFrame_acquireNextCommandBuffer()) {
    return false;
  }
  if (!drawFrame_acquireNextSwapChainImage()) {
    m_swapchain.MarkNeedReCreate();
    return false;
  }

  drawFrame_beginRecordCommandBuffer();
  return true;
}

void VulkanCompositor::EndCommit(GpuLayerCommitContext *ctx) {
  drawFrame_endRecordCommandBuffer();
  drawFrame_submitCommandBuffer();
  drawFrame_presentSwapChain();

  // 当前frame完成，准备next frame
  m_swapchain.IncCurrentInflightFrame();
  m_swapchain.IncCurrentSemaphores();
}

bool VulkanCompositor::drawFrame_acquireNextCommandBuffer() {
  vulkan::InFlightFrame *sync = m_swapchain.GetCurrentInflightFrame();

  // 避免阻塞渲染线程，首先使用0ms进行尝试。
  const int retry_times = 4;
  uint64_t timeout[retry_times] = {
    0,
    10 * 1000 * 1000, // 10ms
    100 * 1000 * 1000, // 100ms
    1000 * 1000 * 1000, // 1m 
  };

  VkResult result = VK_SUCCESS;
  for (int i = 0; i < retry_times; i++) {
    result = vkWaitForFences(GetVkDevice(), 1, &sync->m_command_buffer_fence,
                             VK_TRUE, timeout[i]);
    if (result == VK_TIMEOUT && i != 0) {
      ui::Log("acquireNextCommandBuffer WaitForFences timeout at index: %d", i);
      continue;
    }
  }
  if (result != VK_SUCCESS) {  // VK_ERROR_DEVICE_LOST
    ui::Log("acquireNextCommandBuffer WaitForFences failed, result=%d", result);
    return false;
  }

  // lldb调试时会出现exception，先无视，好像无解，等以后SDK更新吧。
  vkResetFences(m_device_queue.Device(), 1, &sync->m_command_buffer_fence);
  return true;
}

bool VulkanCompositor::drawFrame_acquireNextSwapChainImage() {
  vulkan::InFlightFrame *sync = m_swapchain.GetCurrentInflightFrame();
  vulkan::GpuSemaphores *semaphores = m_swapchain.GetCurrentSemaphores();
  VkDevice device = GetVkDevice();
  // 在没有其他操作的情况下，一直调用acquire的话，是按顺序逐个返回image，如[0,1,2,0,1,2,0...]

  uint32_t imageIndex = 0xFFFFFFFF;
  VkResult result = vkAcquireNextImageKHR(
      device, m_swapchain.handle(),

      // timeout, 无限等待，直到图像可用。
      //
      // 即使 timeout=UINT64_MAX 让 CPU 阻塞到图像“逻辑可用”，
      // 只是交换链的管理模块 （由GPU驱动实现）已将该图像标记为“可被渲染”。
      // 但 GPU 可能仍在执行前一轮对该图像的操作,例如前帧的呈现、后处理等，
      // 这些操作是异步的，CPU 无法直接感知。
      //
      // GPU的后续操作，如vkQueueSubmit，仍需semaphore来保证硬件层面的真正可用性。
      UINT64_MAX,

      // 一个可选的信号量，当图像可用时，该信号量会被触发（signaled）
      // 用于GPU-GPU之间同步，确保渲染操作在图像可用后才执行
      semaphores->m_semaphore_on_image_available,

      // 一个可选的栅栏，当图像可用时，该栅栏会被触发（signaled）。
      // 通常用于 CPU 同步。
      VK_NULL_HANDLE,

      // 获取到的图像索引
      &imageIndex);

  if (imageIndex == 0xFFFFFFFF) {
    ui::Log(
        "vkAcquireNextImageKHR Failed, result=%d: device=%p, m_swapchain=%p",
        result, device, m_swapchain.handle());
    return false;
  }

  m_swapchain.SetCurrentImageIndex(imageIndex);
  return true;
}

void VulkanCompositor::drawFrame_beginRecordCommandBuffer() {
  vulkan::InFlightFrame *sync = m_swapchain.GetCurrentInflightFrame();
  vulkan::SwapChainFrame *frame = m_swapchain.GetCurrentFrame();
  VkCommandBuffer command_buffer_handle = sync->m_command_buffer.handle;

  sync->m_command_buffer.Reset();
  sync->m_command_buffer.BeginRecordCommand();

  VkOffset2D offset = {0, 0};
  VkExtent2D extent = m_swapchain.Extent2D();
  sync->m_command_buffer.BeginRenderPass(frame->m_frame_buffer,
                                          GetVkRenderPass(), offset, extent);
                                          
  sync->m_command_buffer.BindPipeline(GetVkPipeline());

  uint32_t currentImage = m_swapchain.GetCurrentImageIndex();

  m_pipeline.UpdateViewportScissor(m_swapchain.Extent2D().width,
                                   m_swapchain.Extent2D().height,
                                   command_buffer_handle);
  frame->UpdateUniformBuffer(currentImage, command_buffer_handle);
}

void VulkanCompositor::drawFrame_endRecordCommandBuffer() {
  Vk::CommandBuffer& command_buffer = m_swapchain.GetCurrentInflightFrame()->m_command_buffer;

  command_buffer.EndRenderPass();
  command_buffer.EndRecordCommand();
}

void VulkanCompositor::drawFrame_submitCommandBuffer() {
  vulkan::InFlightFrame *sync = m_swapchain.GetCurrentInflightFrame();
  vulkan::GpuSemaphores *semaphores = m_swapchain.GetCurrentSemaphores();

  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
  };
  VkCommandBuffer commandBuffers[] = {
    sync->m_command_buffer.handle
  };

  // 等待 GPU 通知 image_available 之后，GPU 才能进行 vkQueueSubmit 操作
  VkSemaphore waitSemaphores[] = {
    semaphores->m_semaphore_on_image_available
  };

  // GPU vkQueueSubmit 完成后，激活该信号量，用于同步 vkQueuePresentKHR GPU 操作
  VkSemaphore signalSemaphores[] = {
      semaphores->m_semaphore_on_queue_submit_finish
  };

  VkSubmitInfo submitInfo = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                             .pNext = nullptr,

                             .waitSemaphoreCount = 1,
                             .pWaitSemaphores = waitSemaphores,

                             .pWaitDstStageMask = waitStages,

                             .commandBufferCount = 1,
                             .pCommandBuffers = commandBuffers,

                             .signalSemaphoreCount = 1,
                             .pSignalSemaphores = signalSemaphores};

  // 这是一个异步函数。调用会立即返回，CPU可以继续往下执行。后续的逻辑由GPU异步执行。
  if (vkQueueSubmit(m_device_queue.GraphicsQueue(), 1, &submitInfo,

                    // 当 GPU 执行完成 buffer 中所有的指令，已生成最终像素后，fence会被触发（Signaled）。
                    // 用于控制 CPU 最多绘制2帧图片(MAX_FRAMES_IN_FLIGHT)， 避免 CPU 提交过快。
                    sync->m_command_buffer_fence) != VK_SUCCESS) {
    Log("failed to submit draw command buffer!");
  }
}
void VulkanCompositor::drawFrame_presentSwapChain() {
  vulkan::InFlightFrame *sync = m_swapchain.GetCurrentInflightFrame();
  vulkan::GpuSemaphores *semaphores = m_swapchain.GetCurrentSemaphores();

  // 在GPU submit完成之后，才允许present
  // vkQueuePresentKHR调用完成后，会自动将该信号量设置为Unsignaled
  VkSemaphore waitSemaphores[] = {
      semaphores->m_semaphore_on_queue_submit_finish};

  // 设置要present哪一张图片
  VkSwapchainKHR swapChains[] = {m_swapchain.handle()};
  uint32_t indices[] = {m_swapchain.GetCurrentImageIndex()};

  VkPresentInfoKHR presentInfo = {.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,

                                  .waitSemaphoreCount = 1,
                                  .pWaitSemaphores = waitSemaphores,

                                  .swapchainCount = 1,
                                  .pSwapchains = swapChains,
                                  .pImageIndices = indices};

  // 只有present一个新image时，才会释放上一个image给vkAcquireNextImageKHR用。
  // 因为gpu需要一直需要用一个image来进行渲染，直到提供了新的。
  vkQueuePresentKHR(m_device_queue.PresentQueue(), &presentInfo);
}

} // namespace ui