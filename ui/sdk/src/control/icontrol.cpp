#include "include/interface/icontrol.h"
#include "src/control/control.h"
#include "src/control/control_meta.h"

namespace ui {
UI_IMPLEMENT_INTERFACE(Control, Object)

void IControl::ModifyControlStyle(ControlStyle *add, ControlStyle *remove) {
  __pImpl->ModifyControlStyle(add, remove);
}
bool IControl::TestControlStyle(ControlStyle *test) {
  return __pImpl->TestControlStyle(test);
}
bool IControl::IsGroup() { return __pImpl->IsGroup(); }
void IControl::SetGroup(bool b) { return __pImpl->SetGroup(b); }

ITextRenderBase *IControl::GetTextRenderDefault() {
  return __pImpl->GetTextRenderDefault();
}
ITextRenderBase *IControl::CreateDefaultTextRender() {
  return __pImpl->CreateDefaultTextRender();
}

const char *IControl::GetToolTipText() { return __pImpl->GetToolTipText(); }
void IControl::SetToolTipText(const char *szText) {
  __pImpl->SetToolTipText(szText);
}

void IControl::TryUpdateLayoutOnContentChanged() {
  __pImpl->TryUpdateLayoutOnContentChanged();
}

} // namespace ui