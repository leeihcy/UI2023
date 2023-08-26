#ifndef _UI_SDK_INCLUDE_MACRO_MSG_H_
#define _UI_SDK_INCLUDE_MACRO_MSG_H_

#include "sdk/include/event.h"

namespace ui {
struct IMeta;
struct IResource;
struct IRenderTarget;

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

#define UI_MSG_ERASEBKGND 238261617
struct EraseBkgndMessage : public Msg {
  EraseBkgndMessage() { message = UI_MSG_ERASEBKGND; }
  IRenderTarget* rt;
};

#define UI_MSG_PAINT 238261716
struct PaintMessage : public Msg {
  PaintMessage() { message = UI_MSG_PAINT; }
  IRenderTarget* rt;
};

// 控件在自己的子对象画完之后，再接受一个消息用于后处理
// 需要控件有post_paint样式
#define UI_MSG_POSTPAINT 147281159
struct PostPaintMessage : public Msg {
  PostPaintMessage() { message = UI_MSG_POSTPAINT; }
  IRenderTarget* rt;
};


//  接口查询
#define UI_MSG_QUERYINTERFACE 165312200
struct QueryInterfaceMessage : public Msg {
  QueryInterfaceMessage() { message = UI_MSG_QUERYINTERFACE;}
  Uuid uuid;
  // out
  void** pp = nullptr;
};

// 加载属性
struct SerializeParam;
#define UI_MSG_SERIALIZE 139281928
struct SerializeMessage : public Msg {
  SerializeMessage() { message = UI_MSG_SERIALIZE; }
  SerializeParam* param = nullptr;
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