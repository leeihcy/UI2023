#pragma once
#include "include/interface/irect.h"
#include "sdk/include/interface/imeta.h"
#include "src/private_inc.h"

namespace ui {
namespace svg {

struct RectMeta : MetaImpl<IRect> {
  static RectMeta& Get() {
    static RectMeta s;
    return s;
  }
  Uuid UUID() override { return {0xDF69247A, 0x3E8A, 0x11EE, {0xBC, 0x0B, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }

  const wchar_t* Name() override { return L"Rect"; }

  MetaDetail Detail() override {
    MetaDetail param;
    param.major_type = OBJ_CONTROL;
    param.minor_type = CONTROL_SVG;
    param.category = CATEGORY_SVG;
    param.descrption = L"svg rect element";
    param.default_size = {100, 40};
    return param;
  }
};

} // namespace svg
} // namespace ui