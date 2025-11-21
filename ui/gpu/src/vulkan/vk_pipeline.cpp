#include "vk_pipeline.h"
#include "src/util.h"
#include "src/vulkan/vk_swapchain.h"
#include "src/vulkan/vk_bridge.h"
#include "src/vulkan/vk_buffer.h"
#include "src/vulkan/vk_device.h"

#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <vulkan/vulkan.h>

namespace vulkan {

PipeLine::PipeLine(IVulkanBridge &bridge)
    : m_bridge(bridge){}

PipeLine::~PipeLine() {
  Destroy();
}

void PipeLine::Destroy() {
  VkDevice device = m_bridge.GetVkDevice();

  m_pipeline_layout.Destroy(device);
  m_graphics_pipeline.Destroy(device);
  m_texture_sampler.Destroy(device);
  m_uniform_layout.Destroy(device);
  m_texture_layout.Destroy(device);
}

bool PipeLine::Create(uint32_t w, uint32_t h, VkFormat format) {
  Context context;

  bool success = false;
  do {
    buildVertexInput(context, VertexData());
    build_input_assembly(context);
    if (!build_vertex_shader(context))
      break;
    build_viewport_scissor(context, w, h);
    build_rasterization(context);
    if (!build_fragment_shader(context))
      break;
    build_color_blend(context);

    if (!build_layout()) {
      break;
    }
    if (!create_pipeline(context)) {
      break;
    }
    if (!create_texture_sampler()) {
      break;
    }
    success = true;
  } while (0);

  destroy_context(context);
  if (!success) {
    Destroy();
  }

  return success;
}

void PipeLine::UpdateViewportScissor(uint32_t w, uint32_t h,
                                     VkCommandBuffer command_buffer) {
  Context context;
  build_viewport_scissor(context, w, h);

  vkCmdSetViewport(command_buffer, 0, 1, &context.viewport);
  vkCmdSetScissor(command_buffer, 0, 1, &context.scissor);

  destroy_context(context);
}

static bool readSpvFile(const char *filename, std::vector<char> &buffer) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    return false;
  }

  size_t fileSize = (size_t)file.tellg();
  buffer.resize(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return true;
}

// 顶点格式设置
// layout(location = 0) in vec2 inPosition;
// layout(location = 1) in vec3 inColor;   
// layout(location = 2) in vec2 inTexCoord;
void PipeLine::buildVertexInput(Context &ctx, VertexData shader_vertex) {
  ctx.vertex_input.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  ctx.vertex_input_binding_description.binding = 0;
  ctx.vertex_input_binding_description.stride = sizeof(shader_vertex);
  ctx.vertex_input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  ctx.vertex_input.vertexBindingDescriptionCount = 1;
  ctx.vertex_input.pVertexBindingDescriptions =
      &ctx.vertex_input_binding_description;

  ctx.vertex_input_attribute_descriptions[0].binding = 0;
  ctx.vertex_input_attribute_descriptions[0].location = 0;
  ctx.vertex_input_attribute_descriptions[0].format =
      VK_FORMAT_R32G32B32_SFLOAT;
  ctx.vertex_input_attribute_descriptions[0].offset =
      offsetof(VertexData, pos);
  ctx.vertex_input_attribute_descriptions[1].binding = 0;
  ctx.vertex_input_attribute_descriptions[1].location = 1;
  ctx.vertex_input_attribute_descriptions[1].format =
      VK_FORMAT_R32G32B32_SFLOAT;
  ctx.vertex_input_attribute_descriptions[1].offset =
      offsetof(VertexData, color);
  ctx.vertex_input_attribute_descriptions[2].binding = 0;
  ctx.vertex_input_attribute_descriptions[2].location = 2;
  ctx.vertex_input_attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
  ctx.vertex_input_attribute_descriptions[2].offset =
      offsetof(VertexData, texCoord);

  ctx.vertex_input.vertexAttributeDescriptionCount =
      (uint32_t)std::size(ctx.vertex_input_attribute_descriptions);
  ctx.vertex_input.pVertexAttributeDescriptions =
      ctx.vertex_input_attribute_descriptions;
}

void PipeLine::build_input_assembly(Context &ctx) {
  ctx.input_assembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

  // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; 需要6个索引
  ctx.input_assembly.topology =
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; // 只需要4个索引
  ctx.input_assembly.primitiveRestartEnable = VK_FALSE;
}

bool PipeLine::build_vertex_shader(Context &ctx) {
  std::vector<char> vertShaderCode;
  if (!readSpvFile("shaders/vert.spv", vertShaderCode)) {
    return false;
  }
  if (!create_shader_module(vertShaderCode.data(),
                            (uint32_t)vertShaderCode.size(),
                            &ctx.vertex_shader.module)) {
    return false;
  }

  ctx.vertex_shader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  ctx.vertex_shader.stage = VK_SHADER_STAGE_VERTEX_BIT;
  ctx.vertex_shader.pName = "main";
  return true;
}

// Orthographic Projection Matrix
// 正交投影矩阵
void PipeLine::build_viewport_scissor(Context &ctx, uint32_t width,
                                      uint32_t height) {
  ctx.viewport.x = 0.0f;
  ctx.viewport.y = 0.0f;
  ctx.viewport.width = (float)width;   // m_swapchain.Extent2D().width;
  ctx.viewport.height = (float)height; // m_swapchain.Extent2D().height;
  ctx.viewport.minDepth = 0.0f;
  ctx.viewport.maxDepth = 1.0f;

  ctx.scissor.offset = {0, 0};
  ctx.scissor.extent = {width, height};

  ctx.viewport_state.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  ctx.viewport_state.viewportCount = 1;
  ctx.viewport_state.pViewports = &ctx.viewport;
  ctx.viewport_state.scissorCount = 1;
  ctx.viewport_state.pScissors = &ctx.scissor;
}

void PipeLine::build_rasterization(Context &ctx) {
  ctx.rasterizer.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  ctx.rasterizer.depthClampEnable = VK_FALSE;
  ctx.rasterizer.rasterizerDiscardEnable = VK_FALSE;
  ctx.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  ctx.rasterizer.lineWidth = 1.0f;
  ctx.rasterizer.cullMode = VK_CULL_MODE_NONE;
  // ctx.rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  ctx.rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  // ctx.rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

  ctx.rasterizer.depthBiasEnable = VK_FALSE;

  ctx.multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  ctx.multisampling.sampleShadingEnable = VK_FALSE;
  ctx.multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
}

bool PipeLine::build_fragment_shader(Context &ctx) {
  std::vector<char> fragShaderCode;
  if (!readSpvFile("shaders/frag.spv", fragShaderCode)) {
    return false;
  }
  if (!create_shader_module(fragShaderCode.data(),
                            (uint32_t)fragShaderCode.size(),
                            &ctx.fragment_shader.module)) {
    return false;
  }

  ctx.fragment_shader.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  ctx.fragment_shader.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  ctx.fragment_shader.pName = "main";

  return true;
}

void PipeLine::build_color_blend(Context &ctx) {
  ctx.color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  // 启用透明渲染（否则透明区域会显示成黑色）
  ctx.color_blend_attachment.blendEnable = VK_TRUE;
  ctx.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  ctx.color_blend_attachment.dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  ctx.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  ctx.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  ctx.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  ctx.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

  ctx.color_blending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  ctx.color_blending.logicOpEnable = VK_FALSE;
  ctx.color_blending.logicOp = VK_LOGIC_OP_COPY;
  ctx.color_blending.attachmentCount = 1;
  ctx.color_blending.pAttachments = &ctx.color_blend_attachment;
  ctx.color_blending.blendConstants[0] = 0.0f;
  ctx.color_blending.blendConstants[1] = 0.0f;
  ctx.color_blending.blendConstants[2] = 0.0f;
  ctx.color_blending.blendConstants[3] = 0.0f;
}

bool PipeLine::create_texture_sampler() {
  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(m_bridge.GetDeviceQueue().PhysicalDevice(),
                                &properties);

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;

  // 解决layer分块后，将其进行旋转后，在tile边缘处没衔接上的问题。
  // 对应于d3d中的 SamplerState AddressU = MIRROR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;

  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  // samplerInfo.anisotropyEnable = VK_TRUE;
  // samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  samplerInfo.anisotropyEnable = VK_FALSE;
  samplerInfo.maxAnisotropy = 1.0f;

  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  if (vkCreateSampler(m_bridge.GetVkDevice(), &samplerInfo, nullptr,
                      &m_texture_sampler) != VK_SUCCESS) {
    return false;
  }
  return true;
}

//
// Uniform 的核心思想是 “一致” 或 “统一”。
// 它指的是在着色器程序的一次绘制调用（Draw Call）期间，对所有顶点、
// 片段或其他着色器线程来说，其值都保持恒定不变的数据。
//
// 对应shader.vert文件
// 每个绑定(Binding)需要定义一个VkDescriptorSetLayoutBinding。
// 目前shader.vert中只有一个binding:
//  layout(binding = 0) uniform FrameData
//
bool PipeLine::buildUniformLayout() {
  VkDescriptorSetLayoutBinding bindings[] = {
      // 只有1个ubo binding
      {
          .binding = 0,
          .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          .descriptorCount = 1,                     // 只有一个ubo
          .stageFlags = VK_SHADER_STAGE_VERTEX_BIT, // 哪些着色器阶段可以访问。
      }};

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = std::size(bindings);
  layoutInfo.pBindings = bindings;

  if (vkCreateDescriptorSetLayout(m_bridge.GetVkDevice(), &layoutInfo, nullptr,
                                  &m_uniform_layout) != VK_SUCCESS) {
    ui::Log("failed to create descriptor set layout!");
    return false;
  }
  return true;
}

// 对应shader.frag文件
// layout(set = 1, binding = 0) uniform sampler2D texSampler;
//
bool PipeLine::buildTextureLayout() {
  VkDescriptorSetLayoutBinding textureBindings[] = {{
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT, 
  }};

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = std::size(textureBindings);
  layoutInfo.pBindings = textureBindings;

  if (vkCreateDescriptorSetLayout(m_bridge.GetVkDevice(), &layoutInfo, nullptr,
                                  &m_texture_layout) !=
      VK_SUCCESS) {
    ui::Log("failed to create descriptor set layout!");
    return false;
  }
  return true;
}

// 构建shader文件中的布局。
bool PipeLine::build_layout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  if (!this->buildUniformLayout()) {
    return false;
  }
  if (!this->buildTextureLayout()) {
    return false;
  }

  VkDescriptorSetLayout descriptorSetLayouts[2] = {
      m_uniform_layout, m_texture_layout};
  pipelineLayoutInfo.setLayoutCount = std::size(descriptorSetLayouts);
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts;

  // push constant，见struct LayerData
  VkPushConstantRange pushConstantRange = {
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT, // 在顶点着色器中使用
      .offset = 0,
      .size = sizeof(float) * 16, // mat4
  };
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if (vkCreatePipelineLayout(m_bridge.GetVkDevice(), &pipelineLayoutInfo,
                             nullptr, &m_pipeline_layout.handle) != VK_SUCCESS) {
    return false;
  }
  return true;
}

bool PipeLine::create_pipeline(Context &ctx) {
  VkPipelineShaderStageCreateInfo shaderStages[] = {ctx.vertex_shader,
                                                    ctx.fragment_shader};

  ctx.pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  ctx.pipeline_info.stageCount = std::size(shaderStages);
  ctx.pipeline_info.pStages = shaderStages;
  ctx.pipeline_info.layout = m_pipeline_layout;
  ctx.pipeline_info.renderPass = m_bridge.GetVkRenderPass();
  ctx.pipeline_info.subpass = 0;
  ctx.pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

  // 将和窗口大小相关的属性，设置为Dynamic，这样窗口大小改变时，
  // 不需要重新创建pipe line，只需要更新对应属性即可。
  std::vector<VkDynamicState> dynamicStates = {
      VK_DYNAMIC_STATE_VIEWPORT, // 视口动态
      VK_DYNAMIC_STATE_SCISSOR   // 裁剪矩形动态
  };
  VkPipelineDynamicStateCreateInfo dynamicStateInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
      .pDynamicStates = dynamicStates.data()};
  ctx.pipeline_info.pDynamicState = &dynamicStateInfo;

  if (vkCreateGraphicsPipelines(m_bridge.GetVkDevice(), VK_NULL_HANDLE, 1,
                                &ctx.pipeline_info, nullptr,
                                &m_graphics_pipeline) != VK_SUCCESS) {
    return false;
  }

  return true;
}

void PipeLine::destroy_context(Context &ctx) {
  if (ctx.vertex_shader.module != VK_NULL_HANDLE) {
    vkDestroyShaderModule(m_bridge.GetVkDevice(), ctx.vertex_shader.module,
                          nullptr);
    ctx.vertex_shader.module = VK_NULL_HANDLE;
  }

  if (ctx.fragment_shader.module != VK_NULL_HANDLE) {
    vkDestroyShaderModule(m_bridge.GetVkDevice(), ctx.fragment_shader.module,
                          nullptr);
    ctx.fragment_shader.module = VK_NULL_HANDLE;
  }
}

bool PipeLine::create_shader_module(char *code, int length,
                                    VkShaderModule *out_module) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = length;
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code);

  if (vkCreateShaderModule(m_bridge.GetVkDevice(), &createInfo, nullptr,
                           out_module) != VK_SUCCESS) {
    return false;
  }

  return true;
}

} // namespace vulkan