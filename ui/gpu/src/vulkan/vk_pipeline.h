#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_PIPE_LINE_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_PIPE_LINE_H_
#include "src/vulkan/vk_bridge.h"
#include "src/vulkan/vk_objects.h"

#include "src/vulkan/shaders/shader_types.h"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>


namespace vulkan {
class DeviceQueue;

//
// Pipeline = vertex shader + 
//            fragment shader + 
//            geometry shader + 
//            rasterization state + 
//            depth stencil state +
//            color blend state + 
//            vertex input layout
//
// Vulkan 的设计理念是：在绘制时改变状态很昂贵。所以应该提前创建所有需要的pipeline，
// 而不是修改已有pipeline的状态。
// 当有不同的shader程序逻辑或状态配置时，就需要再增加另一个pipeline。
//
// Pipeline封装了几乎所有固定的渲染状态，描述的是 “如何绘制” ，而不是 “绘制什么” 或 “绘制到哪里”。
// 所以无论多少份swapchain image、inflight frame，只需要一份Pipeline就行。
//
class PipeLine {
public:
  PipeLine(IVulkanBridge &bridge);
  ~PipeLine();
  VkPipeline handle() { return m_graphics_pipeline; }

public:
  bool Create(uint32_t w, uint32_t h, VkFormat format);
  void Destroy();

  VkDescriptorSet AllocatateTextureDescriptorSets();

public:
  VkPipelineLayout layout() { return m_pipeline_layout.handle; }
  VkDescriptorSetLayout GetUniformLayout() { return m_uniform_layout; }
  VkDescriptorSetLayout GetTextureLayout() { return m_texture_layout; }
  VkSampler GetTextureSampler() { return m_texture_sampler; }

public:
  struct Context {
    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    VkPipelineVertexInputStateCreateInfo vertex_input{};
    VkVertexInputBindingDescription vertex_input_binding_description{};
    VkVertexInputAttributeDescription vertex_input_attribute_descriptions[3] =
        {};
    VkPipelineShaderStageCreateInfo vertex_shader{};
    VkPipelineShaderStageCreateInfo fragment_shader{};
    VkPipelineViewportStateCreateInfo viewport_state{};
    VkViewport viewport{};
    VkRect2D scissor{};
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    VkPipelineMultisampleStateCreateInfo multisampling{};
    VkPipelineColorBlendStateCreateInfo color_blending{};
    VkPipelineColorBlendAttachmentState color_blend_attachment{};

    VkGraphicsPipelineCreateInfo pipeline_info {
        .pVertexInputState = &vertex_input,
        .pInputAssemblyState = &input_assembly,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pColorBlendState = &color_blending,
    };
  };

private:
  bool create_shader_module(char *code, int length, VkShaderModule *out);

  void buildVertexInput(Context &ctx, VertexData shader_vertex);
  void build_input_assembly(Context &ctx);
  bool build_vertex_shader(Context &ctx);
  bool build_fragment_shader(Context &ctx);
  void build_viewport_scissor(Context &ctx, uint32_t w, uint32_t h);
  void build_rasterization(Context &ctx);
  void build_color_blend(Context &ctx);
  bool buildUniformLayout();
  bool buildTextureLayout();
  bool create_texture_sampler();
  bool build_layout();
  bool create_pipeline(Context &ctx);
  void destroy_context(Context &ctx);

public:
  void UpdateViewportScissor(uint32_t w, uint32_t h, VkCommandBuffer command_buffer);

private:
  IVulkanBridge &m_bridge; // raw_ptr

  Vk::Pipeline m_graphics_pipeline;

  Vk::PipelineLayout m_pipeline_layout;
  Vk::DescriptorSetLayout m_uniform_layout;
  Vk::DescriptorSetLayout m_texture_layout;
  
  Vk::Sampler m_texture_sampler;
};

} // namespace vulkan

#endif