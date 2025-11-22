#ifndef _UI_GPU_SRC_METAL2_METAL2_SWAPCHAIN_QUEUE_H_
#define _UI_GPU_SRC_METAL2_METAL2_SWAPCHAIN_QUEUE_H_

#include "src/metal2/inc.h"
#include "src/metal2/metal2_bridge.h"
#include <memory>
#include <vector>

namespace metal2 {

// CPU最多提交可以提交多少帧给GPU
// 每套InFlight Frame都需要一套独立的资源，如CommandBuffer、UniformBuffer、DescriptorSets。
// 设置为2的时候，很容易出现cpu等待gpu。修改为3
#define MAX_FRAMES_IN_FLIGHT 3

// Inflight Frames：
// ✅ CPU 提交速度限制器（防止 GPU 过载）
// ✅ 流量控制机制（保持流水线稳定）
// ❌ 不是数据复用机制
// ❌ 不能复用任何命令对象（Command Buffer/Encoder 必须新建）
class InFlightFrame {
public:
  InFlightFrame(ui::IMetal2Bridge& bridge);
  ~InFlightFrame();

  void Initialize();
  void Destroy();

  bool BeginCommit(MTLRenderPassDescriptor* desc);
  bool EndCommit(id<CAMetalDrawable> drawable);

private:
  void onCommandComplete(id<MTLCommandBuffer> command_buffer);
  
public:
  ui::IMetal2Bridge& m_bridge;

  // auto release
  id<MTLCommandBuffer> m_commandBuffer = nil;
  id<MTLRenderCommandEncoder> m_renderEncoder = nil;
};

class SwapChain {
public:
  SwapChain(ui::IMetal2Bridge& bridge);
  bool Create();

  InFlightFrame *GetCurrentInflightFrame();
  void IncCurrentInflightFrame();

  bool WaitforNextFrame();
  void SignalSemaphore();

private:
  bool initInFlightFrames();

private:
  ui::IMetal2Bridge& m_bridge;

  // 信号量，限制并发MAX_FRAMES_IN_FLIGHT
  dispatch_semaphore_t m_semaphore;

  // size == MAX_FRAMES_IN_FLIGHT
  // 限制CPU最大并发，避免向GPU提交太多Frame
  std::vector<std::unique_ptr<InFlightFrame>> m_inflight_frames;
  uint32_t m_current_inflight_frame_index = 0;
};

}

#endif