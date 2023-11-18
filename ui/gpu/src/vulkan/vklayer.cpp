#include "vklayer.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include "vktexturetile.h"
#include "vkcompositor.h"
#include "vulkan/vulkan_core.h"
#include <cmath>
#include <vector>

namespace ui {

//   Vulkan 坐标
//    ---------------------------------
//   | -1.0, -1.0            1.0, -1.0 |
//   |                                 |
//   |                                 |
//   | -1.0,  1.0            1.0,  1.0 |
//    ---------------------------------
//
const vulkan::Pipeline::ShaderVertex RECT_VERTICES[] = {
    {{-1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},  // 0
    {{1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},  // 1
    {{1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},   // 2
    {{-1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}}; // 3

const uint16_t RECT_INDICES[] = {0, 1, 2, 2, 3, 0};


// -------------------------------------


VulkanGpuLayer::VulkanGpuLayer(VulkanCompositor &compositor)
    : m_compositor(compositor) {}

VulkanGpuLayer::~VulkanGpuLayer() {
  auto device = m_compositor.GetVkDevice();
  vkDeviceWaitIdle(device);

  vkDestroyBuffer(device, m_indexBuffer, nullptr);
  vkFreeMemory(device, m_indexBufferMemory, nullptr);

  vkDestroyBuffer(device, m_vertexBuffer, nullptr);
  vkFreeMemory(device, m_vertexBufferMemory, nullptr);
}

TextureTile *VulkanGpuLayer::newTile() { 
  return new VkTextureTile(m_compositor); 
}

bool VulkanGpuLayer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties, VkBuffer &buffer,
                  VkDeviceMemory &bufferMemory) {

  auto device = m_compositor.GetVkDevice();

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
  if (!findMemoryType(memRequirements.memoryTypeBits, properties, &allocInfo.memoryTypeIndex)) {
    return false;
  }

  if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) !=
      VK_SUCCESS) {
    return false;
  }

  vkBindBufferMemory(device, buffer, bufferMemory, 0);
  return true;
}

void VulkanGpuLayer::createIndexBuffer() {
  auto device = m_compositor.GetVkDevice();

  VkDeviceSize bufferSize = sizeof(RECT_INDICES[0]) * RECT_INDICES.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingBufferMemory);

  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, RECT_INDICES.data(), (size_t)bufferSize);
  vkUnmapMemory(device, stagingBufferMemory);

  createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory);

  m_compositor.VulkanCopyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);
}


bool VulkanGpuLayer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, uint32_t* out) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_compositor.DeviceQueue().PhysicalDevice(), &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      *out = i;
      return true;
    }
  }
  return false;
}

void VulkanGpuLayer::createVertexBuffer() {
  auto device = m_compositor.GetVkDevice();

  VkDeviceSize bufferSize = sizeof(RECT_VERTICES[0]) * RECT_VERTICES.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer(bufferSize, 
    // 该buffer可以用作一个transfer command(传输命令)的源
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingBufferMemory);

  // 为了向BUFFER写入数据，需要先将BUFFER映射到CPU内存中进行读写。
  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, RECT_VERTICES.data(), (size_t)bufferSize);
  vkUnmapMemory(device, stagingBufferMemory);

  createBuffer(bufferSize,
    // 该buffer可以用作一个transfer command(传输命令)的目标
               VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                   // 
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
               m_vertexBuffer,
               m_vertexBufferMemory);

  m_compositor.VulkanCopyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void VulkanGpuLayer::Resize(int nWidth, int nHeight) {
  if (m_width == (int)nWidth && m_height == (int)nHeight) {
    return;
  }

  int col = (int)std::ceil((float)nWidth / TILE_SIZE);
  int row = (int)std::ceil((float)nHeight / TILE_SIZE);

  doCreateTile(row, col);

  m_width = nWidth;
  m_height = nHeight;

  createVertexBuffer();
  createIndexBuffer();
  // // 创建vbo
  // if (m_pVertexBuffer) {
  //   m_pVertexBuffer->Release();
  //   m_pVertexBuffer = nullptr;
  // }

  // D3D10_BUFFER_DESC BufDesc;
  // BufDesc.ByteWidth = sizeof(DXUT_SCREEN_VERTEX_10) * 4 * row * col;
  // BufDesc.Usage = D3D10_USAGE_DYNAMIC;
  // BufDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
  // BufDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
  // BufDesc.MiscFlags = 0;

  // D3D10App::Get()->m_pDevice->CreateBuffer(&BufDesc, nullptr,
  // &m_pVertexBuffer);

  // // 上传一个纹理所要用的顶点数据。批处理

  // DXUT_SCREEN_VERTEX_10 *pVB = nullptr;
  // if (SUCCEEDED(
  //         m_pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (LPVOID *)&pVB)))
  //         {
  //   for (int y = 0; y < row; ++y) {
  //     for (int x = 0; x < col; ++x) {
  //       int x1 = x + 1;
  //       int y1 = y + 1;

  //       ui::D3DCOLORVALUE color = {1, 1, 1, 1};
  //       DXUT_SCREEN_VERTEX_10 RECT_VERTICES[4] = {
  //           {(float)TILE_SIZE * x, (float)TILE_SIZE * y, 0.f, color, 0, 0},
  //           {(float)TILE_SIZE * x1, (float)TILE_SIZE * y, 0.f, color, 1, 0},
  //           {(float)TILE_SIZE * x, (float)TILE_SIZE * y1, 0.f, color, 0, 1},
  //           {(float)TILE_SIZE * x1, (float)TILE_SIZE * y1, 0.f, color, 1, 1},
  //       };

  //       CopyMemory(pVB, RECT_VERTICES, sizeof(DXUT_SCREEN_VERTEX_10) * 4);
  //       pVB += 4;
  //     }
  //   }

  //   m_pVertexBuffer->Unmap();
  // }
}


void VulkanGpuLayer::Compositor(GpuLayerCommitContext *pContext,
                                float *pMatrixTransform) {
  // vertexCount：尽管这里我们没有使用顶点缓冲，但仍然需要指定三个顶点用于三角形的绘制。
  // instanceCount：用于实例渲染，为1时表示不进行实例渲染。
  // firstVertex：用于定义着色器变量gl_VertexIndex的值。
  // firstInstance：用于定义着色器变量gl_InstanceIndex的值。
  // vkCmdDraw(buffer, 3, 1, 0, 0);

  vulkan::CommandBuffer* command_buffer = (vulkan::CommandBuffer*)pContext->m_data;
  VkCommandBuffer buffer = command_buffer->handle();

  VkBuffer vertexBuffers[] = {m_vertexBuffer};
  VkDeviceSize offsets[] = {0};
  
  vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
  if (true/*draw_by_index*/) {
    vkCmdBindIndexBuffer(buffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(buffer, static_cast<uint32_t>(RECT_INDICES.size()), 1, 0, 0, 0);
  }
  else {
    // vkCmdDraw(command, count, 1, 0, 0);
  }
}

} // namespace ui