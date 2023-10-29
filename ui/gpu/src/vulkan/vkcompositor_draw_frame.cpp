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

  uint32_t imageIndex;
  vkAcquireNextImageKHR(m_device_queue.Device(), m_swapchain.handle(),
                        UINT64_MAX, sync->m_acquire_semaphore, VK_NULL_HANDLE,
                        &imageIndex);

  m_swapchain.SetCurrentImageIndex(imageIndex);

  // if (m_imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
  //   vkWaitForFences(m_device_queue.Device(), 1,
  //   &m_imagesInFlight[imageIndex], VK_TRUE,
  //                   UINT64_MAX);
  // }
  // m_imagesInFlight[imageIndex] = m_inFlightFences[m_currentFrame];

  vkResetFences(m_device_queue.Device(), 1, &sync->m_fence);
}

bool VulkanCompositor::create_commandbuffer(
    pfnCommandBufferRenderCallback callback, void *user_data) {

  //  vulkan::SyncItem *sync = m_swapchain.GetCurrentSync();
  //   vulkan::SwapChainImage* image = m_swapchain.GetCurrentImage();

  //   sync->m_command_buffer->Reset();
  //   sync->m_command_buffer->Begin();

  //   sync->m_command_buffer->BeginRenderPass(image->m_frame_buffer);
  //   sync->m_command_buffer->BindPipeline();

  //   // callback(sync->m_command_buffer->handle(), user_data);

  //   sync->m_command_buffer->EndRenderPass();
  //   sync->m_command_buffer->End();
  return true;
}

bool VulkanCompositor::draw_frame_record_command_buffer() {
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

    // vkCmdBindIndexBuffer(command_buffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    // vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
    //                         pipelineLayout, 0, 1, &descriptorSets[currentFrame],
    //                         0, nullptr);

    // vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(indices.size()), 1, 0,
    //                  0, 0);

    vkCmdDraw(command_buffer, 3, 1, 0, 0);
  }

  sync->m_command_buffer->EndRenderPass();
  sync->m_command_buffer->End();
  return true;
}

void VulkanCompositor::draw_frame_submit_command_buffer() {
  vulkan::SyncItem *sync = m_swapchain.GetCurrentSync();

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {sync->m_acquire_semaphore};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  VkCommandBuffer command_buffers[] = {sync->m_command_buffer->handle()};
  submitInfo.commandBufferCount = std::size(command_buffers);
  submitInfo.pCommandBuffers = command_buffers;

  VkSemaphore signalSemaphores[] = {sync->m_present_semaphore};
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

  VkSemaphore signalSemaphores[] = {sync->m_present_semaphore};

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {m_swapchain.handle()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  uint32_t image_index = m_swapchain.GetCurrentImageIndex();
  presentInfo.pImageIndices = &image_index;

  vkQueuePresentKHR(m_device_queue.PresentQueue(), &presentInfo);

  m_swapchain.IncCurrentFrame();
}

} // namespace ui