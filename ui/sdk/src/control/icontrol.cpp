#include "include/interface/icontrol.h"
#include "src/control/control.h"
#include "src/control/control_meta.h"
#include "src/control/text/label.h"
#include "src/control/text/text_meta.h"
#include "src/control/button/button.h"
#include "src/control/button/button_meta.h"

namespace ui {
UI_IMPLEMENT_INTERFACE(Control, Object)
UI_IMPLEMENT_INTERFACE(Label, Control)
UI_IMPLEMENT_INTERFACE(Button, Control)

void IControl::ModifyControlStyle(ControlStyle *add, ControlStyle *remove) {
  __pImpl->ModifyControlStyle(add, remove);
}
bool IControl::TestControlStyle(ControlStyle *test) {
  return __pImpl->TestControlStyle(test);
}
bool IControl::IsGroup() { return __pImpl->IsGroup(); }
void IControl::SetGroup(bool b) { return __pImpl->SetGroup(b); }

std::shared_ptr<ITextRenderBase> IControl::GetTextRenderOrDefault() {
  return __pImpl->GetTextRenderOrDefault();
}
std::shared_ptr<ITextRenderBase> IControl::CreateDefaultTextRender() {
  return __pImpl->CreateDefaultTextRender();
}

const char *IControl::GetToolTipText() { return __pImpl->GetToolTipText(); }
void IControl::SetToolTipText(const char *szText) {
  __pImpl->SetToolTipText(szText);
}

void IControl::TryUpdateLayoutOnContentChanged() {
  __pImpl->TryUpdateLayoutOnContentChanged();
}

bool IButton::TestButtonStyle(const ButtonStyle &test) {
  return __pImpl->TestButtonStyle(test);
}

} // namespace ui