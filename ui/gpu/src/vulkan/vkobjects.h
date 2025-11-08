#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_IMAGE_VIEW_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_IMAGE_VIEW_H_
#include <memory>
#include <vector>
#include <assert.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace vulkan {
class DeviceQueue;
class CommandBuffer;
struct IVulkanBridge;
}

#define VK_HANDLE2(ClassName, HandleType)                                      \
  namespace Vk {                                                               \
  class ClassName {                                                            \
    using HandleName = Vk##HandleType;                                         \
                                                                               \
  public:                                                                      \
    ~ClassName() { assert(handle == VK_NULL_HANDLE); }                        \
    void Destroy(VkDevice device) {                                            \
      if (handle && device) {                                                  \
        vkDestroy##HandleType(device, handle, nullptr);                        \
        handle = VK_NULL_HANDLE;                                               \
      }                                                                        \
    }                                                                          \
    operator HandleName &() { return handle; }                                 \
    HandleName *operator&() { return &handle; }                                \
    operator bool() { return handle != VK_NULL_HANDLE; }                       \
                                                                               \
    HandleName handle = VK_NULL_HANDLE;                                        \
  };                                                                           \
  } // namespace vulkan

#define VK_HANDLE(HandleType)  VK_HANDLE2(HandleType, HandleType)

VK_HANDLE(PipelineLayout);
VK_HANDLE(Pipeline);
VK_HANDLE(Sampler);
VK_HANDLE(DescriptorSetLayout);
VK_HANDLE(Image);
VK_HANDLE(Framebuffer);
VK_HANDLE(Fence);

VK_HANDLE2(ImageViewBase, ImageView);
VK_HANDLE2(RenderPassBase, RenderPass);
VK_HANDLE2(CommandPoolBase, CommandPool);
VK_HANDLE2(DescriptorPoolBase, DescriptorPool);

namespace Vk {
//
// 用于操作VkImage，例如带mipmapping的image
//
class ImageView : public Vk::ImageViewBase {
public:
  bool Create(VkDevice device, VkImage image, VkFormat image_format);
};

class RenderPass : public Vk::RenderPassBase {
public:
  bool Create(VkDevice device, VkFormat format);
};

class CommandPool : public Vk::CommandPoolBase  {
public:
  bool Create(vulkan::IVulkanBridge& bridge);

  VkCommandBuffer AllocateCommandBuffer(VkDevice device);
  void ReleaseCommandBuffer(VkDevice device, VkCommandBuffer command_buffer);
};

class DescriptorPool : public Vk::DescriptorPoolBase {
public:
  bool CreateUniformBufferPool(VkDevice device, uint32_t size);
  bool CreateTextureSamplePool(VkDevice device, uint32_t size);

  VkDescriptorSet AllocatateDescriptorSet(VkDevice device, VkDescriptorSetLayout layout);
  void FreeDescriptorSet(VkDevice device, VkDescriptorSet);

private:
  bool create(VkDevice device, VkDescriptorType type, uint32_t size);
};

class CommandBuffer {
public:
  ~CommandBuffer();
  operator VkCommandBuffer& () { return handle; }
  VkCommandBuffer* operator& () { return &handle; }

public:
  void Attach(VkCommandBuffer buffer);
  VkCommandBuffer Detach();
  
  void Reset();
  void BeginRecordCommand();
  void EndRecordCommand();

  void BeginRenderPass(VkFramebuffer framebuffer, VkRenderPass renderpass,
                       VkOffset2D offset, VkExtent2D extent);
  void BindPipeline(VkPipeline pipe_line);
  void EndRenderPass();

public:
  VkCommandBuffer handle = VK_NULL_HANDLE;
private:  
  bool m_is_recording = false;
};

}

#endif