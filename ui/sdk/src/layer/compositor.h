#pragma once
#include "layer.h"
#include "src/util/rectregion/rectregion.h"
#include "include/common/ptr/weak_ptr.h"
#include "include/interface/iwindow.h"

namespace ui {
class Application;
class Window;

class Compositor {
public:
  Compositor();
  virtual ~Compositor();

  void SetUIApplication(Application *);
  void SetWindowRender(WindowRender *);
  Application *GetUIApplication();
  bool CreateRenderTarget(IRenderTarget **pp);

  Layer *CreateLayer();
  void SetRootLayer(Layer *pChanged);
  Layer *GetRootLayer();
  
  void BindWindow(Window* w);

  void RequestInvalidate();
  void _onRequestInvalidate();
  void UpdateAndCommit();
  void Commit(const RectRegion &arrDirtyInWindow);

  virtual void UpdateDirty(RectRegion* outArrDirtyInWindow) = 0;
  virtual void Resize(uint nWidth, uint nSize) = 0;

  void SetCommitListener(IWindowCommitListener *);
  IWindowCommitListener *GetCommitListener();
  
protected:
  virtual Layer *createLayerObject() = 0;
  virtual void onBindWindow(Window* w) = 0;
  virtual void doCommit(const RectRegion &arrDirtyInWindow) = 0;

protected:
  Application *m_pUIApp;

  Layer *m_pRootLayer;
  Window* m_window = nullptr;
  WindowRender *m_pWindowRender;

private:
  IWindowCommitListener *m_commit_listener = nullptr;

  // 限制刷新时postmessage的次数。如果已经post了一个，就不再post
  long m_request_invalidate_ref;
  weakptr_factory<Compositor> m_weakptr_factory = {this};
};

} // namespace ui