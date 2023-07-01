#include "uiapplication.h"

namespace ui
{

void UIApplication::Run()
{
    m_message_loop.Run();
}

} // namespace ui
