#include "vk_tile.h"
#include "include/api.h"
#include "include/interface.h"
#include "src/vulkan/vk_pipeline.h"
#include "src/vulkan/vk_compositor.h"
#include "src/util.h"
#include <vulkan/vulkan_core.h>
#include <memory.h>

namespace ui {

VkTextureTile::VkTextureTile(vulkan::IVulkanBridge &bridge)
    : m_bridge(bridge) {}

VkTextureTile::~VkTextureTile() {
  auto device = m_bridge.GetVkDevice();
  
  if (m_texture_descriptorset != VK_NULL_HANDLE) {
    m_bridge.GetTextureDescriptorPool().FreeDescriptorSet(device, m_texture_descriptorset);
    m_texture_descriptorset = VK_NULL_HANDLE;
  }

  m_texture_imageview.Destroy(device);
  if (m_texture_image_memory != VK_NULL_HANDLE) {
    vkFreeMemory(device, m_texture_image_memory, nullptr);
    m_texture_image_memory = VK_NULL_HANDLE;
  }
  m_texture_image.Destroy(device);
}

// 使用一个Staging Buffer作为中转站。
// Staging Buffer的设计目标：让CPU和GPU都各司其职，做自己最擅长的事情，以实现最高的数据传输效率。
// 它的工作流程通常涉及两次数据拷贝：
// 1. CPU -> Staging Buffer (慢速)
// 2. Staging Buffer -> GPU (快速，由GPU执行)
//
// Vulkan支持脏区域上传。
//
void VkTextureTile::Upload(ui::Rect &dirty_of_tile, ui::Rect &dirty_of_layer,
                           ui::GpuUploadBitmap &bitmap) {
  if (m_texture_image == VK_NULL_HANDLE) {
    create();
  }

  vulkan::Buffer& staging_buffer = m_bridge.GetTextureTileStagingBuffer();
 
  constexpr int px_size = 4;
  int w = dirty_of_tile.Width();
  int h = dirty_of_tile.Height();

  int buffer_size = w * h * px_size;
  unsigned char *dest_data = (unsigned char*)staging_buffer.MapMemory(0, buffer_size);

  unsigned char* source_data = bitmap.bits + dirty_of_layer.top * bitmap.pitch;
  source_data += dirty_of_layer.left * px_size;

  unsigned int buffer_pitch = w * px_size;
  for (int row = dirty_of_layer.top; row < dirty_of_layer.bottom; row++) {
    memcpy(dest_data, source_data, buffer_pitch);
    source_data += bitmap.pitch;
    dest_data += buffer_pitch;
  }

  staging_buffer.UnmapMemory();

  transitionImageLayout(m_texture_image, VK_FORMAT_B8G8R8A8_SRGB,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  copyBufferToImage(staging_buffer.handle(), m_texture_image,
                    dirty_of_tile.left, dirty_of_tile.top,
                    dirty_of_tile.Width(), dirty_of_tile.Height());
  transitionImageLayout(m_texture_image, VK_FORMAT_B8G8R8A8_SRGB,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

// 每次上传纹理数据时，只是更新了VkImage内容，VkImage对象并没有替换。
// 所以三缓冲理论上都可以共用一个DescriptorSet。
bool VkTextureTile::updateTextureDescriptorset() {
  if (!m_texture_imageview.handle) {
    return false;
  }
  if (m_texture_descriptorset == VK_NULL_HANDLE) {
    m_texture_descriptorset = m_bridge.GetTextureDescriptorPool().AllocatateDescriptorSet(
      m_bridge.GetVkDevice(), m_bridge.GetPipeline().GetTextureDescriptorSetLayout());
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

  vkUpdateDescriptorSets(m_bridge.GetVkDevice(), 1, &texturedescriptorWrites, 0,
                         nullptr);
  return true;
}

void VkTextureTile::Compositor(long, long, long vertex_start_index,
                               ui::GpuLayerCommitContext *pContext) {
  if (m_texture_descriptorset == VK_NULL_HANDLE) {
    if (!updateTextureDescriptorset()) {
      return;
    }
  }

  VkCommandBuffer buffer = m_bridge.GetCurrentCommandBuffer();

  VkDescriptorSet sets[1] = {m_texture_descriptorset};
  vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          m_bridge.GetPipeline().layout(), 1, 1, sets, 0,
                          nullptr);

  vkCmdDrawIndexed(buffer,
                   4,                  // indexCount
                   1,                  // instanceCount
                   0,                  // firstIndex
                   vertex_start_index, // vertexOffset
                   0                   // firstInstance
  );
}

bool VkTextureTile::create() {
  createTextureImage();
  
  if (!m_texture_imageview.Create(m_bridge.GetVkDevice(), m_texture_image, VK_FORMAT_B8G8R8A8_SRGB)) {
    return false;
  }
  return true;
}

bool VkTextureTile::findMemoryType(uint32_t typeFilter,
                                   VkMemoryPropertyFlags properties,
                                   uint32_t *out) {
  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(
      m_bridge.GetDeviceQueue().PhysicalDevice(), &memory_properties);

  for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      *out = i;
      return true;
    }
  }

  return false;
}

void VkTextureTile::copyBufferToImage(VkBuffer buffer, VkImage image, int left,
                                      int top, uint32_t width,
                                      uint32_t height) {
  Vk::CommandBuffer cb;
  cb.Attach(m_bridge.GetCommandPool().AllocateCommandBuffer(m_bridge.GetVkDevice()));
  cb.BeginRecordCommand();

  // 缓存从0偏移开始，存储了width*height的脏区域数据。
  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = width;  // 不用乘4
  region.bufferImageHeight = height;

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {left, top, 0};
  region.imageExtent = {width, height, 1};

  vkCmdCopyBufferToImage(cb, buffer, image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  cb.EndRecordCommand();
  m_bridge.GetDeviceQueue().Submit(cb);
  m_bridge.GetDeviceQueue().WaitIdle();
  m_bridge.GetCommandPool().ReleaseCommandBuffer(m_bridge.GetVkDevice(), cb.Detach());
}

void VkTextureTile::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                               VkDeviceSize size) {
  VkDevice device = m_bridge.GetVkDevice();

  Vk::CommandBuffer cb;
  cb.Attach(m_bridge.GetCommandPool().AllocateCommandBuffer(device));
  cb.BeginRecordCommand();

  VkBufferCopy copyRegion{};
  copyRegion.size = size;
  vkCmdCopyBuffer(cb, srcBuffer, dstBuffer, 1, &copyRegion);

  cb.EndRecordCommand();
  m_bridge.GetDeviceQueue().Submit(cb);
  m_bridge.GetDeviceQueue().WaitIdle();
  m_bridge.GetCommandPool().ReleaseCommandBuffer(device, cb.Detach());
}

bool VkTextureTile::transitionImageLayout(VkImage image, VkFormat format,
                                          VkImageLayout oldLayout,
                                          VkImageLayout newLayout) {
  VkDevice device = m_bridge.GetVkDevice();

  Vk::CommandBuffer cb;
  cb.Attach(m_bridge.GetCommandPool().AllocateCommandBuffer(device));

  cb.BeginRecordCommand();
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
    assert(false);
  }

  vkCmdPipelineBarrier(cb, sourceStage, destinationStage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);

  cb.EndRecordCommand();

  m_bridge.GetDeviceQueue().Submit(cb);

  // 如果不加wait会报错：
  // Attempt to free VkCommandBuffer 0x7f885410b4c8[] which is in use.
  // The Vulkan spec states: All elements of pCommandBuffers must not be in the pending state
  m_bridge.GetDeviceQueue().WaitIdle();

  m_bridge.GetCommandPool().ReleaseCommandBuffer(device, cb.Detach());

  return true;
}

bool VkTextureTile::createTextureImage() {
  VkDevice device = m_bridge.GetVkDevice();

  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = TILE_SIZE;
  imageInfo.extent.height = TILE_SIZE;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  // initialLayout must be VK_IMAGE_LAYOUT_UNDEFINED or VK_IMAGE_LAYOUT_PREINITIALIZED
  // (https://vulkan.lunarg.com/doc/view/1.3.261.1/mac/1.3-extensions/vkspec.html#VUID-VkImageCreateInfo-initialLayout-00993)
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  //
  // 当你调用 vkCreateImage 时，你并没有为图像数据分配任何实际的GPU内存。你所做的是：
  //
  // 向驱动程序描述你想要的图像：
  //     包括图像的尺寸（宽度、高度、深度）、格式（如VK_FORMAT_R8G8B8A8_SRGB）、
  //     用途（用作颜色附件、采样器等）、平铺方式（Linear 或 Optimal）等。
  //
  // 驱动程序检查需求的合法性：
  //     驱动程序会检查你请求的图像参数在当前物理设备和逻辑设备上是否支持。
  //
  // 获取一个图像句柄：
  //   如果创建成功，你会得到一个 VkImage 句柄。这个句柄代表了一个图像“对象”，
  //   但它目前还是一个“空壳”，没有存储像素数据的地方。它知道自己的结构和需求，
  //   但不知道数据存放在哪里。
  //
  if (vkCreateImage(device, &imageInfo, nullptr, &m_texture_image) !=
      VK_SUCCESS) {
    return false;
  }

  // 查询：
  //   需要多少字节的内存 (size)。
  //   内存对齐要求 (alignment)。
  //   哪些类型的内存堆是兼容的 (memoryTypeBits)。
  //
  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device, m_texture_image, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  if (!findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      &allocInfo.memoryTypeIndex)) {
    return false;
  }

  // 代表了一块实实在在的、已经被分配出来的GPU内存
  if (vkAllocateMemory(device, &allocInfo, nullptr, &m_texture_image_memory) !=
      VK_SUCCESS) {
    return false;
  }

  // vkBindImageMemory 的作用就是在 VkImage 和 VkDeviceMemory 之间建立一对一的绑定关系。
  vkBindImageMemory(device, m_texture_image, m_texture_image_memory, 0);
  return true;
}

} // namespace ui