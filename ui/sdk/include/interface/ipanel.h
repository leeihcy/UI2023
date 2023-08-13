#ifndef _UI_IPANEL_H_
#define _UI_IPANEL_H_

#include "iobject.h"

namespace ui {
struct ILayout;
struct IObject;
struct IRenderBase;
class Panel;

struct UIAPI IPanel : public IObject {
  static Uuid UUID() { return Uuid("A591D454-65ED-40C3-8D84-612ABA3C8167"); }

  ILayout *GetLayout();
  void SetLayoutType(LAYOUTTYPE eLayoutType);

  void SetTextureRender(IRenderBase *p);
  IRenderBase *GetTextureRender();

  UI_DECLARE_INTERFACE(Panel);
};

class RoundPanel;
struct UIAPI IRoundPanel : public IPanel {
  static Uuid UUID() { return Uuid("4B9262CE-3380-4742-8C37-5FEF9FFE8B07"); }

  UI_DECLARE_INTERFACE(RoundPanel);
};

} // namespace ui

#endif