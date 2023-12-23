#ifndef _UI_SDK_SRC_HELPER_SCALE_SCALE_FACTOR_H_
#define _UI_SDK_SRC_HELPER_SCALE_SCALE_FACTOR_H_

#include "include/util/rect.h"

// 2023.12.23 重写DPI缩放逻辑。
// win10开始，支持每个屏幕不一样的缩放比例，因此DPI将不是一个固定的值。
//
namespace ui {
class Object;

#define SCALE_FACTOR_1 1.0f

class ScaleFactorHelper {
public:
  static int ScaleIfGt0(int value, Object* context);
  static Size Scale(Size s, Object* context);

public:
  static float GetObjectScaleFactor(Object* context);
};

}

#endif