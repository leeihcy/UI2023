#ifndef _UI_SDK_SRC_OBJECT_ROOT_OBJECT_H_
#define _UI_SDK_SRC_OBJECT_ROOT_OBJECT_H_
#include "include/interface/iobject.h"
#include "include/interface/iwindow.h"
#include "src/panel/panel.h"

namespace ui {
class Window;

class RootObject : public Panel {
public:
  RootObject(IRootObject *p);
  IRootObject* GetIRootObject() { return m_irootobject; }

  void BindWindow(Window *window) { m_window = window; }
  Window& GetWindow() { return *m_window; }

  void onRouteMessage(ui::Msg *msg);
  void onWindowCreate();
  void OnClientSize(unsigned int nWidth, unsigned int nHeight);
  void syncWindowSize();

  bool LoadLayout(const char *layout_id);

private:
  void FinalConstruct();
  // void recursion_on_load_notify(Object *pParent);
  void onPaintBkgnd(IRenderTarget *);
  
private:
  Window *m_window = nullptr; // not null
  IRootObject* m_irootobject = nullptr;
};
} // namespace ui

#endif // _UI_SDK_SRC_OBJECT_ROOT_OBJECT_H_