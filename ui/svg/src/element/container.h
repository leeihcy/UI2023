#ifndef _UI_SVG_SRC_ELEMENT_CONTAINER_H_
#define _UI_SVG_SRC_ELEMENT_CONTAINER_H_

#include "node.h"

class Container : public Node {
public:
  void AddChild(std::unique_ptr<Node> p);

public:
  void Render(RenderContext& context) override;
  void SetAttribute(const char* key, const char* value) override;
public:
  std::vector<std::unique_ptr<Node>> m_children;
};

#endif
