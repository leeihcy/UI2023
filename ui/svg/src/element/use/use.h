#ifndef _UI_SVG_SRC_ELEMENT_USE_USE_H_
#define _UI_SVG_SRC_ELEMENT_USE_USE_H_

#include "src/element/element.h"

namespace svg {

class Use : public Element {
public:
  void PrepareRender(RenderContext &context) override;
  void Render(RenderContext& context) override;
  void SetAttribute(ui::SerializeParam& data) override;

private:
  void setHref(const char*);
  const char* getHref();

private:
  // 圆中心位置
  ui::Length m_x;
  ui::Length m_y;

  std::string m_href;
};

}
#endif