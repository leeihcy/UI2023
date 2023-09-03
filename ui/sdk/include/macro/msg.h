#ifndef _UI_SDK_INCLUDE_MACRO_MSG_H_
#define _UI_SDK_INCLUDE_MACRO_MSG_H_

// #include "windows.h"
#include "sdk/include/event.h"
#include "sdk/include/util/rect.h"

namespace ui {
struct IMeta;
struct IResource;
struct IRenderTarget;
struct ILayout;

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

//  如果这个对象支持布局，返回布局对象指针。如OBJ_WINDOW,OBJ_PANEL,OBJ_COMPOUND_CONTROL
#define UI_MSG_GETLAYOUT 168261637
struct GetLayoutMessage : public Msg {
  GetLayoutMessage() { message = UI_MSG_GETLAYOUT; }
  // out
  ILayout* layout = nullptr;
};

//
//  获取对象期望大小，不包含对象的Margin，但需要自己去计算padding/border等
//
//  wparam : [out] Size*
//  lparam : GETDESIREDSIZEINFO* (目前仅在ITextRenderBase值不为nullptr)
//
#define UI_MSG_GETDESIREDSIZE 168261803
struct GetDesiredSizeMessage : public Msg {
  GetDesiredSizeMessage() { message = UI_MSG_GETDESIREDSIZE; };
  // out
  Size size = {0, 0};
};

// void  GetDesiredSize(Size* pSize);
// #define UIMSG_GETDESIREDSIZE(func)                                             \
//   if (uMsg == UI_MSG_GETDESIREDSIZE) {                                         \
//     SetMsgHandled(true);                                                       \
//     func((Size *)wParam);                                                      \
//     if (IsMsgHandled())                                                        \
//       return true;                                                             \
//   }


struct MSG {
#if 0 // defined(OS_WIN)
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