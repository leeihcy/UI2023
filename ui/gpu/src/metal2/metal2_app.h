#ifndef _UI_GPU_SRC_METAL2_METAL2APP_H_
#define _UI_GPU_SRC_METAL2_METAL2APP_H_
#include "src/metal2/import.h"

namespace ui {

class Metal2Application {
public:
  static Metal2Application& GetInstance();
public:
  bool Startup();
  void Shutdown();

private:
  bool createDevice();

public:
};

}
#endif