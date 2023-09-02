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
  ui::RectFXYWH view_port;

  // dom内容基于该区域进行作图
  // 例如 x="50%",是按view_box.width 50%算的，不是svg_size 50%
  ui::RectFXYWH view_box;

  // 缩放系数。
  // 不采用matrix进行缩放，会造成失败。直接将对所有原始参数进行缩放。
  float scale = 1.0;  
  float translate_x = 0;
  float translate_y = 0;

  float ResolveX(ui::Length& x);
  float ResolveY(ui::Length& y);
  float ResolveRX(ui::Length& rx);
  float ResolveRY(ui::Length& ry);
};

#endif