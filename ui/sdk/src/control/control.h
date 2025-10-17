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
  IControl *GetIControl() { return m_pIControl; }

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
  void onSerialize(SerializeParam *pData);
  void onPaintBkgnd(IRenderTarget *pRenderTarget);
  void onGetDesiredSize(Size *pSize);

protected:
  IControl *m_pIControl;

  ControlStyle m_controlStyle;
};

} // namespace ui
