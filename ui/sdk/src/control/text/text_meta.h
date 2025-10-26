#ifndef _UI_SDK_SRC_CONTROL_TEXT_TEXT_META_H_
#define _UI_SDK_SRC_CONTROL_TEXT_TEXT_META_H_

#include "include/interface/imeta.h"
#include "include/interface/icontrol.h"

namespace ui {

struct LabelMeta : public MetaImpl<ILabel> {
  static LabelMeta& Get() {
    static LabelMeta s;
    return s;
  }

  Uuid UUID() override { return {0x74D8EF7F, 0x695E, 0x11F0, {0xAB, 0xAB, 0xE8, 0xB0, 0xC5, 0x68, 0x09, 0x7E}}; }

  const char* Name() override { return "label"; }

  MetaDetail Detail() override {
    MetaDetail param;
    param.major_type = OBJ_CONTROL;
    param.minor_type = CONTROL_LABEL;
    param.category = CATEGORY_CONTROL;
    param.descrption = "文本标签";
    param.default_size = {100, 20};
    return param;
  }
};

} // namespace ui

#endif