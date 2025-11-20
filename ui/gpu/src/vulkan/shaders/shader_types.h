#ifndef UI_GPU_SRC_VULKAN_SHADERS_SHADERTYPES_H_
#define UI_GPU_SRC_VULKAN_SHADERS_SHADERTYPES_H_
#include <glm/glm.hpp>

namespace vulkan {

// shader.vert glsl文件中的顶点格式定义
// 每个Layer中的一系列顶点
struct VertexData {
  glm::vec2 pos;
  glm::vec3 color;
  glm::vec2 texCoord;
};

// 轻量级更新的数据
// 每个Layer更新一次的数据
struct LayerData {
  // 每个模型的世界坐标转换矩阵
  glm::mat4 model;
};

// 全局共享矩阵，一帧更新一次。
struct FrameData {
  glm::mat4 view;
  glm::mat4 ortho;
};

} // namespace vulkan

#endif