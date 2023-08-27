#ifndef _UI_SVG_SRC_ELEMENT_CONTAINER_H_
#define _UI_SVG_SRC_ELEMENT_CONTAINER_H_

#include "element.h"

class Container : public Element {
public:
  void AddChild(std::unique_ptr<Element>) override;
  void RemoveAll();

public:
  void Render(RenderContext& context) override;
  void SetAttribute(ui::SerializeParam& data) override;

public:
  std::vector<std::unique_ptr<Element>> m_children;
};

#endif
