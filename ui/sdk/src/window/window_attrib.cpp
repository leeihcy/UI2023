#include "src/attribute/attribute.h"
#include "window.h"

namespace ui {

void Window::onSerialize(SerializeParam *pData) {
  // 放在最前面，设置好Graphics Render Library
  m_window_render.OnSerialize(pData);

  AttributeSerializer s(pData, "Window");

  Panel::onSerialize(pData);

#if 0
  s.AddString(XML_FONT, this,
              memfun_cast<pfnStringSetter>(&Window::SetDefaultRenderFont),
              memfun_cast<pfnStringGetter>(&Window::GetDefaultRenderFontId))
      ->SetDefault(nullptr);
#endif

  s.AddString(XML_TEXT, m_strConfigWindowText);
}

} // namespace ui