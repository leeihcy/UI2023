#include "group.h"
#include "sdk/include/interface/iattribute.h"

namespace svg {

void Group::AddChild(std::unique_ptr<Element> p) {
  p->SetParent(this);
  m_children.push_back(std::move(p));
}

Element *Group::FindElementById(const char *id) {
  for (auto &iter : m_children) {
    if (iter->GetId() == id) {
      return iter.get();
    }
    return iter->FindElementById(id);
  }
  return nullptr;
}

void Group::RemoveAll() { m_children.clear(); }

void Group::Render(RenderContext &context) {
#if defined(_DEBUG)
  context.depth++;
  context.index = 0;
#endif

  for (auto &child : m_children) {

    RenderContext ctx = context;
    child->PrepareRender(ctx);
    child->Render(ctx);

#if defined(_DEBUG)
    context.index++;
#endif
  }
}
void Group::SetAttribute(ui::SerializeParam &data) {
  Element::SetAttribute(data);
}
} // namespace svg