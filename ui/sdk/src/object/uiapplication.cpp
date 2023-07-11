#include "uiapplication.h"

#if defined(OS_MAC)
#include "mac/application_mac.h"
#endif

namespace ui {

UIApplication::UIApplication() {
#if defined(OS_MAC)
  ApplicationMac::Init();
#endif
}
void UIApplication::Run() { m_message_loop.Run(); }
void UIApplication::Quit() { m_message_loop.Quit(); }

} // namespace ui
