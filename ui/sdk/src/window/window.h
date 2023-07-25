#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_H_

#include "common.h"
#include "interface/iwindow.h"
#include "common/signalslot/signal.h"
#include "base/uimsg.h"
#include "object/object.h"
#include <SkCanvas.h>
#include <SkSurface.h>

namespace ui {
class Window;

struct WindowPlatform {
  virtual ~WindowPlatform(){};
  virtual void Initialize() = 0;
  virtual void Release() = 0;
  virtual bool Create(const Rect &rect) = 0;
  virtual void SetTitle(const char* title) = 0;
  virtual void Show() = 0;
  virtual void Submit(sk_sp<SkSurface> sksurface) = 0;
};

class Window : public Object {
public:
  Window(IWindow* p);
  ~Window();

	UI_BEGIN_MSG_MAP()
        // UIMSG_ERASEBKGND( OnEraseBkgnd )
        // UIMSG_DM_GETDEFID( OnGetDefId )
        // UIMSG_DM_SETDEFID( OnSetDefId )
        // UIMSG_GETDESIREDSIZE( OnGetDesiredSize )
        // UIMSG_QUERYINTERFACE( WindowBase )
        // UIMSG_SERIALIZE( OnSerialize )
        // UIMSG_FINALCONSTRUCT( FinalConstruct )
        // UIMSG_PRECREATEWINDOW( PreCreateWindow )
    // UI_END_MSG_MAP_CHAIN_PARENT( Panel )
    // UI_END_MSG_MAP_CHAIN_PARENT( Message )
    UI_END_MSG_MAP()

  void OnMessage();
  void Create(const Rect &rect);
  void SetTitle(const char* title);
  void Show();

public:
  // platform回调
  void onClose();
  void onDestroy();
  void onPaint(Rect *dirty);
  void onSize(int width, int height);

  signal<void()> &DestroySignal() { return m_signal_destroy; }
  signal<void(SkCanvas&)> &PaintSignal() { return m_signal_paint; }
  
private:
  void on_paint(SkCanvas& canvas);
  void on_erase_bkgnd(SkCanvas& canvas);
  void swap_buffer();

private:
  // 平台相关函数。
  WindowPlatform *m_platform = nullptr;

  // 渲染
  sk_sp<SkSurface> m_sksurface;
  int m_width = 0;
  int m_height = 0;

  // 事件定义
  signal<void()> m_signal_destroy;
  signal<void(SkCanvas&)> m_signal_paint;
};

} // namespace ui

#endif