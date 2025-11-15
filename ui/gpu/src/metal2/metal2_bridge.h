#ifndef _UI_GPU_SRC_METAL2_METAL2BRIDGE_H_
#define _UI_GPU_SRC_METAL2_METAL2BRIDGE_H_

#import "src/metal2/import.h"

namespace ui {

struct IMetal2Bridge {
  virtual id<MTLDevice> GetMetalDevice() = 0;
}; 

}

#endif