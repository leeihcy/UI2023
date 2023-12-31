#ifndef _UI_SVG_SRC_ELEMENT_GROUP_GROUP_H_
#define _UI_SVG_SRC_ELEMENT_GROUP_GROUP_H_

#include "svg/src/element/element.h"

namespace svg {

class Group : public Element {
public:
  void AddChild(std::unique_ptr<Element>) override;
  Element* FindElementById(const char* id) override;
  void RemoveAll();

public:
  void Render(RenderContext& context) override;
  void SetAttribute(ui::SerializeParam& data) override;

public:
  std::vector<std::unique_ptr<Element>> m_children;
};

}
#endif