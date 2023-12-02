#include "vktexturetile.h"
#include "include/api.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include "vkcompositor.h"
#include "vulkan/vulkan_core.h"


namespace ui {

VkTextureTile::VkTextureTile(vulkan::IVulkanBridge& bridge)
    : m_bridge(bridge) {}

VkTextureTile::~VkTextureTile() {
  auto device = m_bridge.GetVkDevice();

  vkDestroyImageView(device, m_texture_imageview, nullptr);
  vkDestroyImage(device, m_texture_image, nullptr);
  vkFreeMemory(device, m_texture_image_memory, nullptr);

}

void VkTextureTile::Upload(ui::Rect &rcSrc, ui::UploadGpuBitmapInfo &source) {
  if (m_texture_image == VK_NULL_HANDLE) {
    create();
  }

  VkDeviceSize imageSize = TILE_SIZE * TILE_SIZE * 4;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  create_vulkan_buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       stagingBuffer, stagingBufferMemory);

  VkDevice device = m_bridge.GetVkDevice();

  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
  // memcpy(data, pixels, static_cast<size_t>(imageSize));

  // 如果本次内容没有填充满Tile，则需要将空白处清0，以防上一次的脏数据干扰
  int w = rcSrc.right - rcSrc.left;
  int h = rcSrc.bottom - rcSrc.top;
  if (w != TILE_SIZE || h != TILE_SIZE) {
    memset(data, 0, imageSize);
  }

  unsigned char *pSrcBits = (unsigned char *)source.pFirstLineBits;
  unsigned char *pTexels = (unsigned char *)data;

  pSrcBits += rcSrc.top * source.pitch;

  // 创建的VK_FORMAT_B8G8R8A8_SRGB格式image view
  if (source.hasAlphaChannel) {
    int w = rcSrc.right - rcSrc.left;

    // 不要使用for{for循环，效率太低。
    for (int row = rcSrc.top; row < rcSrc.bottom; row++) {
      memcpy(pTexels, pSrcBits + (rcSrc.left * 4), w * 4);
      pSrcBits += source.pitch;
      pTexels += TILE_SIZE * 4;
    }
  } else {
    for (int row = rcSrc.top; row < rcSrc.bottom; row++) {
      int dstcol = 0;
      // 创建的VK_FORMAT_B8G8R8A8_SRGB格式image view
      for (int col = rcSrc.left; col < rcSrc.right; col++, dstcol++) {
        pTexels[dstcol * 4 + 0] = pSrcBits[col * 4 + 0]; // Blue
        pTexels[dstcol * 4 + 1] = pSrcBits[col * 4 + 1]; // Green
        pTexels[dstcol * 4 + 2] = pSrcBits[col * 4 + 2]; // Red
        pTexels[dstcol * 4 + 3] = 255;
      }

      pSrcBits += source.pitch;
      pTexels += TILE_SIZE * 4;
    }
  }

  vkUnmapMemory(device, stagingBufferMemory);

  transitionImageLayout(m_texture_image, VK_FORMAT_B8G8R8A8_SRGB,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  copyBufferToImage(stagingBuffer, m_texture_image,
                    static_cast<uint32_t>(TILE_SIZE),
                    static_cast<uint32_t>(TILE_SIZE));
  transitionImageLayout(m_texture_image, VK_FORMAT_B8G8R8A8_SRGB,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);

  if (m_texture_descriptorset == VK_NULL_HANDLE) {
    VkDescriptorSetAllocateInfo textureAllocInfo = {};
    textureAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    textureAllocInfo.descriptorPool = m_bridge.GetPipeline().texture_descriptor_pool();
    
    VkDescriptorSetLayout layouts[1] = {m_bridge.GetPipeline().texture_descriptor_set_layout()};
    textureAllocInfo.descriptorSetCount = std::size(layouts);
    textureAllocInfo.pSetLayouts = layouts;

    if (vkAllocateDescriptorSets(device, &textureAllocInfo,
                                 &m_texture_descriptorset) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate descriptor sets");
    }
  }
  VkDescriptorImageInfo imageInfo = {};
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  imageInfo.imageView = m_texture_imageview;
  imageInfo.sampler = m_bridge.GetPipeline().texture_sampler();

  VkWriteDescriptorSet texturedescriptorWrites = {};
  texturedescriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  texturedescriptorWrites.dstSet = m_texture_descriptorset;
  texturedescriptorWrites.dstBinding = 0;
  texturedescriptorWrites.dstArrayElement = 0;
  texturedescriptorWrites.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  texturedescriptorWrites.descriptorCount = 1;
  texturedescriptorWrites.pImageInfo = &imageInfo;
  texturedescriptorWrites.pTexelBufferView = nullptr;

  vkUpdateDescriptorSets(device, 1, &texturedescriptorWrites, 0, nullptr);
}

void VkTextureTile::Compositor(long xOffset, long yOffset, long vertexStartIndex,
                  ui::GpuLayerCommitContext *pContext)
{
  vulkan::CommandBuffer *command_buffer =
      (vulkan::CommandBuffer *)pContext->m_data;
  VkCommandBuffer buffer = command_buffer->handle();

  // VkBuffer vertexBuffers[] = {m_vertexBuffer.handle()};
  // VkDeviceSize offsets[] = {0};

  // vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
  //   vkCmdBindIndexBuffer(buffer, m_indexBuffer.handle(), 0, VK_INDEX_TYPE_UINT16);

    VkDescriptorSet sets[2] = { m_bridge.GetPipeline().descriptor_set(0), m_texture_descriptorset };
    vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_bridge.GetPipeline().layout(), 0, 2,
                            sets, 0, nullptr);

  // m_bridge.GetPipeline().UpdateDescriptorSets(1, &m_texture_imageview);

    vkCmdDrawIndexed(buffer,
                     4, // indexCount
                     1, // instanceCount
                     0, // firstIndex
                     vertexStartIndex, // vertexOffset
                     0  // firstInstance
    );
    // vkCmdDraw(command, count, 1, 0, 0);
}

bool VkTextureTile::create() {

  create_texture_image(
      TILE_SIZE, TILE_SIZE, VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  create_texture_imageview();
  return true;
}

bool VkTextureTile::findMemoryType(uint32_t typeFilter,
                                   VkMemoryPropertyFlags properties,
                                   uint32_t *out) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_bridge.GetDeviceQueue().PhysicalDevice(),
                                      &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      *out = i;
      return true;
    }
  }

  return false;
}

void VkTextureTile::copyBufferToImage(VkBuffer buffer, VkImage image,
                                      uint32_t width, uint32_t height) {
  std::unique_ptr<vulkan::CommandBuffer> cb = m_bridge.GetCommandPool().CreateBuffer();
  cb->Begin(); 

  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width, height, 1};

  vkCmdCopyBufferToImage(cb->handle(), buffer, image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  cb->End();
  m_bridge.GetDeviceQueue().Submit(cb.get());
  m_bridge.GetDeviceQueue().WaitIdle();
  cb->Destroy();
}

void VkTextureTile::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                               VkDeviceSize size) {
  std::unique_ptr<vulkan::CommandBuffer> cb = m_bridge.GetCommandPool().CreateBuffer();
  cb->Begin(); 

  VkBufferCopy copyRegion{};
  copyRegion.size = size;
  vkCmdCopyBuffer(cb->handle(), srcBuffer, dstBuffer, 1, &copyRegion);

  cb->End();
  m_bridge.GetDeviceQueue().Submit(cb.get());
  m_bridge.GetDeviceQueue().WaitIdle();
  cb->Destroy();
}

bool VkTextureTile::transitionImageLayout(VkImage image, VkFormat format,
                                          VkImageLayout oldLayout,
                                          VkImageLayout newLayout) {
  std::unique_ptr<vulkan::CommandBuffer> cb = m_bridge.GetCommandPool().CreateBuffer();
  cb->Begin(); 
  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
      newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    return false;
  }

  vkCmdPipelineBarrier(cb->handle(), sourceStage, destinationStage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);

  cb->End();
 
  m_bridge.GetDeviceQueue().Submit(cb.get());
  m_bridge.GetDeviceQueue().WaitIdle();
  cb->Destroy();

  return true;
}

bool VkTextureTile::create_texture_image(uint32_t width, uint32_t height,
                                        VkFormat format, VkImageTiling tiling,
                                        VkImageUsageFlags usage,
                                        VkMemoryPropertyFlags properties) {
  VkDevice device = m_bridge.GetVkDevice();

  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = format;
  imageInfo.tiling = tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = usage;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(device, &imageInfo, nullptr, &m_texture_image) !=
      VK_SUCCESS) {
    return false;
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device, m_texture_image, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  if (!findMemoryType(memRequirements.memoryTypeBits, properties,
                      &allocInfo.memoryTypeIndex)) {
    return false;
  }

  if (vkAllocateMemory(device, &allocInfo, nullptr, &m_texture_image_memory) !=
      VK_SUCCESS) {
    return false;
  }

  vkBindImageMemory(device, m_texture_image, m_texture_image_memory, 0);
  return true;
}

bool VkTextureTile::create_vulkan_buffer(VkDeviceSize size,
                                         VkBufferUsageFlags usage,
                                         VkMemoryPropertyFlags properties,
                                         VkBuffer &buffer,
                                         VkDeviceMemory &bufferMemory) {

  VkDevice device = m_bridge.GetVkDevice();

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    return false;
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  if (!findMemoryType(memRequirements.memoryTypeBits, properties,
                      &allocInfo.memoryTypeIndex)) {
    return false;
  }

  if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) !=
      VK_SUCCESS) {
    return false;
  }

  vkBindBufferMemory(device, buffer, bufferMemory, 0);
  return true;
}

bool VkTextureTile::create_texture_imageview() {
  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = m_texture_image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  if (vkCreateImageView(m_bridge.GetVkDevice(), &viewInfo, nullptr,
                        &m_texture_imageview) != VK_SUCCESS) {
    return false;
  }
  return true;
}

} // namespace ui