#ifndef _UI_IPANEL_H_
#define _UI_IPANEL_H_

#include "iobject.h"

namespace ui {
struct ILayout;
struct IObject;
struct IRenderBase;
class Panel;

struct UIAPI IPanel : public IObject {
  ILayout *GetLayout();
  void SetLayout(ILayout* p);
  void SetLayoutType(const char* name);

  void SetTextureRender(IRenderBase *p);
  IRenderBase *GetTextureRender();

  UI_DECLARE_INTERFACE(Panel);
};

class RoundPanel;
struct UIAPI IRoundPanel : public IPanel {
  UI_DECLARE_INTERFACE(RoundPanel);
};

} // namespace ui

#endif