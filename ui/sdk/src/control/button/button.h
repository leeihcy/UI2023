#ifndef _UI_SDK_SRC_CONTROL_BUTTON_BUTTON_H_
#define _UI_SDK_SRC_CONTROL_BUTTON_BUTTON_H_

#include "include/interface/icontrol.h"
#include "include/interface/irenderbase.h"
#include "include/interface/renderlibrary.h"
#include "include/macro/msg.h"
#include "include/macro/xmldefine.h"
#include "src/control/control.h"
#include <shared_mutex>

namespace ui {

class Button : public Control {
public:
  Button(IButton *p);

  void onRouteMessage(ui::Msg *msg);
  IButton *GetIButton() { return m_pIButton; }

public:
  const char* GetText();
  void SetText(const char* text);

	void  ModifyButtonStyle(ButtonStyle* add, ButtonStyle* remove);
	bool  TestButtonStyle(const ButtonStyle& test);
  
  bool IsChecked() { return m_button_style.checked; }

protected:
  struct ButtonDrawContext {
    IRenderTarget *r;
    int render_state;
    ui::Rect bounds;  // 0,0,w,h
  };
  void drawBackground(ButtonDrawContext& c);
  void drawText(ButtonDrawContext& c);
  void drawIcon(ButtonDrawContext& c);
  void drawFocus(ButtonDrawContext& c);
  int getDrawState();
  
protected:
  void onFinalConstruct(FinalConstructMessage* msg);
  void onPaint(IRenderTarget* rt);
  void onPaintBkgnd(IRenderTarget* rt);
  void onSerialize(SerializeParam *pData);
  void onGetDesiredSize(Size *pSize);
  void onStateChanged(StateChangedMessage* msg);
  void onTextChanged();
  void onLButtonDown(LButtonDownMessage* msg);
  void onLButtonUp(LButtonUpMessage* msg);
  void onClicked();

private:
  IButton *m_pIButton;
  ButtonStyle  m_button_style;

  std::string  m_text;  // utf8

  eButtonAutoSizeType m_auto_size_type = eButtonAutoSizeType::NotDefine;
  int m_icon_align = AlignLeft;
  int m_icon_text_space = 0;

  std::shared_ptr<IRenderBase>  m_focus_render;
  std::shared_ptr<IRenderBase>  m_default_render;
};

} // namespace ui

#endif