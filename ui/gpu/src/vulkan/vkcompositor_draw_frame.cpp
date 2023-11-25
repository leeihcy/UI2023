#include "src/vulkan/wrap/vulkan_swap_chain_image.h"
#include "vkcompositor.h"
#include "vulkan/vulkan_core.h"
#include <_types/_uint32_t.h>

namespace ui {

void VulkanCompositor::draw_frame_wait_for_previous_frame_to_finish() {
  vulkan::SyncItem *sync = m_swapchain.GetCurrentSync();

  vkWaitForFences(GetVkDevice(), 1, &sync->m_fence, VK_TRUE, UINT64_MAX);
}

void VulkanCompositor::draw_frame_acquire_image_from_swap_chain() {
  vulkan::SyncItem *sync = m_swapchain.GetCurrentSync();

  // 1. 设置semaphore，用于同步后续的submit操作。
  // 2. 在没有其他操作的情况下，
  // 一直调用acquire的话，是按顺序逐个返回image，如[0,1,2,0,1,2,0...]

  uint32_t imageIndex;
  vkAcquireNextImageKHR(m_device_queue.Device(), m_swapchain.handle(),
                        UINT64_MAX, sync->m_acquire_submit_semaphore,
                        VK_NULL_HANDLE, &imageIndex);

  m_swapchain.SetCurrentImageIndex(imageIndex);

  vkResetFences(m_device_queue.Device(), 1, &sync->m_fence);
}

vulkan::CommandBuffer* VulkanCompositor::draw_frame_begin_record_command_buffer() {
  vulkan::SyncItem *sync = m_swapchain.GetCurrentSync();
  vulkan::SwapChainImage *image = m_swapchain.GetCurrentImage();

  sync->m_command_buffer->Reset();
  sync->m_command_buffer->Begin();

  sync->m_command_buffer->BeginRenderPass(image->m_frame_buffer);
  sync->m_command_buffer->BindPipeline();

  VkCommandBuffer command_buffer = sync->m_command_buffer->handle();
  // callback(sync->m_command_buffer->handle(), user_data);
  {
    // VkViewport viewport{};
    // viewport.x = 0.0f;
    // viewport.y = 0.0f;
    // viewport.width = (float)swapChainExtent.width;
    // viewport.height = (float)swapChainExtent.height;
    // viewport.minDepth = 0.0f;
    // viewport.maxDepth = 1.0f;
    // vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // VkRect2D scissor{};
    // scissor.offset = {0, 0};
    // scissor.extent = swapChainExtent;
    // vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // VkBuffer vertexBuffers[] = {vertexBuffer};
    // VkDeviceSize offsets[] = {0};
    // vkCmdBindVertexBuffers(command_buffer, 0, 1, vertexBuffers, offsets);

    // vkCmdBindIndexBuffer(command_buffer, indexBuffer, 0,
    // VK_INDEX_TYPE_UINT16);

    // vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
    //                         pipelineLayout, 0, 1,
    //                         &descriptorSets[currentFrame], 0, nullptr);

    // vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(indices.size()),
    // 1, 0,
    //                  0, 0);

    // ----
    // vkCmdDraw(command_buffer, 3, 1, 0, 0);
  }

  vkCmdBindDescriptorSets(
      sync->m_command_buffer->handle(), VK_PIPELINE_BIND_POINT_GRAPHICS,
      m_pipeline.layout(), 0, 1,
      &m_pipeline.descriptor_set(m_swapchain.GetCurrentImageIndex()), 0, nullptr);

  m_pipeline.UpdateUniformBuffer(m_swapchain.GetCurrentImageIndex());

  return sync->m_command_buffer.get();
}

void VulkanCompositor::draw_frame_end_record_command_buffer(
    vulkan::CommandBuffer *command_buffer) {
  command_buffer->EndRenderPass();
  command_buffer->End();
}

void VulkanCompositor::draw_frame_submit_command_buffer() {
  vulkan::SyncItem *sync = m_swapchain.GetCurrentSync();

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  // 在获取到图像缓存后才允许提交命令操作
  VkSemaphore waitSemaphores[] = {sync->m_acquire_submit_semaphore};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  VkCommandBuffer command_buffers[] = {sync->m_command_buffer->handle()};
  submitInfo.commandBufferCount = std::size(command_buffers);
  submitInfo.pCommandBuffers = command_buffers;

  // submit完成之后，才允许present。用于同步present操作。
  VkSemaphore signalSemaphores[] = {sync->m_submit_present_semaphore};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(m_device_queue.GraphicsQueue(), 1, &submitInfo,
                    sync->m_fence) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }
}
void VulkanCompositor::draw_frame_present_swap_chain() {
  vulkan::SyncItem *sync = m_swapchain.GetCurrentSync();

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  // 在submit完成之后，才允许present
  VkSemaphore signalSemaphores[] = {sync->m_submit_present_semaphore};
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  // 设置要present哪一张图片
  VkSwapchainKHR swapChains[] = {m_swapchain.handle()};
  uint32_t indices[] = {m_swapchain.GetCurrentImageIndex()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = indices;

  // 只有present一个新image时，才会释放上一个image给vkAcquireNextImageKHR用。
  // 因为gpu需要一直需要用一个image来进行渲染，直到提供了新的。
  vkQueuePresentKHR(m_device_queue.PresentQueue(), &presentInfo);

  // ??
  m_swapchain.IncCurrentFrame();
}

} // namespace ui