#ifndef _UI_SDK_SRC_CONTROL_BUTTON_BUTTON_META_H_
#define _UI_SDK_SRC_CONTROL_BUTTON_BUTTON_META_H_

#include "include/interface/imeta.h"
#include "include/interface/icontrol.h"

namespace ui {

struct ButtonMeta : public MetaImpl<IButton> {
  static ButtonMeta& Get() {
    static ButtonMeta s;
    return s;
  }

  Uuid UUID() override { return {0x5E0E807A, 0xA650, 0x11F0, {0xA2, 0x36, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }

  const char* Name() override { return "button"; }

  MetaDetail Detail() override {
    MetaDetail param;
    param.major_type = OBJ_CONTROL;
    param.minor_type = CONTROL_BUTTON;
    param.category = CATEGORY_CONTROL;
    param.descrption = "按钮";
    param.default_size = {100, 20};
    return param;
  }
};

} // namespace ui

#endif