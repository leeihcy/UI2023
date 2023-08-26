#include "container.h"

void Container::AddChild(std::unique_ptr<Node> p) {
  m_children.push_back(std::move(p));
}

void Container::Render(RenderContext& context) {
  for (auto &child : m_children) {
    child->Render(context);
  }
}
void Container::SetAttribute(const char *key, const char *value) {}
