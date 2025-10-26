#ifndef _UI_SDK_SRC_CONTROL_TEXT_LABEL_H_
#define _UI_SDK_SRC_CONTROL_TEXT_LABEL_H_

#include "include/interface/graphics.h"
#include "src/control/control.h"

namespace ui {

class Label : public Control {
public:
  Label(ILabel *p);

  void onRouteMessage(ui::Msg *msg);
  ILabel *GetILabel() { return m_pILabel; }

public:
  const char* GetText();
  void SetText(const char* text);
  
protected:
  void onFinalConstruct(FinalConstructMessage* msg);
  void onPaint(IRenderTarget* rt);
  void onSerialize(SerializeParam *pData);
  void onGetDesiredSize(Size *pSize);

  void onTextChanged();

private:
  ILabel *m_pILabel;
  std::string  m_text;  // utf8
};

} // namespace ui

#endif