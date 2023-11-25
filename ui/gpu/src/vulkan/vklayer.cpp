#include "vklayer.h"
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_command_buffer.h"
#include "vkcompositor.h"
#include "vktexturetile.h"
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
    {{-1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}}, // 0
    {{1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},  // 1
    {{1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},   // 2
    {{-1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}}; // 3

const uint16_t RECT_INDICES[] = {0, 1, 2, 2, 3, 0};

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

void VulkanGpuLayer::createIndexBuffer() {

  m_indexBuffer.Create(vulkan::Buffer::INDEX, (void*)RECT_INDICES,
                        sizeof(RECT_INDICES));
}

// bool VulkanGpuLayer::findMemoryType(uint32_t typeFilter,
//                                     VkMemoryPropertyFlags properties,
//                                     uint32_t *out) {
//   VkPhysicalDeviceMemoryProperties memProperties;
//   vkGetPhysicalDeviceMemoryProperties(
//       m_compositor.GetDeviceQueue().PhysicalDevice(), &memProperties);

//   for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//     if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
//                                     properties) == properties) {
//       *out = i;
//       return true;
//     }
//   }
//   return false;
// }

void VulkanGpuLayer::createVertexBuffer() {
  m_vertexBuffer.Create(vulkan::Buffer::VERTEX, (void*)RECT_VERTICES,
                        sizeof(RECT_VERTICES));
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

  vulkan::CommandBuffer *command_buffer =
      (vulkan::CommandBuffer *)pContext->m_data;
  VkCommandBuffer buffer = command_buffer->handle();

  VkBuffer vertexBuffers[] = {m_vertexBuffer.handle()};
  VkDeviceSize offsets[] = {0};

  vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
  if (true /*draw_by_index*/) {
    vkCmdBindIndexBuffer(buffer, m_indexBuffer.handle(), 0, VK_INDEX_TYPE_UINT16);

    // vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
    //                         pipelineLayout, 0, 1,
    //                         &descriptorSets[currentFrame], 0, nullptr);

    vkCmdDrawIndexed(buffer, std::size(RECT_INDICES), 1,
                     0, 0, 0);
  } else {
    // vkCmdDraw(command, count, 1, 0, 0);
  }
}

} // namespace ui