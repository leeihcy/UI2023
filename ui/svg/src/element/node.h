#ifndef _UI_SVG_SRC_ELEMENT_NODE_H_
#define _UI_SVG_SRC_ELEMENT_NODE_H_
#include "SkCanvas.h"
#include "render_context.h"

class Node {
public:
  virtual ~Node() {}
  virtual void Render(RenderContext& context);
  virtual void SetAttribute(const char* key, const char* value);
};

#endif
