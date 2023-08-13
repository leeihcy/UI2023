#include "rect.h"
#include "rect_desc.h"

namespace ui { namespace svg {

Rect::Rect(IRect* p) : ui::MessageProxy(p) {
  m_pIRect = p;
  m_pIRect->SetDescription(RectDescription::Get());
}

void Rect::OnEraseBkgnd(ui::IRenderTarget* rt)
{
  ui::Rect rc;
  m_pIRect->GetClientRectInObject(&rc);
  ui::Color c(0,0, 255);
  rt->DrawRect(&rc, &c);
}

}}