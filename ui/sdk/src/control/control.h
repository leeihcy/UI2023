#pragma once

#include "include/interface/icontrol.h"
#include "src/object/object.h"
#include <memory>

namespace ui {
struct IToolTipUI;

class Control : public Object {
public:
  Control(IControl *);

  void onRouteMessage(ui::Msg *msg);
  IControl *GetIControl() { return static_cast<IControl*>(m_imessage); }

public:
  void ModifyControlStyle(ControlStyle *add, ControlStyle *remove);
  bool TestControlStyle(ControlStyle *test);

  bool IsGroup();
  void SetGroup(bool b);

  std::shared_ptr<ITextRenderBase> GetTextRenderOrDefault();
  std::shared_ptr<ITextRenderBase> CreateDefaultTextRender();
  void TryUpdateLayoutOnContentChanged();

public:
  const char *GetToolTipText();
  void SetToolTipText(const char *szText);

protected:
  // BOOL OnGetToolTipInfo(TOOLTIPITEM *pItem, IToolTipUI *pUI);
  void onSerialize(SerializeParam *data);
  void onPaintBkgnd(IRenderTarget *r);
  void onPaint(IRenderTarget *r);
  void onGetDesiredSize(Size *size);

protected:
  ControlStyle m_controlStyle;
};

} // namespace ui
