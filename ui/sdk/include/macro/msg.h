#ifndef _UI_SDK_INCLUDE_MACRO_MSG_H_
#define _UI_SDK_INCLUDE_MACRO_MSG_H_

#include "sdk/include/event.h"

namespace ui {
struct IMeta;
struct IResource;

#define UI_MSG_FINALCONSTRUCT 168252120
struct FinalConstructMessage : public Msg {
  FinalConstructMessage() { message = UI_MSG_FINALCONSTRUCT; }
  // in
  IResource* resource = nullptr;
  IMeta* meta = nullptr;
  // out，如果设置为false，表示创建对象失败。
  bool success = true;
};

//  message : UI_WM_FINALRELEASE
#define UI_MSG_FINALRELEASE 168252121
struct FinalReleaseMessage : public Msg {
  FinalReleaseMessage() { message = UI_MSG_FINALRELEASE; }
};




struct MSG {
#if defined(OS_WIN)
  HWND hWnd;
#else
  long hWnd;
#endif
  unsigned int message;
  long wParam;
  long lParam;
};


// 消息结构定义。系统控件仍然使uiSG，但UI控件使用UIMsg进行代码
struct UIMSG : public MSG {
  UIMSG() { memset(this, 0, sizeof(UIMSG)); }

  IMessage *pMsgFrom; // 消息发送者
  IMessage *pMsgTo;   // 消息接受者
  unsigned int nCode;         // 针对 WM_COMMAND,WM_NOTIFY
  long lRet;          // 消息处理结束后的返回值
  bool bHandled;      // 该消息是否已被处理过
};

}

#endif