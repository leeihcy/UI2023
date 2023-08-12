#include "compositor.h"
#include "include/inc.h"
#include "src/application/uiapplication.h"
#include "src/window/window.h"
#include "windowrender.h"

namespace ui {
Compositor::Compositor() {
  m_pRootLayer = nullptr;
  m_pUIApp = nullptr;
#if defined(OS_WIN)
  m_hWnd = nullptr;
#endif
  m_pWindowRender = nullptr;
  m_request_invalidate_ref = 0;
}

Compositor::~Compositor() {
  // 由每个object自己去销毁
  //     while (m_pRootLayer)
  //     {
  //         Layer* p = m_pRootLayer;
  //         m_pRootLayer = m_pRootLayer->GetNext();
  //
  //         delete p;
  //     }
}

void Compositor::SetUIApplication(Application *p) { m_pUIApp = p; }
Application *Compositor::GetUIApplication() { return m_pUIApp; }

Layer *Compositor::CreateLayer() {
  Layer *pLayer = this->virtualCreateLayer();
  if (pLayer) {
    pLayer->SetCompositorPtr(this);
  }

  return pLayer;
}

void Compositor::SetRootLayer(Layer *pChanged) { m_pRootLayer = pChanged; }

Layer *Compositor::GetRootLayer() { return m_pRootLayer; }

#if defined(OS_WIN)
void Compositor::BindHWND(HWND hWnd) {
  if (m_hWnd == hWnd)
    return;

  m_hWnd = hWnd;
  this->virtualBindHWND(hWnd);
}

HWND Compositor::GetHWND() { return m_hWnd; }
#endif

void Compositor::RequestInvalidate() {
  // #if defined(OS_WIN)
  //   UIASSERT(m_hWnd);
  //   if (m_request_invalidate_ref == 0) {
  //     ::PostMessage(m_hWnd, UI_MSG_POSTMESSAGE, MSG_INVALIDATE, 0);
  //     m_request_invalidate_ref++;
  //   }
  // #endif
  // UI_LOG_DEBUG(L"RequestInvalidate");

  // m_pWindowRender->m_window.Invalidate(nullptr);

  if (m_request_invalidate_ref == 0) {
    weakptr<Compositor> ptr = m_weakptr_factory.get();
    m_pUIApp->GetMessageLoop().PostTask(
        Slot(&Compositor::_onRequestInvalidate, ptr));
    m_request_invalidate_ref++;
  }
}

void Compositor::_onRequestInvalidate() {
  // UI_LOG_DEBUG(L"_onRequestInvalidate");
  UpdateAndCommit();
  m_request_invalidate_ref = 0;
}

void Compositor::UpdateAndCommit() {
  RectRegion arrDirtyInWindow;
  this->UpdateDirty(&arrDirtyInWindow);

  if (!m_pWindowRender->CanCommit()) {
    UI_LOG_WARN(L"can not commit now");
    return;
  }

  this->Commit(arrDirtyInWindow);
}


void Compositor::Commit(const RectRegion &arrDirtyInWindow) {
  if (m_commit_listener) {
    m_commit_listener->PreCommit(arrDirtyInWindow.GetArrayPtr2(),
                         arrDirtyInWindow.GetCount());

    this->virtualCommit(arrDirtyInWindow);

    m_commit_listener->PostCommit(arrDirtyInWindow.GetArrayPtr2(),
                          arrDirtyInWindow.GetCount());
  } else {
    this->virtualCommit(arrDirtyInWindow);
  }
}

bool Compositor::CreateRenderTarget(IRenderTarget **pp) {
  return m_pWindowRender->CreateRenderTarget(pp);
}

void Compositor::SetCommitListener(IWindowCommitListener *p) {
  m_commit_listener = p;
}
IWindowCommitListener *Compositor::GetCommitListener() {
  return m_commit_listener;
}

void Compositor::SetWindowRender(WindowRender *p) { m_pWindowRender = p; }
} // namespace ui