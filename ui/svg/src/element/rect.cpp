#include "rect.h"

namespace ui { namespace svg {

Rect::Rect(IRect* p) : ui::MessageProxy(p) {
  m_pIRect = p;
}

}}