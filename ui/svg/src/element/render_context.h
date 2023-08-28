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
  ui::Size svg_size;
  ui::Size view_port_size;

  int ResolveX(ui::Length& x);
  int ResolveY(ui::Length& y);
  int ResolveRX(ui::Length& rx);
  int ResolveRY(ui::Length& ry);
};

#endif