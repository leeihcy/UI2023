#include "src/metal2/metal2_swapchain.h"
#include "src/metal2/metal2_pipeline.h"
#include "src/metal2/metal2_objects.h"
#include "src/util.h"

namespace metal2 {

InFlightFrame::InFlightFrame(ui::IMetal2Bridge& bridge) : m_bridge(bridge) {

}
InFlightFrame::~InFlightFrame() {

}

void InFlightFrame::Initialize() {

}

void InFlightFrame::Destroy() {
  // if (m_commandBuffer) {
  //   [m_commandBuffer release];
  //   m_commandBuffer = nullptr;
  // }
  //  if (m_renderEncoder) {
  //   [m_renderEncoder release];
  //   m_renderEncoder = nullptr;
  // }
}

void InFlightFrame::onCommandComplete(id<MTLCommandBuffer> command_buffer) {
  m_bridge.GetSwapchain().SignalSemaphore();
}
bool InFlightFrame::BeginCommit(MTLRenderPassDescriptor* desc) {
  // command buffer不能复用，metal内部有自己的复用逻辑。
  m_commandBuffer = [m_bridge.GetCommandQueue() commandBuffer];
  [m_commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {
    onCommandComplete(commandBuffer);
  }];

  // render encoder不能复用。需要每帧创建一次。
  metal2::RenderPass& render_pass = m_bridge.GetRenderPass();
  m_renderEncoder = [m_commandBuffer
      renderCommandEncoderWithDescriptor:desc];

  return true;
}

bool InFlightFrame::EndCommit(id<CAMetalDrawable> drawable) {
  // 一旦 renderEncoder 调用 endEncoding() 或命令缓冲区提交后，编码器就变为不可用状态。
  [m_renderEncoder endEncoding];
  [m_commandBuffer presentDrawable:drawable];
  [m_commandBuffer commit];

  return true;
}

// ----------------------------------------

SwapChain::SwapChain(ui::IMetal2Bridge& bridge) : m_bridge(bridge) {

}

bool SwapChain::Create() {
  if (!initInFlightFrames()) {
    return false;
  }

  m_semaphore = dispatch_semaphore_create( MAX_FRAMES_IN_FLIGHT );

  return true;
}

bool SwapChain::initInFlightFrames() {
  if (m_inflight_frames.empty()) {
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      std::unique_ptr<InFlightFrame> item = std::make_unique<InFlightFrame>(m_bridge);
      item->Initialize();
      m_inflight_frames.push_back(std::move(item));
    }
  }
  return true;
}

InFlightFrame *SwapChain::GetCurrentInflightFrame() {
  assert(m_current_inflight_frame_index < m_inflight_frames.size());
  return m_inflight_frames[m_current_inflight_frame_index].get();
}

void SwapChain::IncCurrentInflightFrame() {
  m_current_inflight_frame_index = (m_current_inflight_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
}

bool SwapChain::WaitforNextFrame() {
  // 等待信号量，确保不会超过_maxFramesInFlight帧同时处理

  // 避免阻塞渲染线程，首先使用0ms进行尝试。
  const int retry_times = 5;
  uint64_t timeout[retry_times] = {
      0,
      10 * 1000 * 1000,   // 10ms
      100 * 1000 * 1000,  // 100ms
      1000 * 1000 * 1000, // 1m,
      DISPATCH_TIME_FOREVER,
  };

  int ret = -1;
  for (int i = 0; i < retry_times; i++) {
    ret = dispatch_semaphore_wait(m_semaphore, timeout[i]);
    if (ret == 0) {
      break;
    }

    if (i != 0) {
      ui::Log("dispatch_semaphore_wait timeout at index: %d", i);
      continue;
    }
  }
  if (ret != 0) {
    ui::Log("dispatch_semaphore_wait timtout");
    return false;
  }

  return true;
}

void SwapChain::SignalSemaphore() {
  dispatch_semaphore_signal(m_semaphore);
}
}