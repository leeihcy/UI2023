#include "vulkan_pipe_line.h"
#include "src/vulkan/vkapp.h"
#include "vulkan/vulkan_core.h"
#include "vulkan_device_queue.h"

#include <_types/_uint32_t.h>
#include <array>
#include <fstream>
#include <vulkan/vulkan.h>

namespace vulkan {

Pipeline::Pipeline(IVulkanBridge& bridge) : m_bridge(bridge) {}

Pipeline::~Pipeline() { Destroy(); }

bool Pipeline::Initialize(uint32_t w, uint32_t h, VkFormat format) {
  if (!create_graphics_pipeline(w, h, format)) {
    return false;
  }
  return true;
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
}

bool Pipeline::create_graphics_pipeline(uint32_t w, uint32_t h, VkFormat format) {
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
    if (!build_layout())
      break;
    if (!create_renderpass(format)) 
      break;
    if (!create_pipeline(context))
      break;

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
void Pipeline::build_vertex_input(Context &ctx,
                                  ShaderVertex shader_vertex) {
  ctx.vertex_input.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

#if 1
  ctx.vertex_input.vertexBindingDescriptionCount = 0;
  ctx.vertex_input.vertexAttributeDescriptionCount = 0;
#else
  ctx.vertex_input_binding_description.binding = 0;
  ctx.vertex_input_binding_description.stride = sizeof(shader_vertex);
  ctx.vertex_input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  ctx.vertex_input.vertexBindingDescriptionCount = 1;
  ctx.vertex_input.pVertexBindingDescriptions = &ctx.vertex_input_binding_description;

  ctx.vertex_input_attribute_descriptions[0].binding = 0;
  ctx.vertex_input_attribute_descriptions[0].location = 0;
  ctx.vertex_input_attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  ctx.vertex_input_attribute_descriptions[0].offset = offsetof(ShaderVertex, pos);
  ctx.vertex_input_attribute_descriptions[1].binding = 0;
  ctx.vertex_input_attribute_descriptions[1].location = 1;
  ctx.vertex_input_attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  ctx.vertex_input_attribute_descriptions[1].offset = offsetof(ShaderVertex, color);
  // ctx.vertex_input_attribute_descriptions[2].binding = 0;
  // ctx.vertex_input_attribute_descriptions[2].location = 2;
  // ctx.vertex_input_attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
  // ctx.vertex_input_attribute_descriptions[2].offset = offsetof(ShaderVertex, texCoord);

  ctx.vertex_input.vertexAttributeDescriptionCount =
      std::size(ctx.vertex_input_attribute_descriptions);
  ctx.vertex_input.pVertexAttributeDescriptions = ctx.vertex_input_attribute_descriptions;
#endif
}

void Pipeline::build_input_assembly(Context &ctx) {
  ctx.input_assembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  ctx.input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  ctx.input_assembly.primitiveRestartEnable = VK_FALSE;
}

bool Pipeline::build_vertex_shader(Context &ctx) {
  std::vector<char> vertShaderCode;
  if (!read_spv_file("shaders/vert.spv", vertShaderCode)) {
    return false;
  }
  if (!create_shader_module(vertShaderCode.data(), vertShaderCode.size(),
                            &ctx.vertex_shader.module)) {
    return false;
  }

  ctx.vertex_shader.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  ctx.vertex_shader.stage = VK_SHADER_STAGE_VERTEX_BIT;
  ctx.vertex_shader.pName = "main";
  return true;
}

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

  ctx.viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  ctx.viewport_state.viewportCount = 1;
  ctx.viewport_state.pViewports = &ctx.viewport;
  ctx.viewport_state.scissorCount = 1;
  ctx.viewport_state.pScissors = &ctx.scissor;
}

void Pipeline::build_rasterization(Context &ctx) {
  ctx.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  ctx.rasterizer.depthClampEnable = VK_FALSE;
  ctx.rasterizer.rasterizerDiscardEnable = VK_FALSE;
  ctx.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  ctx.rasterizer.lineWidth = 1.0f;
  ctx.rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  ctx.rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
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
  if (!create_shader_module(fragShaderCode.data(), fragShaderCode.size(),
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
  ctx.color_blend_attachment.blendEnable = VK_FALSE;

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

bool Pipeline::build_layout() {
   
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pushConstantRangeCount = 0;

  if (vkCreatePipelineLayout(m_bridge.GetVkDevice(), &pipelineLayoutInfo, nullptr,
                             &m_pipeline_layout) != VK_SUCCESS) {
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

  if (vkCreateRenderPass(m_bridge.GetVkDevice(), &renderPassInfo, nullptr, &m_renderpass) !=
      VK_SUCCESS) {
    return false;
  }
  return true;
}

bool Pipeline::create_pipeline(Context &ctx) {
  VkPipelineShaderStageCreateInfo shaderStages[] = {
    ctx.vertex_shader,
    ctx.fragment_shader};

  ctx.pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  ctx.pipeline_info.stageCount = std::size(shaderStages);
  ctx.pipeline_info.pStages = shaderStages;
  ctx.pipeline_info.layout = m_pipeline_layout;
  ctx.pipeline_info.renderPass = m_renderpass;
  ctx.pipeline_info.subpass = 0;
  ctx.pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

  if (vkCreateGraphicsPipelines(m_bridge.GetVkDevice(), VK_NULL_HANDLE, 1,
                                &ctx.pipeline_info, nullptr,
                                &m_graphics_pipeline) != VK_SUCCESS) {
    return false;
  }

  return true;
}

void Pipeline::destroy_context(Context &ctx) {
  if (ctx.vertex_shader.module != VK_NULL_HANDLE) {
    vkDestroyShaderModule(m_bridge.GetVkDevice(),
                          ctx.vertex_shader.module, nullptr);
    ctx.vertex_shader.module = VK_NULL_HANDLE;
  }

  if (ctx.fragment_shader.module != VK_NULL_HANDLE) {
    vkDestroyShaderModule(m_bridge.GetVkDevice(),
                          ctx.fragment_shader.module, nullptr);
    ctx.fragment_shader.module = VK_NULL_HANDLE;
  }
}

bool Pipeline::create_shader_module(char *code, int length,
                                    VkShaderModule *out_module) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = length;
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code);

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(m_bridge.GetVkDevice(), &createInfo, nullptr,
                           out_module) != VK_SUCCESS) {
    return false;
  }

  return true;
}

} // namespace vulkan