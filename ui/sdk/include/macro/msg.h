#ifndef _UI_SDK_INCLUDE_MACRO_MSG_H_
#define _UI_SDK_INCLUDE_MACRO_MSG_H_

// #include "windows.h"
#include "sdk/include/event.h"
#include "sdk/include/util/rect.h"

namespace ui {
struct IObject;
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

// 绘制包含非客户区，不受padding scroll影响
#define UI_MSG_PAINTBKGND 238261617
struct PaintBkgndMessage : public Msg {
  PaintBkgndMessage() { message = UI_MSG_PAINTBKGND; }
  IRenderTarget* rt;
};

// 绘制客户区，已踢除padding，并且scroll
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



struct IRenderTarget;
struct RENDERBASE_DRAWSTATE {
  IRenderTarget *pRenderTarget;
  Rect rc;
  int nState;
};

struct TEXTRENDERBASE_DRAWSTATE {
  RENDERBASE_DRAWSTATE ds_renderbase;
  const char *szText;
  int nDrawTextFlag;
};

//
//  RenderBase绘制消息
//
//  message : UI_WM_RENDERBASE_DRAWSTATE
//  code : NA
//  wparam :
//  lparam : RENDERBASE_DRAWSTATE*
//
#define UI_MSG_RENDERBASE_DRAWSTATE 168261825
struct RenderBaseDrawStateMessage : public Msg {
  RenderBaseDrawStateMessage() { message = UI_MSG_RENDERBASE_DRAWSTATE; };
  RENDERBASE_DRAWSTATE draw_state;
};

struct TextRenderDrawStateMessage : public Msg {
  TextRenderDrawStateMessage() { message = UI_MSG_RENDERBASE_DRAWSTATE; };
  TEXTRENDERBASE_DRAWSTATE draw_state;
};



//
//  加载完所有控件后，由窗口发起的初始化
//
//  初始化顺序：
//    1. 初始化自己UI_WM_INITIALIZE，
//    2. 初始化自己的子对象
//    3. 给自己再发送一个UI_WM_INITIALIZE2消息表示子对象初始化完成
//    4. 给自己再发送一个UI_MSG_BINDPLZ消息,表示可以去绑定子控件了
//    每次换布局（换肤）后，会再发送一次UI_MSG_BINDPLZ通知，但init消息不会再发送了
//    换肤前会给每个控件发送一个UI_MSG_UNBINDPLZ通知，用于取消各种绑定
//
#define UI_MSG_INITIALIZE 155051209
#define UI_MSG_INITIALIZE2 155051210
// void  OnInitialize();
// void  OnInitialize2();


#define UI_MSG_DOBINDPLZ 174191106
// void  DoBindPlz(bool bind);
struct DoBindPlzMessage : public ui::Msg {
  DoBindPlzMessage() { message = UI_MSG_DOBINDPLZ; }

  // true绑定， false取消绑定
  bool bind = true;
};


//	祖、父对象中的可见状态发生了改变（主要是用于在父对象隐藏时，能自动将HwndObj对象也隐藏）
#define UI_MSG_VISIBLE_CHANGED 168261620
struct VisibleChangedMessage : public ui::Msg {
  VisibleChangedMessage() { message = UI_MSG_VISIBLE_CHANGED; }
  bool visible = true;
  ui::IObject* obj_trigger;
};


//
//	MouseManager通知一个对象，它的当前状态发生改变(hover,press...)
//  专门用于刷新处理，不要在这个消息中添加其它逻辑处理，因为可能会被HOOK掉，导致状态不正确
//
#define UI_MSG_STATECHANGED 168261626
struct StateChangedMessage : public ui::Msg {
  StateChangedMessage() { message = UI_MSG_STATECHANGED; }
  unsigned int state_changed_mask;
};

//
//  修改创建窗口数据
//
//  message: CREATESTRUCT*
//
#define UI_MSG_PRECREATEWINDOW 168270752

struct CreateWindowParam {
  bool position;
  int x;
  int y;
  int w;
  int h;
  
  // xywh的单位，true表示是像素，false表示是已经乘以了dpi系数。
  bool is_pixel_unit = false;
  // wh是窗口大小，还是客户区域大小。
  bool is_client_size = false;

  bool transparent;
  bool topmost;
};

struct PreCreateWindowMessage : public ui::Msg{
  PreCreateWindowMessage() {
     message = UI_MSG_PRECREATEWINDOW; 
     memset(&param, 0, sizeof(param));
  }
  CreateWindowParam param;
};
// bool  OnPreCreateWindow(CreateWindowParam* pcs);


#define UI_MSG_MOVE 257131741
struct MoveMessage : public ui::Msg {
  MoveMessage() { message = UI_MSG_MOVE; }
  int x = 0; 
  int y = 0;
};

#define UI_MSG_SIZE 257131742
struct SizeMessage : public ui::Msg {
  SizeMessage() { message = UI_MSG_SIZE; }
  int width = 0; 
  int height = 0;
};

#define UI_MSG_POSCHANGED 257131742
struct PosChangedMessageMessage : public ui::Msg {
  PosChangedMessageMessage() {
    message = UI_MSG_POSCHANGED;
  }
  int  x = 0;
  int  y = 0;
  int  width = 0;
  int  height = 0;
};



//
//	获取当前绘制的偏移量，用于OnDraw
//
//		wparam:  [out] int* xOffset
//		lparam:  [out] int* yOffset
//
//	Return
//
#define UI_MSG_GETSCROLLOFFSET 168261657
struct GetScrollOffsetMessage : public ui::Msg {
  GetScrollOffsetMessage() {
    message = UI_MSG_GETSCROLLOFFSET;
  }
  int x_offset = 0;
  int y_offset = 0;
};


//
//	获取当前滚动范围
//
//		wparam:  [out] int* xRange
//		lparam:  [out] int* yRange
//
//	Return
//
#define UI_MSG_GETSCROLLRANGE 168261658
struct GetScrollRangeMessage : public ui::Msg {
  GetScrollRangeMessage() {
    message = UI_MSG_GETSCROLLRANGE;
  }
  int x_range = 0;
  int y_range = 0;
};


#if 0
struct MSG {
#if 0 // defined(OS_WIN)
  HWND hWnd;
#else
  llong hWnd;
#endif
  unsigned int message;
  llong wParam;
  llong lParam;
};


// 消息结构定义。系统控件仍然使uiSG，但UI控件使用UIMsg进行代码
struct UIMSG : public MSG {
  UIMSG() { memset(this, 0, sizeof(UIMSG)); }

  IMessage *pMsgFrom; // 消息发送者
  IMessage *pMsgTo;   // 消息接受者
  unsigned int nCode;         // 针对 WM_COMMAND,WM_NOTIFY
  llong lRet;          // 消息处理结束后的返回值
  bool bHandled;      // 该消息是否已被处理过
};
#endif

}

#endif