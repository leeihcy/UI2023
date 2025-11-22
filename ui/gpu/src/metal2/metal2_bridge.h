#ifndef _UI_GPU_SRC_METAL2_METAL2BRIDGE_H_
#define _UI_GPU_SRC_METAL2_METAL2BRIDGE_H_

#import "src/metal2/import.h"

namespace metal2 {
class PipeLine;
class RenderPass;
class SwapChain;
}

namespace ui {

struct IMetal2Bridge {
  virtual id<MTLDevice> GetMetalDevice() = 0;
  virtual id<MTLCommandQueue> GetCommandQueue() = 0;
  // virtual id<MTLRenderCommandEncoder> GetRenderEncoder() = 0;

  virtual metal2::PipeLine& GetPipeline() = 0;
  virtual metal2::RenderPass& GetRenderPass() = 0;
  virtual metal2::SwapChain& GetSwapchain() = 0;
}; 

}

#endif