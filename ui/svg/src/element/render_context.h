#ifndef _UI_SVG_SRC_ELEMENT_RENDER_CONTEXT_H_
#define _UI_SVG_SRC_ELEMENT_RENDER_CONTEXT_H_

#include "SkCanvas.h"
#include "SkPaint.h"
#include "sdk/include/util/rect.h"

namespace ui {
  struct Length;
}

struct RenderContext {
  SkCanvas* canvas;
  SkPaint paint;
  
  // 控件大小（画布大小）
  ui::Size canvas_size;

  // dom内容基于该区域进行作图
  // 例如 x="50%",是按view_box.width 50%算的，不是svg_size 50%
  ui::RectXYWH view_box;

  int ResolveX(ui::Length& x);
  int ResolveY(ui::Length& y);
  int ResolveRX(ui::Length& rx);
  int ResolveRY(ui::Length& ry);
};

#endif