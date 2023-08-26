#ifndef _UI_SVG_SRC_SVG_SVG_META_H_
#define _UI_SVG_SRC_SVG_SVG_META_H_

#include "include/interface/isvg.h"
#include "sdk/include/interface/imeta.h"
#include "src/private_inc.h"

namespace ui {
namespace svg {

class SvgMeta : public MetaImpl<ISvg> {
public:
  static SvgMeta& Get() {
    static SvgMeta s;
  return s;
  }

  Uuid UUID() override { return {0x4AD17FBE, 0x3E8B, 0x11EE, {0xB9, 0x0E, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }

  const char* Name() override { return "Svg"; }

  MetaDetail Detail() override {
    MetaDetail param;
    param.major_type = OBJ_CONTROL;
    param.minor_type = CONTROL_SVG;
    param.category = CATEGORY_SVG;
    param.descrption = "svg svg element";
    param.default_size = {200, 20};
    return param;
  }
};

} // namespace svg
} // namespace ui
#endif