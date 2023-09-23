#ifndef _UI_SVG_SRC_ELEMENT_SVG_ROOT_H_
#define _UI_SVG_SRC_ELEMENT_SVG_ROOT_H_

#include "src/element/group/group.h"

// https://developer.mozilla.org/en-US/docs/Web/SVG/Element/svg
//
// The svg element is a container that defines a new coordinate system and
// viewport. It is used as the outermost element of SVG documents, but it can
// also be used to embed an SVG fragment inside an SVG or HTML document.

namespace svg {

class Svg : public Group {
public:
  void Render(RenderContext &context) override;
  void SetAttribute(ui::SerializeParam &data) override;

  ui::Size GetDesiredSize();
private:
  bool prepareToRender(RenderContext &context);
  
  void calcViewPort(RenderContext &context);
  SkMatrix calcViewBoxMatrix(RenderContext &context);

  ui::Size getViewPortWithDefault();

private:
  ui::Length m_width;
  ui::Length m_height;

  // viewBox
  // 所有子元素都是基于这个坐标进行绘制的，而不是svg控件大小
  ui::RectFXYWH m_view_box;
  bool m_view_box_configed = false;
};

} // namespace svg

#endif