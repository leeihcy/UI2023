#ifndef _UI_SDK_SRC_BASE_UIAPPLICATION_H_
#define _UI_SDK_SRC_BASE_UIAPPLICATION_H_

#include "message_loop/message_loop.h"

namespace ui
{
class UIApplication
{
public:  
    void Run();

private:
    MessageLoop m_message_loop;
    
};
} // namespace ui


#endif