#ifndef _UI_SVG_SRC_ELEMENT_ELEMENT_H_
#define _UI_SVG_SRC_ELEMENT_ELEMENT_H_
// #include "SkCanvas.h"
#include "svg/src/element/render_context.h"
#include "sdk/include/macro/uidefine.h"
#include "sdk/include/util/color.h"

namespace svg {
  
class Element {
public:
  virtual ~Element() {}
  virtual void PrepareRender(RenderContext& context);
  virtual void Render(RenderContext& context);
  virtual void SetAttribute(ui::SerializeParam& data);
  virtual void SetXmlNodeData(const char* data) {};
  virtual void AddChild(std::unique_ptr<Element>) {}
  virtual Element* FindElementById(const char* id) { return nullptr; }

public:
  void SetParent(Element* p);
  Element* GetRootElement();

  const std::string& GetId();

private:
  void SetFillColor(ui::Color c);
  ui::Color GetFillColor();
  void SetTransform(const char*);
  const char* GetTransform();

  void setId(const char*);
  const char* getId();

protected:
  Element* m_parent = nullptr; // raw ptr

  std::string m_id;

  std::unique_ptr<SkMatrix>  m_matrix;

  // 如何表示没有配置该属性的场景
  std::unique_ptr<ui::Color> m_fill;

  // fill
  // fill-opacity
  // fill-rule
  //
  ui::Color m_stroke;
  ui::Length m_stroke_width;
  // stroke-opacity
  // stroke-linecap
  // stroke-linejoin
  // stroke-dasharray
  // stroke-dashoffset
  // stroke-miterlimit
  // 
};

}
#endif
