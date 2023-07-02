#ifndef _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_H_
#define _UI_SDK_SRC_OBJECT_WINDOW_WINDOW_H_

#include "../../base/common.h"
#include "../../base/signalslot/signal.h"
#include <SkCanvas.h>
#include <SkSurface.h>

namespace ui {
class Window;

struct WindowPlatform {
  virtual ~WindowPlatform(){};
  virtual void Initialize(ui::Window *) = 0;
  virtual void Release() = 0;
  virtual void Create(const Rect &rect) = 0;
  virtual void Show() = 0;
  virtual void Submit(sk_sp<SkSurface> sksurface) = 0;
};

class Window {
public:
  Window();
  ~Window();

  void OnMessage();
  void Create(const Rect &rect);
  void Show();

public:
  // platform回调
  void onClose();
  void onDestroy();
  void onPaint(Rect *dirty);
  void onSize(int width, int height);

  signal<void()> &DestroySignal() { return m_signal_destroy; }

private:
  void on_paint(SkCanvas *canvas);
  void on_erase_bkgnd(SkCanvas *canvas);
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
};

} // namespace ui

#endif