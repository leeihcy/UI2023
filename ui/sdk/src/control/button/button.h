#ifndef _UI_SDK_SRC_CONTROL_BUTTON_BUTTON_H_
#define _UI_SDK_SRC_CONTROL_BUTTON_BUTTON_H_

#include "include/interface/icontrol.h"
#include "include/interface/irenderbase.h"
#include "include/interface/graphics.h"
#include "include/macro/msg.h"
#include "include/macro/xmldefine.h"
#include "include/util/rect.h"
#include "src/control/control.h"
#include <shared_mutex>

namespace ui {

class Button : public Control {
public:
  Button(IButton *p);

  void onRouteMessage(ui::Msg *msg);
  IButton *GetIButton() { return static_cast<IButton*>(m_imessage); }

public:
  const char *GetText();
  void SetText(const char *text);

  void ModifyButtonStyle(ButtonStyle *add, ButtonStyle *remove);
  bool TestButtonStyle(const ButtonStyle &test);

  bool IsChecked() { return m_button_style.checked; }
  void SetDefaultPushButtonStyle(bool);
  bool HasDefaultPushButtonStyle();
  void SetAutoDefault(bool);
  bool IsAutoDefault();

protected:
  struct ButtonDrawContext {
    IRenderTarget *r;
    int render_state;
    ui::Rect bounds; // 0,0,w,h
  };
  void drawText(ButtonDrawContext &c);
  void drawIcon(ButtonDrawContext &c);
  int getDrawState();

protected:
  void onFinalConstruct(FinalConstructMessage *msg);
  void onBind();
  void onPaint(IRenderTarget *rt);
  void onPaintBkgnd(IRenderTarget *rt);
  void onSerialize(SerializeParam *pData);
  void onGetDesiredSize(Size *pSize);
  void onStateChanged(StateChangedMessage *msg);
  void onTextChanged();
  void onLButtonDown(LButtonDownMessage *msg);
  void onLButtonUp(LButtonUpMessage *msg);
  void onClicked();

  void onKeyDown(KeyDownMessage* msg);
  void onKeyUp(KeyUpMessage* msg);
  
  void setWindowDefaultButton();

private:
  ButtonStyle m_button_style;

  std::string m_text; // utf8

  eButtonAutoSizeType m_auto_size_type = eButtonAutoSizeType::NotDefine;
  int m_icon_align = AlignLeft;
  int m_icon_text_space = 0;

  std::shared_ptr<IRenderBase> m_focus_render;
  std::shared_ptr<IRenderBase> m_default_render;
};

} // namespace ui

#endif