#pragma once
#include "layer.h"
#include "src/util/rectregion/rectregion.h"
#include "include/common/weakptr/weakptr.h"

// #define MSG_INVALIDATE 161311307   废弃，改为使用 messageloop posttask
// #define MSG_ASYNCTASK 175021413   废弃，改为使用 messageloop posttask

namespace ui {
class Application;

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
#if defined(OS_WIN)
  void BindHWND(HWND);
  HWND GetHWND();
#endif
  void RequestInvalidate();
  void _onRequestInvalidate();
  void UpdateAndCommit();
  void UpdateAndCommit(RECT* rcCommitEx);
  void Commit(const RectRegion &arrDirtyInWindow);

  virtual void UpdateDirty(RectRegion* outArrDirtyInWindow) = 0;
  virtual void Resize(uint nWidth, uint nSize) = 0;

protected:
  virtual Layer *virtualCreateLayer() = 0;
  virtual void virtualBindHWND(HWND) = 0;
  virtual void virtualCommit(const RectRegion &arrDirtyInWindow) = 0;

protected:
  Application *m_pUIApp;

  Layer *m_pRootLayer;
#if defined(OS_WIN)
  HWND m_hWnd;
#endif
  WindowRender *m_pWindowRender;

private:
  // 限制刷新时postmessage的次数。如果已经post了一个，就不再post
  long m_lPostInvalidateMsgRef;
  weakptr_factory<Compositor> m_weakptr_factory = {this};
};

} // namespace ui