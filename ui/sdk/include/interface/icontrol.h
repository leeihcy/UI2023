#ifndef _UI_ICONTROL_H_
#define _UI_ICONTROL_H_

#include "iobject.h"

namespace ui
{
struct ITextRenderBase;

// control style
// #define CONTROL_STYLE_GROUP             0x00002000    // 是否是一个新组的开始
// #define CONTROL_STYLE_OWNERDRAW         0x00004000    // 是否自绘
typedef struct tagControlStyle
{
	bool  group : 1;
	bool  ownerdraw : 1;
} ControlStyle;

class Control;
struct UIAPI IControl : public IObject {
  void ModifyControlStyle(ControlStyle *add, ControlStyle *remove);
  bool TestControlStyle(ControlStyle *test);
  bool IsGroup();
  void SetGroup(bool);

  const char *GetToolTipText();
  void SetToolTipText(const char *szText);

  std::shared_ptr<ITextRenderBase> CreateDefaultTextRender();
  std::shared_ptr<ITextRenderBase> GetTextRenderOrDefault();
  void TryUpdateLayoutOnContentChanged();

  UI_DECLARE_INTERFACE(Control);
};

class Label;
struct UIAPI ILabel : public IControl {
   UI_DECLARE_INTERFACE(Label);
};

struct ButtonStyle {
  bool default_push_button : 1; // 可以成为默认按钮
  bool click_on_pushdown : 1;   // 当鼠标按下时就触发click，而不是鼠标弹起时触发
  bool notify_hoverleave : 1;   // 鼠标移入移出时，发出通知
  bool hand_cursor : 1;         // 使用手型鼠标
  bool checked : 1;             // radio/check button
  bool enable_radio_toggle : 1; // 允许radio button在被选中时，再次点击自己后，取消自己的选中状态
};

class Button;
struct UIAPI IButton : public IControl {
   UI_DECLARE_INTERFACE(Button);
};

#define BUTTON_CLICK_EVENT "clicked"
struct ButtonClickedEvent : public Event {
  IButton *button;
};

#define BUTTON_HOVER_EVENT "hover"
struct ButtonHoverEvent : public Event {
  IButton *button;
};
#define BUTTON_LEAVE_EVENT "leave"
struct ButtonLeaveEvent : public Event {
  IButton *button;
};
}

#endif  // _UI_ICONTROL_H_