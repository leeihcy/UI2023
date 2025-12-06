#include "vk_swapchain_frame.h"

#include <vulkan/vulkan_core.h>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "src/vulkan/shaders/shader_types.h"
#include "src/util.h"
#include "src/vulkan/vk_pipeline.h"
#include "src/vulkan/vk_swapchain.h"
#include "src/vulkan/vk_compositor.h"

namespace vulkan {

SwapChainFrame::SwapChainFrame(IVulkanBridge &bridge, VkImage swapchain_image)
    : m_bridge(bridge), m_swapchain_image_ref(swapchain_image),
    m_framedata_buffer(bridge) {}

SwapChainFrame::~SwapChainFrame() {
  VkDevice device = m_bridge.GetVkDevice();

  m_image_view.Destroy(device);
  m_frame_buffer.Destroy(device);

  if (m_frame_descriptorset != VK_NULL_HANDLE) {
    m_bridge.GetUniformDescriptorPool().FreeDescriptorSet(device, m_frame_descriptorset);
    m_frame_descriptorset = VK_NULL_HANDLE;
  }
  m_framedata_buffer.Destroy(true);
}

bool SwapChainFrame::Create(VkFormat imageFormat) {
  if (!m_image_view.Create(m_bridge.GetVkDevice(), m_swapchain_image_ref, imageFormat)) {
    return false;
  }
  return true;
}

bool SwapChainFrame::CreateFrameDataUniformBuffer() {
  m_frame_descriptorset = m_bridge.GetUniformDescriptorPool().AllocatateDescriptorSet(
    m_bridge.GetVkDevice(), m_bridge.GetPipeline().GetUniformLayout());

  VkDeviceSize bufferSize = sizeof(FrameData);

  return m_framedata_buffer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

bool SwapChainFrame::CreateFrameBuffer(int width, int height,
                                       VkRenderPass render_pass) {
  VkImageView attachments[] = {m_image_view.handle};

  VkFramebufferCreateInfo framebufferInfo{};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

  // 这里的render_pass并不是要绑定这个render_pass，
  // 只是用这个render pass来验证Framebuffer中的attachment是否和render
  // pass中的attachment兼容。
  framebufferInfo.renderPass = render_pass;

  framebufferInfo.attachmentCount = 1;
  framebufferInfo.pAttachments = attachments;
  framebufferInfo.width = width;
  framebufferInfo.height = height;
  framebufferInfo.layers = 1;

  if (vkCreateFramebuffer(m_bridge.GetVkDevice(), &framebufferInfo, nullptr,
                          &m_frame_buffer) != VK_SUCCESS) {
    return false;
  }
  return true;
}

void SwapChainFrame::UpdateUniformBuffer(uint32_t currentImage,
                                   VkCommandBuffer command_buffer) {
  vulkan::SwapChain &swapchain = m_bridge.GetSwapChain();

  FrameData ubo{};
  ubo.view = glm::mat4(1.0f);
  ubo.ortho = glm::orthoLH<float>(0, (float)swapchain.Extent2D().width, 0,
                          (float)swapchain.Extent2D().height, -2000.f, 2000.f);
  void *data;
  vkMapMemory(m_bridge.GetVkDevice(),
              m_framedata_buffer.memory(), 0, sizeof(ubo), 0,
              &data);
  memcpy(data, &ubo, sizeof(ubo));
  vkUnmapMemory(m_bridge.GetVkDevice(),
                m_framedata_buffer.memory());

  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = m_framedata_buffer.handle();
  bufferInfo.range = sizeof(FrameData);

  VkWriteDescriptorSet descriptorWrites[1] = {};
  descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrites[0].dstSet = m_frame_descriptorset;
  descriptorWrites[0].dstBinding = 0;
  descriptorWrites[0].dstArrayElement = 0;
  descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrites[0].descriptorCount = 1;
  descriptorWrites[0].pBufferInfo = &bufferInfo;
  vkUpdateDescriptorSets(m_bridge.GetVkDevice(), std::size(descriptorWrites),
                         descriptorWrites, 0, nullptr);

  vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          m_bridge.GetPipeline().layout(), 0, 1, &m_frame_descriptorset,
                          0, nullptr);
}

void SwapChainFrame::UpdatePushData(VkCommandBuffer &command_buffer,
                              glm::mat4 &mat4) {
  LayerData position;
  position.model = mat4;

  vkCmdPushConstants(
      command_buffer,             // 当前命令缓冲区
      m_bridge.GetPipeline().layout(),          // 管线布局（需包含 Push Constants 范围）
      VK_SHADER_STAGE_VERTEX_BIT, // 生效的着色器阶段（这里是顶点着色器）
      0,                          // 偏移量（如果 Push Constants 块有多个变量）
      sizeof(LayerData),           // 数据大小
      &position                   // 数据指针
  );
}

} // namespace vulkan