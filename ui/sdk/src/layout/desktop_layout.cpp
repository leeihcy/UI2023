#include "desktop_layout.h"
#include "canvaslayout.h"
#include "sdk/include/macro/uidefine.h"
#include "sdk/src/window/window.h"
#include "src/panel/root_object.h"

namespace ui {

void DesktopLayout::Arrange(Window *window) {
  if (nullptr == window)
    return;

#if 0 // defined(OS_WIN)
  if (window->IsChildWindow()) {
    HWND hWndParent = GetParent(window->m_hWnd);
    Rect rc;
    ::GetClientRect(hWndParent, &rc);

    int nCX = rc.Width();
    int nCY = rc.Height();

    CanvasLayout::ArrangeObject(window, nCX, nCY);
    return;
  }
#endif

  // 读取其他属性值来设置rectW
  int x = 0, y = 0;                     // 最终在屏幕中的坐标
  int nCXScreen = 800, nCYScreen = 600; // 屏幕大小
  int left = NDEF, top = NDEF, right = NDEF, bottom = NDEF;

  auto &root_object = window->GetRootObject();
  CanvasLayout canvas_layout;
  CanvasLayoutParam *pParam = canvas_layout.GetObjectLayoutParam(&root_object);
  if (!pParam) {
    return;
  }

  left = pParam->GetConfigLeft();
  top = pParam->GetConfigTop();
  right = pParam->GetConfigRight();
  bottom = pParam->GetConfigBottom();

  Size size_window = window->GetDesiredSize();

#ifdef _DEBUG
  if (size_window.width == 0 && size_window.height == 0) {
    UI_LOG_WARN(
        "GetDesiredSize 返回窗口大小为0，检查是否配置了窗口的widht/height");
  }
#endif

  ui::Rect rc_workarea;
  window->m_platform->GetMonitorWorkArea(&rc_workarea);

  ui::Rect rc_parent; // TODO: 如果当前窗口是子窗口，获取对应父窗口区域。
  // 这里先只做没有父窗口的场景
  rc_parent = rc_workarea;

  // 如果同时指定了left/right,则忽略width属性
  if (left != NDEF && right != NDEF) {
    size_window.width = rc_parent.Width() - left - right;
  }
  // 如果同时指定了top/bottom，则忽略height属性
  if (top != NDEF && bottom != NDEF) {
    size_window.height = rc_parent.Height() - top - bottom;
  }

#if 0
  // 再次确认最小最大尺寸
  int min_width = window->GetMinWidth();
  int min_height = window->GetMinHeight();
  int max_width = window->GetMinWidth();
  int max_height = window->GetMaxHeight();
  if (min_width > 0 && size_window.width < min_width)
    size_window.width = min_width;
  if (min_height > 0 && size_window.height < min_height)
    size_window.height = min_height;
  if (max_width > 0 && size_window.width > max_width)
    size_window.width = max_width;
  if (max_height > 0 && size_window.height > max_height)
    size_window.height = max_height;
#endif

  // 计算出坐标
  if (left != NDEF) {
    x = rc_parent.left + left;
    x += root_object.GetMarginL();
  } else {
    if (right != NDEF) {
      x = rc_parent.right - right -
          size_window.width; // right是指窗口右侧距离屏幕右侧的距离
      x -= root_object.GetMarginR();
    } else {
      // 居中
      x = rc_parent.left + (rc_parent.Width() - size_window.width) / 2;
    }
  }
  if (top != NDEF) {
    y = rc_parent.top + top;
    y += root_object.GetMarginT();
  } else {
    if (bottom != NDEF) {
      y = rc_parent.bottom - bottom - size_window.height; // 同right
      y -= root_object.GetMarginB();
    } else {
      // 居中
      y = rc_parent.top + (rc_parent.Height() - size_window.height) / 2;
    }
  }

  // 限制在屏幕之内
  if (x < rc_workarea.left)
    x = rc_workarea.left;
  else if (x + size_window.width > rc_workarea.right)
    x = rc_workarea.right - size_window.width;

  if (y < rc_workarea.top)
    y = rc_workarea.top;
  else if (y + size_window.height > rc_workarea.bottom)
    y = rc_workarea.bottom - size_window.height;

  Rect rc_client_old, rc_client_new;
  window->m_platform->GetClientRect(&rc_client_old);

  SetPositionFlags flags;
  flags.redraw = false;
  window->m_platform->SetWindowPos(x, y, size_window.width, size_window.height,
                                   flags);

  // 解决如果窗口大小没有发生改变，改变窗口没有收到WM_SIZE时，手动布局一次
  window->m_platform->GetClientRect(&rc_client_new);
  window->m_dpi.RestoreRect(&rc_client_new);

  if (rc_client_new.Width() == rc_client_old.Width() &&
      rc_client_new.Height() == rc_client_old.Height()) {
    root_object.notify_WM_SIZE(0, rc_client_new.Width(), rc_client_new.Height(),
                               window->m_dpi.GetScaleFactor());
  }
}

} // namespace ui