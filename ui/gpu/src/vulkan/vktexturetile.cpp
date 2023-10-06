#include "vktexturetile.h"
#include "include/api.h"
#include "vkcompositor.h"
#include "vulkan/vulkan_core.h"

namespace ui {

VkTextureTile::VkTextureTile(VulkanCompositor &compositor)
    : m_compositor(compositor) {}

VkTextureTile::~VkTextureTile() {
  auto device = m_compositor.GetVulkanDevice();

  vkDestroySampler(device, m_texture_sampler, nullptr);
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

  VkDevice device = m_compositor.GetVulkanDevice();

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

  if (source.hasAlphaChannel) {
    int w = rcSrc.right - rcSrc.left;

    // 不要使用for{for循环，效率太低。
    for (int row = rcSrc.top; row < rcSrc.bottom; row++) {
      memcpy(pTexels, pSrcBits + (rcSrc.left * 4), w * 4);

      /*
      int dstcol = 0;
      for (int col = rcSrc.left; col < rcSrc.right; col++, dstcol++)
      {
              pTexels[dstcol*4 + 0] = pSrcBits[col*4 + 2]; // Red
              pTexels[dstcol*4 + 1] = pSrcBits[col*4 + 1]; // Green
              pTexels[dstcol*4 + 2] = pSrcBits[col*4 + 0]; // Blue
              pTexels[dstcol*4 + 3] = pSrcBits[col*4 + 3]; // Alpha
      }
      */

      pSrcBits += source.pitch;
      pTexels += TILE_SIZE * 4;
    }
  } else {
    for (int row = rcSrc.top; row < rcSrc.bottom; row++) {
      int dstcol = 0;
      for (int col = rcSrc.left; col < rcSrc.right; col++, dstcol++) {
        pTexels[dstcol * 4 + 0] = pSrcBits[col * 4 + 2]; // Red
        pTexels[dstcol * 4 + 1] = pSrcBits[col * 4 + 1]; // Green
        pTexels[dstcol * 4 + 2] = pSrcBits[col * 4 + 0]; // Blue
        pTexels[dstcol * 4 + 3] = 255;
      }

      pSrcBits += source.pitch;
      pTexels += TILE_SIZE * 4;
    }
  }

  vkUnmapMemory(device, stagingBufferMemory);

  transitionImageLayout(m_texture_image, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  copyBufferToImage(stagingBuffer, m_texture_image,
                    static_cast<uint32_t>(TILE_SIZE),
                    static_cast<uint32_t>(TILE_SIZE));
  transitionImageLayout(m_texture_image, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);
}

bool VkTextureTile::create() {

  create_texture_image(
      TILE_SIZE, TILE_SIZE, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  create_texture_imageview();
  create_texture_sampler();
  return true;
}

bool VkTextureTile::findMemoryType(uint32_t typeFilter,
                                   VkMemoryPropertyFlags properties,
                                   uint32_t *out) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_compositor.GetVulkanPhysicalDevice(),
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
  VkCommandBuffer commandBuffer = m_compositor.BeginSingleTimeCommands();

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

  vkCmdCopyBufferToImage(commandBuffer, buffer, image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  m_compositor.EndSingleTimeCommands(commandBuffer);
}

void VkTextureTile::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                               VkDeviceSize size) {
  VkCommandBuffer commandBuffer = m_compositor.BeginSingleTimeCommands();

  VkBufferCopy copyRegion{};
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  m_compositor.EndSingleTimeCommands(commandBuffer);
}

bool VkTextureTile::transitionImageLayout(VkImage image, VkFormat format,
                                          VkImageLayout oldLayout,
                                          VkImageLayout newLayout) {
  VkCommandBuffer commandBuffer = m_compositor.BeginSingleTimeCommands();

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

  vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);

  m_compositor.EndSingleTimeCommands(commandBuffer);
  return true;
}

bool VkTextureTile::create_texture_image(uint32_t width, uint32_t height,
                                        VkFormat format, VkImageTiling tiling,
                                        VkImageUsageFlags usage,
                                        VkMemoryPropertyFlags properties) {
  VkDevice device = m_compositor.GetVulkanDevice();

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

  VkDevice device = m_compositor.GetVulkanDevice();

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
  viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  if (vkCreateImageView(m_compositor.GetVulkanDevice(), &viewInfo, nullptr,
                        &m_texture_imageview) != VK_SUCCESS) {
    return false;
  }
  return true;
}

bool VkTextureTile::create_texture_sampler() {
  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(m_compositor.GetVulkanPhysicalDevice(),
                                &properties);

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  // samplerInfo.anisotropyEnable = VK_TRUE;
  // samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  samplerInfo.anisotropyEnable = VK_FALSE;
  samplerInfo.maxAnisotropy = 1.0f;

  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  if (vkCreateSampler(m_compositor.GetVulkanDevice(), &samplerInfo, nullptr,
                      &m_texture_sampler) != VK_SUCCESS) {
    return false;
  }
  return true;
}

} // namespace ui