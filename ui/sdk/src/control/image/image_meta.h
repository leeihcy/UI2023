#ifndef _UI_SDK_SRC_CONTROL_IMAGE_IMAGE_META_H_
#define _UI_SDK_SRC_CONTROL_IMAGE_IMAGE_META_H_

#include "include/interface/imeta.h"
#include "include/interface/icontrol.h"

namespace ui {

struct ImageMeta : public MetaImpl<IImage> {
  static ImageMeta& Get() {
    static ImageMeta s;
    return s;
  }

  Uuid UUID() override { return {0xA85C7B20, 0xB212, 0x11F0, {0x97, 0x4E, 0xE8, 0xB0, 0xC5, 0x68, 0x09, 0x7E}}; }

  const char* Name() override { return "img"; }

  MetaDetail Detail() override {
    MetaDetail param;
    param.major_type = OBJ_CONTROL;
    param.minor_type = CONTROL_IMAGE;
    param.category = CATEGORY_CONTROL;
    param.descrption = "静态图片";
    param.default_size = {100, 20};
    return param;
  }
};

} // namespace ui

#endif