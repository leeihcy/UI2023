#ifndef _UI_GPU_SRC_METAL2_METAL2OBJECTS_QUEUE_H_
#define _UI_GPU_SRC_METAL2_METAL2OBJECTS_QUEUE_H_

#include "src/metal2/inc.h"
#import "src/metal2/import.h"

namespace metal2 {

class CommandQueue {
public:
  bool Create();

};

class RenderPass {
public:
  bool Create();
  bool Destroy();

  void SetTexture(id<MTLTexture> texture);

  MTLRenderPassDescriptor* m_descriptor = nullptr;
};

class CommandBuffer {
public:

  bool Create(id<MTLCommandQueue>);
public:
  id<MTLCommandBuffer> m_command_buffer;
};


}

#endif