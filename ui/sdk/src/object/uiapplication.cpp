#include "uiapplication.h"

namespace ui {

void UIApplication::Run() { m_message_loop.Run(); }
void UIApplication::Quit() { m_message_loop.Quit(); }

} // namespace ui
