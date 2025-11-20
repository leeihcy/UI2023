#include "vk_layer.h"

#include "src/vulkan/vk_bridge.h"
#include "src/vulkan/vk_pipeline.h"
#include "src/vulkan/vk_swapchain_image.h"
#include "src/vulkan/vk_swapchain.h"
#include "src/vulkan/vk_tile.h"
#include "src/util.h"
#include <vector>

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

namespace ui {

//   Vulkan 坐标
//   +---------------------------------+
//   | -1.0, -1.0            1.0, -1.0 |
//   |                                 |
//   |                                 |
//   | -1.0,  1.0            1.0,  1.0 |
//   +---------------------------------+
//


VulkanGpuLayer::VulkanGpuLayer(vulkan::IVulkanBridge &bridge)
    : m_bridge(bridge), m_vertexBuffer(bridge), m_indexBuffer(bridge) {}

VulkanGpuLayer::~VulkanGpuLayer() {}

TextureTile *VulkanGpuLayer::newTile() { return new VkTextureTile(m_bridge); }

void VulkanGpuLayer::createVertexBuffer() {

  glm::vec3 color(1.0f, 1.0f, 1.0f);
  std::vector<vulkan::ShaderVertex> vertex_array;

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

  size_t data_size = sizeof(vulkan::ShaderVertex) *
                            vertex_array.size();
  m_vertexBuffer.Create(vulkan::Buffer::VERTEX, (void *)vertex_array.data(),
                       data_size);
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

  m_indexBuffer.Create(vulkan::Buffer::INDEX, (void *)index_array.data(),
                       sizeof(uint16_t) * index_array.size());
}

void VulkanGpuLayer::Resize(int nWidth, int nHeight) {
  if (m_width == (int)nWidth && m_height == (int)nHeight) {
    return;
  }
  ui::Log("VulkanGpuLayer Resize: width=%d, height=%d", nWidth, nHeight);

  doCreateTile(nWidth, nHeight);

  m_width = nWidth;
  m_height = nHeight;

  createVertexBuffer();
  createIndexBuffer();
}

void VulkanGpuLayer::Compositor(GpuLayerCommitContext *pContext,
                                float *pMatrixTransform) {
  if (m_width == 0 || m_height == 0) {
    return;
  }
  vulkan::SwapChainFrame* frame = m_bridge.GetSwapChain().GetCurrentFrame();
  VkCommandBuffer buffer = m_bridge.GetCurrentCommandBuffer();

  VkBuffer vertexBuffers[] = {m_vertexBuffer.handle()};
  VkDeviceSize offsets[] = {0};

  vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
  vkCmdBindIndexBuffer(buffer, m_indexBuffer.handle(), 0, VK_INDEX_TYPE_UINT16);

  // 更新layer在世界坐标转换矩阵。
  glm::mat4 transfrom;
  if (pMatrixTransform) {
    memcpy(&transfrom, pMatrixTransform, sizeof(transfrom));
  } else {
    transfrom = glm::mat4(1.0f);
  }
  glm::mat4 model = glm::translate(
      transfrom, glm::vec3(pContext->m_xOffset, pContext->m_yOffset, 0));
  frame->UpdatePushData(buffer, model);

  int row = m_arrayTile.GetRow();
  int col = m_arrayTile.GetCol();

  int index = 0;
  for (int y = 0; y < row; y++) {
    for (int x = 0; x < col; x++) {
      // 不需要偏移量，只需要顶点索引。
      static_cast<VkTextureTile*>(m_arrayTile[y][x])->Compositor(index, pContext);
      index += 4;
    }
  }
}

} // namespace ui