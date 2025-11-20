#ifndef _UI_GPU_SRC_METAL2_METAL2_PIPELINE_H_
#define _UI_GPU_SRC_METAL2_METAL2_PIPELINE_H_

#include "src/metal2/inc.h"
#include "src/metal2/metal2_bridge.h"

namespace metal2 {
class PipeLine {
public:
  PipeLine(ui::IMetal2Bridge& bridge);

  bool Create();
  bool Destroy();
private:
  id<MTLLibrary> loadShaderLibrary();
  
private:
  ui::IMetal2Bridge& m_bridge;
public:
  id <MTLRenderPipelineState> m_state = nil;
};

}

#endif