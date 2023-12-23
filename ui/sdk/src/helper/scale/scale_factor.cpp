#include "scale_factor.h"
#include "include/util/rect.h"
#include "src/object/object.h"
#include "src/window/window.h"
#include "include/util/math.h"

namespace ui {

float ScaleFactorHelper::GetObjectScaleFactor(Object* context) {
  if (!context) {
    return SCALE_FACTOR_1;
  }
  // 冒泡获取？太慢了。
  Window* window = context->GetWindow();
  if (!window) {
    return SCALE_FACTOR_1;
  }
  return window->GetScaleFactor();
}

int ScaleFactorHelper::ScaleIfGt0(int value, Object* context) {
  float scale_factor = GetObjectScaleFactor(context);
  if (scale_factor == SCALE_FACTOR_1) {
    return value;
  }

  if (value <= 0)
    return value;

  return _round(value * scale_factor);
}

Size ScaleFactorHelper::Scale(Size s, Object* context) {
  float scale_factor = GetObjectScaleFactor(context);
  if (scale_factor == SCALE_FACTOR_1) {
    return s;
  }

  Size ret;
  ret.width = _round(s.width * scale_factor);
  ret.height = _round(s.height * scale_factor);
  return ret;
}

}