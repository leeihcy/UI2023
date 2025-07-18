#include "vulkan_pipe_line.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "src/vulkan/vkapp.h"
#include "src/vulkan/wrap/vulkan_bridge.h"
#include "src/vulkan/wrap/vulkan_buffer.h"
#include "src/vulkan/wrap/vulkan_swap_chain.h"
#include "src/vulkan/wrap/vulkan_util.h"
#include "vulkan_device_queue.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

namespace vulkan {

Pipeline::Pipeline(IVulkanBridge &bridge) : m_bridge(bridge) {}

Pipeline::~Pipeline() { Destroy(); }

bool Pipeline::Initialize(uint32_t w, uint32_t h, VkFormat format) {
  if (!create_graphics_pipeline(w, h, format)) {
    return false;
  }
  return true;
}

void Pipeline::UpdateViewportScissor(uint32_t w, uint32_t h, VkCommandBuffer command_buffer) {
  Context context;
  build_viewport_scissor(context, w, h);

  vkCmdSetViewport(command_buffer, 0, 1, &context.viewport);
  vkCmdSetScissor(command_buffer, 0, 1, &context.scissor);

  destroy_context(context);
}

void Pipeline::Destroy() {
  if (m_renderpass != VK_NULL_HANDLE) {
    vkDestroyRenderPass(m_bridge.GetVkDevice(), m_renderpass, nullptr);
    m_renderpass = VK_NULL_HANDLE;
  }
  if (m_pipeline_layout != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(m_bridge.GetVkDevice(), m_pipeline_layout, nullptr);
    m_pipeline_layout = VK_NULL_HANDLE;
  }
  if (m_graphics_pipeline != VK_NULL_HANDLE) {
    vkDestroyPipeline(m_bridge.GetVkDevice(), m_graphics_pipeline, nullptr);
    m_graphics_pipeline = VK_NULL_HANDLE;
  }

  if (m_texture_sampler != VK_NULL_HANDLE) {
    vkDestroySampler(m_bridge.GetVkDevice(), m_texture_sampler, nullptr);
    m_texture_sampler = VK_NULL_HANDLE;
  }

  if (m_descriptor_set_layout != VK_NULL_HANDLE) {
    vkDestroyDescriptorSetLayout(m_bridge.GetVkDevice(),
                                 m_descriptor_set_layout, nullptr);
    m_descriptor_set_layout = VK_NULL_HANDLE;
  }
  if (m_texture_descriptor_set_layout != VK_NULL_HANDLE) {
    vkDestroyDescriptorSetLayout(m_bridge.GetVkDevice(),
                                 m_texture_descriptor_set_layout, nullptr);
    m_texture_descriptor_set_layout = VK_NULL_HANDLE;
  }

  if (m_descriptor_pool != VK_NULL_HANDLE) {
    if (!m_arr_descriptor_sets.empty()) {
      // 创建pool时，我们使用了 CREATE_FREE_DESCRIPTOR_SET_BIT，
      // 这里我们自己释放资源。
      vkFreeDescriptorSets(m_bridge.GetVkDevice(), m_descriptor_pool,
                           (uint32_t)m_arr_descriptor_sets.size(),
                           m_arr_descriptor_sets.data());
      m_arr_descriptor_sets.clear();
    }

    m_arr_uniform_buffers.clear();

    vkDestroyDescriptorPool(m_bridge.GetVkDevice(), m_descriptor_pool, nullptr);
    m_descriptor_pool = VK_NULL_HANDLE;
  }

  if (m_texture_descriptor_pool != VK_NULL_HANDLE) {
    vkDestroyDescriptorPool(m_bridge.GetVkDevice(), m_texture_descriptor_pool,
                            nullptr);
    m_texture_descriptor_pool = VK_NULL_HANDLE;
  }
}

bool Pipeline::create_graphics_pipeline(uint32_t w, uint32_t h,
                                        VkFormat format) {
  Context context;

  bool success = false;
  do {
    build_vertex_input(context, ShaderVertex());
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
    if (!create_renderpass(format)) {
      break;
    }
    if (!create_pipeline(context)) {
      break;
    }
    if (!create_texture_sampler()) {
      break;
    }

    create_uniform_buffers();
    create_descriptor_pool();
    create_descriptor_sets();

    create_texture_descriptor_pool();

    success = true;
  } while (0);

  destroy_context(context);
  if (!success) {
    Destroy();
  }

  return success;
}

static bool read_spv_file(const char *filename, std::vector<char> &buffer) {
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
void Pipeline::build_vertex_input(Context &ctx, ShaderVertex shader_vertex) {
  ctx.vertex_input.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

#if 0 // 直接在shader中写死顶点的版本
  ctx.vertex_input.vertexBindingDescriptionCount = 0;
  ctx.vertex_input.vertexAttributeDescriptionCount = 0;
#else
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
      offsetof(ShaderVertex, pos);
  ctx.vertex_input_attribute_descriptions[1].binding = 0;
  ctx.vertex_input_attribute_descriptions[1].location = 1;
  ctx.vertex_input_attribute_descriptions[1].format =
      VK_FORMAT_R32G32B32_SFLOAT;
  ctx.vertex_input_attribute_descriptions[1].offset =
      offsetof(ShaderVertex, color);
  ctx.vertex_input_attribute_descriptions[2].binding = 0;
  ctx.vertex_input_attribute_descriptions[2].location = 2;
  ctx.vertex_input_attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
  ctx.vertex_input_attribute_descriptions[2].offset =
      offsetof(ShaderVertex, texCoord);

  ctx.vertex_input.vertexAttributeDescriptionCount =
      (uint32_t)std::size(ctx.vertex_input_attribute_descriptions);
  ctx.vertex_input.pVertexAttributeDescriptions =
      ctx.vertex_input_attribute_descriptions;
#endif
}

void Pipeline::build_input_assembly(Context &ctx) {
  ctx.input_assembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

  // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; 需要6个索引
  ctx.input_assembly.topology =
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; // 只需要4个索引
  ctx.input_assembly.primitiveRestartEnable = VK_FALSE;
}

bool Pipeline::build_vertex_shader(Context &ctx) {
  std::vector<char> vertShaderCode;
  if (!read_spv_file("shaders/vert.spv", vertShaderCode)) {
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
void Pipeline::build_viewport_scissor(Context &ctx, uint32_t width,
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

void Pipeline::build_rasterization(Context &ctx) {
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

bool Pipeline::build_fragment_shader(Context &ctx) {
  std::vector<char> fragShaderCode;
  if (!read_spv_file("shaders/frag.spv", fragShaderCode)) {
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

void Pipeline::build_color_blend(Context &ctx) {
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

bool Pipeline::create_texture_sampler() {
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

// 对应shader.vert文件
// 每个绑定(Binding)需要定义一个VkDescriptorSetLayoutBinding。
// 目前shader.vert中只有一个binding:
//  layout(binding = 0) uniform UniformBufferObject
//
void Pipeline::build_descriptor_set_layout() {
  VkDescriptorSetLayoutBinding bindings[] = {
      // 只有1个ubo binding
      {
          .binding = 0, // binding的索引
          .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          .descriptorCount = 1,                     // 只有一个ubo
          .stageFlags = VK_SHADER_STAGE_VERTEX_BIT, // 哪此着色器阶段可以访问。
      }};

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = std::size(bindings);
  layoutInfo.pBindings = bindings;

  if (vkCreateDescriptorSetLayout(m_bridge.GetVkDevice(), &layoutInfo, nullptr,
                                  &m_descriptor_set_layout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

// 对应shader.frag文件
// layout(set = 1, binding = 0) uniform sampler2D texSampler;
//
void Pipeline::build_texture_descriptor_set_layout() {
  VkDescriptorSetLayoutBinding textureBindings[] = {{
      .binding = 0, // binding索引
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1, // 目前只有一个纹理，没用数组。
      .stageFlags =
          VK_SHADER_STAGE_FRAGMENT_BIT, // 指定在frag shader中使用sampler
  }};

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = std::size(textureBindings);
  layoutInfo.pBindings = textureBindings;

  if (vkCreateDescriptorSetLayout(m_bridge.GetVkDevice(), &layoutInfo, nullptr,
                                  &m_texture_descriptor_set_layout) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

void Pipeline::create_descriptor_pool() {
  vulkan::SwapChain &swapchain = m_bridge.GetSwapChain();

  VkDescriptorPoolSize poolSizes[] = {
      {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
       .descriptorCount = static_cast<uint32_t>(swapchain.Size())}};

  VkDescriptorPoolCreateInfo poolInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,

      // * 当设置 VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT 标志时：
      // 允许显式释放单个描述符集：
      // 你可以调用 vkFreeDescriptorSets() 主动释放池中的
      // 特定描述符集，而无需重置或销毁整个池。
      //
      // * 未设置时：
      // 描述符集只能通过 vkResetDescriptorPool()
      // 批量释放（所有集合一起释放），或随池销毁时自动释放。
      //
      .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,

      .maxSets = static_cast<uint32_t>(swapchain.Size()),
      .poolSizeCount = std::size(poolSizes),
      .pPoolSizes = poolSizes};

  if (vkCreateDescriptorPool(m_bridge.GetVkDevice(), &poolInfo, nullptr,
                             &m_descriptor_pool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

void Pipeline::create_texture_descriptor_pool() {
  VkDescriptorPoolSize texturePoolSizes[1] = {};
  texturePoolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  texturePoolSizes[0].descriptorCount = 999999; // TBD

  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = std::size(texturePoolSizes);
  poolInfo.pPoolSizes = texturePoolSizes;
  poolInfo.maxSets = 999999; // TBD

  if (vkCreateDescriptorPool(m_bridge.GetVkDevice(), &poolInfo, nullptr,
                             &m_texture_descriptor_pool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

// swapchain三缓冲，则需要对应分配三个描述符集。
void Pipeline::create_descriptor_sets() {
  vulkan::SwapChain &swapchain = m_bridge.GetSwapChain();
  const uint32_t count = swapchain.Size();

  // 三个descriptor sets，使用三个相同的布局。
  std::vector<VkDescriptorSetLayout> layouts(count, m_descriptor_set_layout);

  VkDescriptorSetAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .descriptorPool = m_descriptor_pool,

      // vkAllocateDescriptorSets一次性创建count个
      .descriptorSetCount = count,
      .pSetLayouts = layouts.data(),
  };

  m_arr_descriptor_sets.resize(count);
  if (vkAllocateDescriptorSets(m_bridge.GetVkDevice(), &allocInfo,
                               &m_arr_descriptor_sets[0]) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }
}

VkDescriptorSet Pipeline::AllocatateTextureDescriptorSets() {
  VkDescriptorSetAllocateInfo textureAllocInfo = {};
  textureAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  textureAllocInfo.descriptorPool = texture_descriptor_pool();

  VkDescriptorSetLayout layouts[1] = {
      m_bridge.GetPipeline().texture_descriptor_set_layout()};
  textureAllocInfo.descriptorSetCount = std::size(layouts);
  textureAllocInfo.pSetLayouts = layouts;

  VkDescriptorSet texture_descriptorset = VK_NULL_HANDLE;
  if (vkAllocateDescriptorSets(m_bridge.GetVkDevice(), &textureAllocInfo,
                               &texture_descriptorset) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets");
  }
  return texture_descriptorset;
}

// 构建shader文件中的布局。
bool Pipeline::build_layout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  this->build_descriptor_set_layout();
  this->build_texture_descriptor_set_layout();

  VkDescriptorSetLayout descriptorSetLayouts[2] = {
      m_descriptor_set_layout, m_texture_descriptor_set_layout};
  pipelineLayoutInfo.setLayoutCount = std::size(descriptorSetLayouts);
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts;

  // push constant，见struct PushData
  VkPushConstantRange pushConstantRange = {
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT, // 在顶点着色器中使用
      .offset = 0,
      .size = sizeof(float) * 16, // mat4
  };
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if (vkCreatePipelineLayout(m_bridge.GetVkDevice(), &pipelineLayoutInfo,
                             nullptr, &m_pipeline_layout) != VK_SUCCESS) {
    return false;
  }
  return true;
}

bool Pipeline::create_renderpass(VkFormat format) {
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = format; // m_swapchain.ImageFormat();
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  if (vkCreateRenderPass(m_bridge.GetVkDevice(), &renderPassInfo, nullptr,
                         &m_renderpass) != VK_SUCCESS) {
    return false;
  }
  return true;
}

bool Pipeline::create_pipeline(Context &ctx) {
  VkPipelineShaderStageCreateInfo shaderStages[] = {ctx.vertex_shader,
                                                    ctx.fragment_shader};

  ctx.pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  ctx.pipeline_info.stageCount = std::size(shaderStages);
  ctx.pipeline_info.pStages = shaderStages;
  ctx.pipeline_info.layout = m_pipeline_layout;
  ctx.pipeline_info.renderPass = m_renderpass;
  ctx.pipeline_info.subpass = 0;
  ctx.pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

  // 将和窗口大小相关的属性，设置为Dynamic，这样窗口大小改变时，
  // 不需要重新创建pipe line，只需要更新对应属性即可。
  std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,   // 视口动态
    VK_DYNAMIC_STATE_SCISSOR     // 裁剪矩形动态
  };
  VkPipelineDynamicStateCreateInfo dynamicStateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
    .pDynamicStates = dynamicStates.data()
  };
  ctx.pipeline_info.pDynamicState = &dynamicStateInfo;

  if (vkCreateGraphicsPipelines(m_bridge.GetVkDevice(), VK_NULL_HANDLE, 1,
                                &ctx.pipeline_info, nullptr,
                                &m_graphics_pipeline) != VK_SUCCESS) {
    return false;
  }

  return true;
}

void Pipeline::destroy_context(Context &ctx) {
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

bool Pipeline::create_shader_module(char *code, int length,
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

void Pipeline::create_uniform_buffers() {
  VkDeviceSize bufferSize = sizeof(UniformBufferObject);

  int size = m_bridge.GetSwapChain().Size();

  for (size_t i = 0; i < size; i++) {
    m_arr_uniform_buffers.push_back(vulkan::Buffer(m_bridge));
    m_arr_uniform_buffers[i].CreateBuffer(
        bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  }
}

void Pipeline::UpdateUniformBuffer(uint32_t currentImage,
                                   VkCommandBuffer command_buffer) {
  // static auto startTime = std::chrono::high_resolution_clock::now();
  vulkan::SwapChain &swapchain = m_bridge.GetSwapChain();

  // auto currentTime = std::chrono::high_resolution_clock::now();
  // float time = std::chrono::duration<float, std::chrono::seconds::period>(
  //                  currentTime - startTime)
  //                  .count();

  UniformBufferObject ubo{};
  // ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
  //                         glm::vec3(0.0f, 0.0f, 1.0f));
  // ubo.view =
  //     glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
  //                 glm::vec3(0.0f, 0.0f, 1.0f));
  // ubo.proj = glm::perspective(
  //     glm::radians(45.0f),
  //     swapchain.Extent2D().width / (float)swapchain.Extent2D().height,
  //     0.1f, 10.0f);
  // ubo.proj[1][1] *= -1;

  // ubo.ortho = glm::mat4(1.0);
  ubo.ortho =
      glm::orthoLH<float>(0, (float)swapchain.Extent2D().width, 0,
                          (float)swapchain.Extent2D().height, -2000.f, 2000.f);
  // ubo.orth = PrepareOrthographicProjectionMatrix(0,
  // swapchain.Extent2D().width, swapchain.Extent2D().height, 0, -2000.f,
  // 2000.f);

  void *data;
  vkMapMemory(m_bridge.GetVkDevice(),
              m_arr_uniform_buffers[currentImage].memory(), 0, sizeof(ubo), 0,
              &data);
  memcpy(data, &ubo, sizeof(ubo));
  vkUnmapMemory(m_bridge.GetVkDevice(),
                m_arr_uniform_buffers[currentImage].memory());

  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = m_arr_uniform_buffers[currentImage].handle();
  bufferInfo.range = sizeof(UniformBufferObject);

  VkWriteDescriptorSet descriptorWrites[1] = {};
  descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrites[0].dstSet = m_arr_descriptor_sets[currentImage];
  descriptorWrites[0].dstBinding = 0;
  descriptorWrites[0].dstArrayElement = 0;
  descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrites[0].descriptorCount = 1;
  descriptorWrites[0].pBufferInfo = &bufferInfo;
  vkUpdateDescriptorSets(m_bridge.GetVkDevice(), std::size(descriptorWrites),
                         descriptorWrites, 0, nullptr);

  vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          layout(), 0, 1, &m_arr_descriptor_sets[currentImage],
                          0, nullptr);
}

void Pipeline::UpdatePushData(VkCommandBuffer &command_buffer,
                              glm::mat4 &mat4) {
  PushData position;
  position.model = mat4;

  vkCmdPushConstants(
      command_buffer,             // 当前命令缓冲区
      m_pipeline_layout,          // 管线布局（需包含 Push Constants 范围）
      VK_SHADER_STAGE_VERTEX_BIT, // 生效的着色器阶段（这里是顶点着色器）
      0,                          // 偏移量（如果 Push Constants 块有多个变量）
      sizeof(PushData),           // 数据大小
      &position                   // 数据指针
  );
}

} // namespace vulkan