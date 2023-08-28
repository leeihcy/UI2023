#ifndef _UI_SVG_SRC_ELEMENT_ELEMENT_H_
#define _UI_SVG_SRC_ELEMENT_ELEMENT_H_
#include "SkCanvas.h"
#include "render_context.h"
#include "sdk/include/macro/uidefine.h"
#include "sdk/include/util/color.h"

namespace svg {
  
class Element {
public:
  virtual ~Element() {}
  virtual void Render(RenderContext& context);
  virtual void SetAttribute(ui::SerializeParam& data);
  virtual void SetDomData(const char* data) {};
  virtual void AddChild(std::unique_ptr<Element>) {};
  

protected:
 
  ui::Color m_fill;
  // fill
  // fill-opacity
  // fill-rule
  //
  // stroke
  // stroke-opacity
  // stroke-linecap
  // stroke-width
  // stroke-linejoin
  // stroke-dasharray
  // stroke-dashoffset
  // stroke-miterlimit
  // 
};

}
#endif
