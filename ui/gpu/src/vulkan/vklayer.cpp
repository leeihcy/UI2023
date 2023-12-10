#include "vklayer.h"
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_pipe_line.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include "vktexturetile.h"
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
// static const vulkan::Pipeline::ShaderVertex RECT_VERTICES[] = {
//     {{-1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} }, // 0
//     {{1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },  // 1
//     {{1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },   // 2
//     {{-1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} }}; // 3

// 加入正交投影后：
// static const vulkan::Pipeline::ShaderVertex RECT_VERTICES[] = {
//     {{0, 0}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} }, // 0
//     {{0, 256}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },  // 1
//     {{256, 256}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },   // 2
//     {{256, 0}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} }}; // 3    

// static const uint16_t RECT_INDICES[] = {0, 1, 2, 2, 3, 0};


// -------------------------------------

VulkanGpuLayer::VulkanGpuLayer(vulkan::IVulkanBridge& bridge)
    : m_bridge(bridge),
    m_vertexBuffer(bridge),
    m_indexBuffer(bridge) {}

VulkanGpuLayer::~VulkanGpuLayer() {
}

TextureTile *VulkanGpuLayer::newTile() {
  return new VkTextureTile(m_bridge);
}

void VulkanGpuLayer::createVertexBuffer() {

  glm::vec3 color(1.0f, 1.0f, 1.0f);
  std::vector<vulkan::Pipeline::ShaderVertex> vertex_array;

  for (int y = 0; y < m_arrayTile.GetRow(); ++y) {
    for (int x = 0; x < m_arrayTile.GetCol(); ++x) {
      int x1 = x + 1;
      int y1 = y + 1;

      // TBD: 还能继续优化，这里面应该有很多重复点。

      vertex_array.push_back(
          {{(float)TILE_SIZE * x, (float)TILE_SIZE * y}, color, {0, 0}});
      vertex_array.push_back(
          {{(float)TILE_SIZE * x1, (float)TILE_SIZE * y}, color, {1, 0}});
      vertex_array.push_back(
          {{(float)TILE_SIZE * x, (float)TILE_SIZE * y1}, color, {0, 1}});
      vertex_array.push_back(
          {{(float)TILE_SIZE * x1, (float)TILE_SIZE * y1}, color, {1, 1}});
    }
  }

  m_vertexBuffer.Create(vulkan::Buffer::VERTEX, (void *)vertex_array.data(),
                        sizeof(vulkan::Pipeline::ShaderVertex) *
                            vertex_array.size());
}

void VulkanGpuLayer::createIndexBuffer() {

 std::vector<uint16_t> index_array;

  int base = 0;
  for (int y = 0; y < m_arrayTile.GetRow(); ++y) {
    for (int x = 0; x < m_arrayTile.GetCol(); ++x) {

      index_array.push_back(0 + base);
      index_array.push_back(1 + base);
      index_array.push_back(2 + base);
      index_array.push_back(3 + base);
      
      base += 4;
    }
  }

  m_indexBuffer.Create(vulkan::Buffer::INDEX, (void*)index_array.data(),
                        sizeof(uint16_t)*index_array.size());

}

void VulkanGpuLayer::OnBeginCommit(GpuLayerCommitContext *ctx) {
   int row = m_arrayTile.GetRow();
  int col = m_arrayTile.GetCol();

  for (int y = 0; y < row; y++) {
    for (int x = 0; x < col; x++) {
      m_arrayTile[y][x]->OnBeginCommit(ctx);
    }
  }
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
}

void VulkanGpuLayer::Compositor(GpuLayerCommitContext *pContext,
                                float *pMatrixTransform) {

  vulkan::CommandBuffer *command_buffer =
      (vulkan::CommandBuffer *)pContext->m_data;
  VkCommandBuffer buffer = command_buffer->handle();

  VkBuffer vertexBuffers[] = {m_vertexBuffer.handle()};
  VkDeviceSize offsets[] = {0};

  vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
  vkCmdBindIndexBuffer(buffer, m_indexBuffer.handle(), 0, VK_INDEX_TYPE_UINT16);


  int row = m_arrayTile.GetRow();
  int col = m_arrayTile.GetCol();

  long xOffset = 0;
  long yOffset = 0;
  for (int y = 0; y < row; y++) {
    yOffset = y * TILE_SIZE;
    for (int x = 0; x < col; x++) {
      xOffset = x * TILE_SIZE;

      m_arrayTile[y][x]->Compositor(xOffset, yOffset, (y * col + x) * 4,
                                    pContext);
    }
  }

#if 0
  // vertexCount：尽管这里我们没有使用顶点缓冲，但仍然需要指定三个顶点用于三角形的绘制。
  // instanceCount：用于实例渲染，为1时表示不进行实例渲染。
  // firstVertex：用于定义着色器变量gl_VertexIndex的值。
  // firstInstance：用于定义着色器变量gl_InstanceIndex的值。
  // vkCmdDraw(buffer, 3, 1, 0, 0);

  vulkan::CommandBuffer *command_buffer =
      (vulkan::CommandBuffer *)pContext->m_data;
  VkCommandBuffer buffer = command_buffer->handle();

  VkBuffer vertexBuffers[] = {m_vertexBuffer.handle()};
  VkDeviceSize offsets[] = {0};

  vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(buffer, m_indexBuffer.handle(), 0, VK_INDEX_TYPE_UINT16);

    // vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
    //                         pipelineLayout, 0, 1,
    //                         &descriptorSets[currentFrame], 0, nullptr);

    vkCmdDrawIndexed(buffer, std::size(RECT_INDICES), 1,
                     0, 0, 0);
    // vkCmdDraw(command, count, 1, 0, 0);
#endif
}

} // namespace ui