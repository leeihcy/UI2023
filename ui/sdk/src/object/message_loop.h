#ifndef _UI_SDK_SRC_BASE_MSG_LOOP_H_
#define _UI_SDK_SRC_BASE_MSG_LOOP_H_

namespace ui
{
class MessageLoop
{
public:
    void  MsgHandleLoop(bool* pbQuitLoopRef);
    
};
} // namespace ui


#endif