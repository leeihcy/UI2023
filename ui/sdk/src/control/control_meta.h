#pragma once
#include "include/interface/imeta.h"
#include "include/interface/icontrol.h"

namespace ui {

struct ControlMeta : public MetaImpl<IControl> {
  static ControlMeta& Get() {
    static ControlMeta s;
    return s;
  }

  Uuid UUID() override { return {0x79B56914, 0xA176, 0x11EE, {0x91, 0x81, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }

  const char* Name() override { return "Control"; }

  MetaDetail Detail() override {
    MetaDetail param;
    param.major_type = OBJ_CONTROL;
    param.minor_type = CONTROL_UNKNOWN;
    param.category = CATEGORY_CONTROL;
    param.descrption = "所有控件基类";
    param.default_size = {100, 100};
    return param;
  }
};

} // namespace ui