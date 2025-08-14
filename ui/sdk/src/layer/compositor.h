#pragma once
#include "layer.h"
#include "include/util/rect_region.h"
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
  void SetRootLayer(Layer *pChanged) {m_root_layer = pChanged;}
  Layer *GetRootLayer() { return m_root_layer;}
  
  void BindWindow(Window* w);

  void InvalidateNow();
  void RequestInvalidate();

  void Commit(const RectRegion &arrDirtyInWindow);

  virtual bool UpdateDirty(RectRegion* outDirtyRegionInWindow) = 0;
  virtual void Resize(uint nWidth, uint nSize) = 0;

  void SetCommitListener(IWindowCommitListener *p) {m_commit_listener = p;}
  IWindowCommitListener *GetCommitListener() { return m_commit_listener;}
  
protected:
  virtual Layer *createLayerObject() = 0;
  virtual void onBindWindow(Window* w) = 0;
  virtual void doCommit(const RectRegion &arrDirtyInWindow) = 0;

protected:
  Application *m_pUIApp;

  Layer *m_root_layer;
  WindowRender *m_window_render;

private:
  IWindowCommitListener *m_commit_listener = nullptr;
};

} // namespace ui