#include "compositor.h"
#include "include/inc.h"
#include "include/util/log.h"
#include "src/application/uiapplication.h"
#include "src/window/window.h"
#include "windowrender.h"

namespace ui {
Compositor::Compositor() {
  m_root_layer = nullptr;
  m_pUIApp = nullptr;
  m_window_render = nullptr;
}

Compositor::~Compositor() {
  // layer由每个object自己去销毁
  m_root_layer = nullptr;
}

void Compositor::SetWindowRender(WindowRender *p) { m_window_render = p; }

void Compositor::SetUIApplication(Application *p) { m_pUIApp = p; }
Application *Compositor::GetUIApplication() { return m_pUIApp; }

Layer *Compositor::CreateLayer() {
  Layer *pLayer = this->createLayerObject();
  if (pLayer) {
    pLayer->SetCompositorPtr(this);
  }
  return pLayer;
}

void Compositor::BindWindow(Window *w) { this->onBindWindow(w); }

void Compositor::InvalidateNow() { m_window_render->InvalidateNow(); }
void Compositor::RequestInvalidate() { m_window_render->RequestInvalidate(); }

bool Compositor::CreateRenderTarget(IRenderTarget **pp) {
  return m_window_render->CreateRenderTarget(pp);
}

void Compositor::Commit(const RectRegion &arrDirtyInWindow) {
  if (!m_commit_listener) {
    this->doCommit(arrDirtyInWindow);
    return;
  }

  m_commit_listener->PreCommit(arrDirtyInWindow.RectPtr2(),
                               arrDirtyInWindow.Count());
  this->doCommit(arrDirtyInWindow);
  m_commit_listener->PostCommit(arrDirtyInWindow.RectPtr2(),
                                arrDirtyInWindow.Count());
}


} // namespace ui