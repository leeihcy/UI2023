#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_PIPE_LINE_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_PIPE_LINE_H_
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_buffer.h"
#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace vulkan {
class DeviceQueue;

//
// 用于操作VkImage，例如带mipmapping的image
//
class Pipeline {
public:
  Pipeline(IVulkanBridge& bridge);
  ~Pipeline();

  VkPipeline handle() { return m_graphics_pipeline; }
  VkPipelineLayout layout() { return m_pipeline_layout; }
  VkDescriptorSet& descriptor_set(int index) { return m_descriptorSets[index]; }
  VkDescriptorSetLayout texture_descriptor_set_layout() { return m_texture_descriptor_set_layout; }
  VkDescriptorPool texture_descriptor_pool() { return m_texture_descriptor_pool; }
  VkSampler texture_sampler() { return m_texture_sampler; }
  bool Initialize(uint32_t w, uint32_t h, VkFormat format);
  void Destroy();

  VkRenderPass GetVkRenderPass() { return m_renderpass; }
  
public:
  // shader.vert glsl文件中的顶点格式定义
  struct ShaderVertex {
    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
  };

  struct UniformBufferObject {
    // glm::mat4 model;
    // glm::mat4 view;
    // glm::mat4 proj;

    glm::mat4 model;
    glm::mat4 ortho;
  };

  struct Context {
    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    VkPipelineVertexInputStateCreateInfo vertex_input{};
    VkVertexInputBindingDescription vertex_input_binding_description{};
    VkVertexInputAttributeDescription vertex_input_attribute_descriptions[3] = {};
    VkPipelineShaderStageCreateInfo vertex_shader{};
    VkPipelineShaderStageCreateInfo fragment_shader{};
    VkPipelineViewportStateCreateInfo viewport_state{};
    VkViewport viewport{};
    VkRect2D scissor{};
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    VkPipelineMultisampleStateCreateInfo multisampling{};
    VkPipelineColorBlendStateCreateInfo color_blending{};
    VkPipelineColorBlendAttachmentState color_blend_attachment{};
  
    VkGraphicsPipelineCreateInfo pipeline_info{
        .pVertexInputState = &vertex_input,
        .pInputAssemblyState = &input_assembly,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pColorBlendState = &color_blending,

    };
  };

private:
  bool create_graphics_pipeline(uint32_t w, uint32_t h, VkFormat format);
  bool create_shader_module(char* code, int length, VkShaderModule* out);

  void build_vertex_input(Context &ctx, ShaderVertex shader_vertex);
  void build_input_assembly(Context &ctx);
  bool build_vertex_shader(Context &ctx);
  bool build_fragment_shader(Context &ctx);
  void build_viewport_scissor(Context &ctx, uint32_t w, uint32_t h);
  void build_rasterization(Context &ctx);
  void build_color_blend(Context &ctx);
  void build_descriptor_set_layout(int texture_count);
  void build_texture_descriptor_set_layout();
  void create_descriptor_pool();
  void create_texture_descriptor_pool();
  void create_descriptor_sets();
  bool create_texture_sampler();
  bool build_layout();
  bool create_renderpass(VkFormat format);
  bool create_pipeline(Context &ctx);
  void destroy_context(Context &ctx);

  void create_uniform_buffers();
  

public:
  void UpdateUniformBuffer(uint32_t currentImage);
  void UpdateDescriptorSets(int count, VkImageView* texture_image_views);

private:
  IVulkanBridge& m_bridge; // raw_ptr<

  VkPipeline m_graphics_pipeline = VK_NULL_HANDLE;
  VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;

  VkRenderPass m_renderpass = VK_NULL_HANDLE;

  VkDescriptorSetLayout m_descriptor_set_layout = VK_NULL_HANDLE;
  VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
  std::vector<VkDescriptorSet> m_descriptorSets;

  VkDescriptorSetLayout m_texture_descriptor_set_layout = VK_NULL_HANDLE;
  VkDescriptorPool m_texture_descriptor_pool = VK_NULL_HANDLE;
  
  VkSampler m_texture_sampler = VK_NULL_HANDLE;

  std::vector<vulkan::Buffer> m_uniform_buffers;
};

}

#endif