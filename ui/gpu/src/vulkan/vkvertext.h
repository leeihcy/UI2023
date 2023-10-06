#ifndef _UI_GPU_SRC_VULKAN_VKVERTEX_H_
#define _UI_GPU_SRC_VULKAN_VKVERTEX_H_
#include <array>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace ui {

struct ShaderVertex {
  glm::vec2 pos;
  glm::vec3 color;
  // glm::vec2 texCoord;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(ShaderVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  static std::array<VkVertexInputAttributeDescription, 2/*3*/>
  getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2/*3*/> attributeDescriptions = {};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(ShaderVertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(ShaderVertex, color);

    // attributeDescriptions[2].binding = 0;
    // attributeDescriptions[2].location = 2;
    // attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    // attributeDescriptions[2].offset = offsetof(ShaderVertex, texCoord);

    return attributeDescriptions;
  }
};

}
#endif