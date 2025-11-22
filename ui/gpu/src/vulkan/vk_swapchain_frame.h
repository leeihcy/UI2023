#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SWAP_CHAIN_IMAGE_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_SWAP_CHAIN_IMAGE_H_
#include "src/vulkan/vk_bridge.h"
#include "src/vulkan/vk_objects.h"
#include "src/vulkan/vk_buffer.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace vulkan {

// swapchain中的一帧。
class SwapChainFrame {
public:
  explicit SwapChainFrame(IVulkanBridge& bridge, VkImage swapchain_images);
  ~SwapChainFrame();

  bool Create(VkFormat imageFormat);
  bool CreateFrameBuffer(int width, int height, VkRenderPass render_pass);
  bool CreateFrameDataUniformBuffer();

  void UpdateUniformBuffer(uint32_t currentImage, VkCommandBuffer command_buffer);
  void UpdatePushData(VkCommandBuffer &command_buffer, glm::mat4 &mat4);

public:
  IVulkanBridge& m_bridge;

  // swap chain中创建的image，不需要释放
  // 画布。存储像素数据的内存块。
  VkImage m_swapchain_image_ref = VK_NULL_HANDLE;

  // 画布的一个特定视图，它定义了是用整块画布，还是只用画布的一个角落。
  Vk::ImageView m_image_view;

  // frame buffer是一个包含了多个图像的集合，这此图像可是以颜色、深度或模板缓冲区。
  // 在这里，我们只包含一个color attachment (image view).
  //
  // 需要为SwapChain中的每一个Image创建一个FrameBuffer。
  Vk::Framebuffer m_frame_buffer;

  //
  // 一个 descriptor set 中可以包含多个 descriptor，相当于是一个数组。
  // 一个 descriptor 指向一个缓存或纹理。
  //
  // vulkan shader 不支持绑定单个descriptor，只能绑定descriptor set
  //
  // https://vkguide.dev/docs/chapter-4/descriptors/
  // 一些设备只支持最多绑定4个descriptor set，因此我们可以按绑定频率进行分层：
  // The descriptor set number 0 will be used for engine-global resources, 
  // and bound once per frame. 
  // The descriptor set number 1 will be used for per-pass resources, 
  // and bound once per pass. 
  // The descriptor set number 2 will be used for material resources, 
  // and the number 3 will be used for per-object resources.
  // This way, the inner render loops will only be binding descriptor 
  // sets 2 and 3, and performance will be high.
  //
  // 四个描述符集的层次结构
  // 第0层：引擎全局资源 (每帧绑定一次)
  // 绑定频率：每帧一次
  // 包含内容：
  // - 帧计数器
  // - 全局光照参数
  // - 相机参数（如果相机每帧更新）
  // - 全局渲染设置
  //
  // 第1层：每通道资源 (每个渲染通道绑定一次)
  // 绑定频率：每个渲染通道一次
  // 包含内容：
  // - 阴影贴图
  // - G-Buffer
  // - 后期处理参数
  // - 通道特定的uniform缓冲区
  //
  // 第2层：材质资源 (每个材质绑定一次)
  // 绑定频率：每个材质一次
  // 包含内容：
  // - 漫反射纹理
  // - 法线贴图
  // - 金属度/粗糙度贴图
  // - 材质参数（颜色、反射率等）
  //
  // 第3层：每个对象资源 (每个对象绑定一次)
  // 绑定频率：每个对象一次
  // 包含内容：
  // - 模型矩阵
  // - 骨骼动画数据
  // - 对象特定的参数
  // 

  // 帧级别的资源集合。
  VkDescriptorSet m_frame_descriptorset;

  vulkan::Buffer m_framedata_buffer;
};

} // namespace vulkan

#endif