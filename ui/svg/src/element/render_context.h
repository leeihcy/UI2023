#ifndef _UI_SVG_SRC_ELEMENT_RENDER_CONTEXT_H_
#define _UI_SVG_SRC_ELEMENT_RENDER_CONTEXT_H_

#include "SkCanvas.h"
#include "SkPaint.h"

struct RenderContext {
  SkCanvas* canvas;
  SkPaint paint;
};

#endif