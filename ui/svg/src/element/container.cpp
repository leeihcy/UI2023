#include "container.h"

void Container::AddChild(std::unique_ptr<Element> p) {
  m_children.push_back(std::move(p));
}

void Container::RemoveAll() {
  m_children.clear();
}

void Container::Render(RenderContext& context) {
  for (auto &child : m_children) {
    RenderContext c = context;
    child->Render(c);
  }
}
void Container::SetAttribute(ui::SerializeParam& data) {}
