#include "vktexturetile.h"
#include "include/api.h"
#include "src/vulkan/vkpipeline.h"
#include "src/util.h"
#include <vulkan/vulkan_core.h>
#include <memory.h>

namespace ui {

VkTextureTile::VkTextureTile(vulkan::IVulkanBridge &bridge)
    : m_bridge(bridge) {}

VkTextureTile::~VkTextureTile() {
  auto device = m_bridge.GetVkDevice();
  
  m_texture_imageview.Destroy(device);
  vkFreeMemory(device, m_texture_image_memory, nullptr);
  m_texture_image.Destroy(device);
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

  
  // 几种最常用的 Image Layout：
  //
  // VK_IMAGE_LAYOUT_UNDEFINED：
  // 含义：布局未定义，图像内容无保证（可能是垃圾数据）。
  // 用途：通常用作布局转换的初始状态。因为你不在乎旧数据，所以转换到这种布局开销很小。例如，在开始渲染前，将交换链图像转换到 COLOR_ATTACHMENT_OPTIMAL 时，初始布局常用 UNDEFINED。
  //
  // VK_IMAGE_LAYOUT_GENERAL：
  // 含义：通用布局。对所有操作都“基本可用”，但通常不是性能最优的。
  // 用途：当图像在单个渲染过程中需要被用于多种不同类型的操作，且没有其他更合适的布局时使用。例如，在计算着色器中写入，然后在片段着色器中采样。
  // 
  // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL：
  // 含义：对作为颜色附件进行写入操作最优的布局。
  // 用途：在渲染通道中，作为 VK_ATTACHMENT_LOAD_OP_LOAD 或 VK_ATTACHMENT_STORE_OP_STORE 的颜色附件时使用。
  //
  // VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL：
  // 含义：对作为深度/模板附件进行读写操作最优的布局。
  // 用途：在渲染通道中，作为深度或模板测试的附件时使用。
  //
  // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL：
  // 含义：对在着色器（如片段着色器）中进行采样操作最优的布局。
  // 用途：将图像作为纹理贴图使用时。
  //
  // VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL / VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL：
  // 含义：对作为复制命令的源或目标最优的布局。
  // 用途：使用 vkCmdCopyImage 等命令进行图像数据拷贝时。
  //
  // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR：
  // 含义：对呈现（Present）到屏幕最优的布局。
  // 用途：交换链图像在渲染完成后，准备呈现给窗口系统时。
  //
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
}

// 每次上传纹理数据时，只是更新了VkImage内容，VkImage对象并没有替换。
// 所以三缓冲理论上都可以共用一个DescriptorSet。
bool VkTextureTile::updateTextureDescriptorset() {
  if (!m_texture_imageview.handle) {
    return false;
  }
  if (m_texture_descriptorset == VK_NULL_HANDLE) {
    m_texture_descriptorset =
        m_bridge.GetPipeline().AllocatateTextureDescriptorSets();
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

void VkTextureTile::Compositor(long, long, long vertexStartIndex,
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
                   4,                // indexCount
                   1,                // instanceCount
                   0,                // firstIndex
                   vertexStartIndex, // vertexOffset
                   0                 // firstInstance
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

void VkTextureTile::copyBufferToImage(VkBuffer buffer, VkImage image,
                                      uint32_t width, uint32_t height) {
  Vk::CommandBuffer cb;
  cb.Attach(m_bridge.GetCommandPool().AllocateCommandBuffer(m_bridge.GetVkDevice()));
  cb.BeginRecordCommand();

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


} // namespace ui