#ifndef _UI_GPU_SRC_VULKAN_WRAP_VULKAN_COMMAND_BUFFER_H_
#define _UI_GPU_SRC_VULKAN_WRAP_VULKAN_COMMAND_BUFFER_H_
#include <vulkan/vulkan.h>

namespace vulkan {
class DeviceQueue;
class CommandPool;
struct IVulkanBridge;

class CommandBuffer {
public:
  CommandBuffer(IVulkanBridge& bridge, bool primary=true);
  ~CommandBuffer();

  enum RecordType {
    // Nothing has been recorded yet.
    RECORD_TYPE_EMPTY,

    // Recorded for single use, will be reset upon submission.
    RECORD_TYPE_SINGLE_USE,

    // Recording for multi use, once submitted it can't be modified until reset.
    RECORD_TYPE_MULTI_USE,

    // Recorded for multi-use, can no longer be modified unless reset.
    RECORD_TYPE_RECORDED,

    // Dirty, should be cleared before use. This assumes its externally
    // synchronized and the command buffer is no longer in use.
    RECORD_TYPE_DIRTY,
  };

public:
  bool Initialize();
  void Destroy();

  VkCommandBuffer handle() { return m_command_buffer; }
  
  void Reset();
  void BeginRecordCommand();
  void EndRecordCommand();
  
  void BeginRenderPass(VkFramebuffer framebuffer);
  void BindPipeline(VkPipeline pipe_line);
  void EndRenderPass();

protected:
  bool alloc();  // 由CommandPool调用。

public:
  //   bool Submit(uint32_t num_wait_semaphores,
  //               VkSemaphore* wait_semaphores,
  //               uint32_t num_signal_semaphores,
  //               VkSemaphore* signal_semaphores);

  //   void Enqueue(VkCommandBuffer primary_command_buffer);
  //   void Clear();
  //   void Wait(uint64_t timeout);
  // bool SubmissionFinished();

  //   void TransitionImageLayout(
  //       VkImage image,
  //       VkImageLayout old_layout,
  //       VkImageLayout new_layout,
  //       uint32_t src_queue_family_index = VK_QUEUE_FAMILY_IGNORED,
  //       uint32_t dst_queue_family_index = VK_QUEUE_FAMILY_IGNORED);
  //   void CopyBufferToImage(VkBuffer buffer,
  //                          VkImage image,
  //                          uint32_t buffer_width,
  //                          uint32_t buffer_height,
  //                          uint32_t width,
  //                          uint32_t height,
  //                          uint64_t buffer_offset = 0);
  //   void CopyImageToBuffer(VkBuffer buffer,
  //                          VkImage image,
  //                          uint32_t buffer_width,
  //                          uint32_t buffer_height,
  //                          uint32_t width,
  //                          uint32_t height,
  //                          uint64_t buffer_offset = 0);

private:
  const bool m_is_primary;
  bool m_is_recording = false;
  RecordType record_type_ = RECORD_TYPE_EMPTY;
  IVulkanBridge& m_bridge; // raw_ptr<

  VkCommandBuffer m_command_buffer = VK_NULL_HANDLE;
  // VulkanFenceHelper::FenceHandle submission_fence_;

  friend class CommandPool;
};

} // namespace vulkan

#endif