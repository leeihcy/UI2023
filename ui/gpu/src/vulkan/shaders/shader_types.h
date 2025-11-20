#ifndef UI_GPU_SRC_VULKAN_SHADERS_SHADERTYPES_H_
#define UI_GPU_SRC_VULKAN_SHADERS_SHADERTYPES_H_
#include <glm/glm.hpp>

namespace vulkan {

  // 全局共享矩阵，一帧更新一次。
  struct UniformBufferObject {
    glm::mat4 view;
    // glm::mat4 proj;
    glm::mat4 ortho;
  };

  // 轻量级更新的数据
  struct PushData {
    // 每个模型的世界坐标转换矩阵
    glm::mat4 model;
  };

  // shader.vert glsl文件中的顶点格式定义
  struct ShaderVertex {
    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
  };
  
}

#endif