#ifndef _UI_SVG_SRC_ELEMENT_SVG_ROOT_H_
#define _UI_SVG_SRC_ELEMENT_SVG_ROOT_H_

#include "src/element/container.h"

// https://developer.mozilla.org/en-US/docs/Web/SVG/Element/svg
//
// The svg element is a container that defines a new coordinate system and
// viewport. It is used as the outermost element of SVG documents, but it can
// also be used to embed an SVG fragment inside an SVG or HTML document.

namespace svg {

class Svg : public Container {
public:
  void Render(RenderContext &context) override;
  void SetAttribute(ui::SerializeParam &data) override;

  ui::Size GetDesiredSize();

private:
  bool prepareToRender(RenderContext &context);

private:
  ui::Length m_width;
  ui::Length m_height;

  // viewBox
  ui::RectXYWH m_view_box = {-1,-1,-1,-1};
};

} // namespace svg

#endif