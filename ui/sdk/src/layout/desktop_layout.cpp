#include "desktop_layout.h"
#include "canvaslayout.h"
#include "sdk/src/window/window.h"
#include "sdk/include/macro/uidefine.h"


namespace ui {

void DesktopLayout::Arrange(Window *pWindow) {
  if (nullptr == pWindow)
    return;

#if 0 // defined(OS_WIN)
  if (pWindow->IsChildWindow()) {
    HWND hWndParent = GetParent(pWindow->m_hWnd);
    Rect rc;
    ::GetClientRect(hWndParent, &rc);

    int nCX = rc.Width();
    int nCY = rc.Height();

    CanvasLayout::ArrangeObject(pWindow, nCX, nCY);
    return;
  }
#endif


  // 读取其他属性值来设置rectW
  int x = 0, y = 0;                     // 最终在屏幕中的坐标
  int nCXScreen = 800, nCYScreen = 600; // 屏幕大小
  int left = NDEF, top = NDEF, right = NDEF, bottom = NDEF;

  CanvasLayout canvas_layout;
  CanvasLayoutParam *pParam = canvas_layout.GetObjectLayoutParam(pWindow);
  if (!pParam) {
    return;
  }

  left = pParam->GetConfigLeft();
  top = pParam->GetConfigTop();
  right = pParam->GetConfigRight();
  bottom = pParam->GetConfigBottom();

  // 获得的SIZE包括了MARGIN的大小
  // s.width=margin.left+width+margin.right
  Size s = pWindow->GetDesiredSize();
#ifdef _DEBUG
  if (s.width == 0 && s.height == 0) {
    UI_LOG_WARN(
        "GetDesiredSize 返回窗口大小为0，检查是否配置了窗口的widht/height");
  }
#endif

  Rect rcParent;
  Rect rcLimit;
  rcParent.SetEmpty();
  rcLimit.SetEmpty();

#if 0
  // 计算出屏幕工作区域的大小(不使用整个屏幕的大小)
  // nCXScreen = ::GetSystemMetrics( SM_CXSCREEN );
  // nCYScreen = ::GetSystemMetrics( SM_CYSCREEN );

  if (GetWindowLong(pWindow->m_hWnd, GWL_STYLE) & WS_CHILD) {
    HWND hWndParent = GetParent(pWindow->m_hWnd);
    if (hWndParent) {
      ::GetClientRect(hWndParent, &rcParent);
      rcLimit = rcParent;
    }
  } else {
    HWND hWndParent = GetParent(pWindow->m_hWnd);
    if (hWndParent) {
      if (IsIconic(hWndParent)) {
        GetWindowNormalRect(hWndParent, &rcParent);
      } else {
        GetWindowRect(hWndParent, &rcParent);
      }
      MONITORINFO mi = {sizeof(mi), 0};
      if (GetMonitorInfo(MonitorFromRect(rcParent, MONITOR_DEFAULTTOPRIMARY),
                         &mi)) {
        rcLimit = mi.rcWork;
      } else {
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rcLimit, 0);
      }
    } else {
      Point pt = {0};
      GetCursorPos(&pt);

      MONITORINFO mi = {sizeof(mi), 0};
      if (GetMonitorInfo(MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &mi)) {
        rcParent = mi.rcWork;
      } else {
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rcParent, 0);
      }
      rcLimit = rcParent;
    }
  }

  // 如果同时指定了left/right,则忽略width属性
  if (left != NDEF && right != NDEF) {
    s.width = rcParent.Width() - left - right;
  }
  // 如果同时指定了top/bottom，则忽略height属性
  if (top != NDEF && bottom != NDEF) {
    s.height = rcParent.Height() - top - bottom;
  }
#endif

  // 再次确认最小最大尺寸
  int minWidth = pWindow->GetMinWidth();
  int minHeight = pWindow->GetMinHeight();
  int maxWidth = pWindow->GetMinWidth();
  int maxHeight = pWindow->GetMaxHeight();
  if (minWidth > 0 && s.width < minWidth)
    s.width = minWidth;
  if (minHeight > 0 && s.height < minHeight)
    s.height = minHeight;
  if (maxWidth > 0 && s.width > maxWidth)
    s.width = maxWidth;
  if (maxHeight > 0 && s.height > maxHeight)
    s.height = maxHeight;


  SetPositionFlags flags;
  pWindow->SetObjectPos(x, y, s.width, s.height, flags);

#if 0
  // 计算出坐标
  if (left != NDEF) {
    x = rcParent.left + left;
    x += pWindow->GetMarginL();
  } else {
    if (right != NDEF) {
      x = rcParent.right - right - s.width; // right是指窗口右侧距离屏幕右侧的距离
      x -= pWindow->GetMarginR();
    } else {
      // 居中
      x = rcParent.left + (rcParent.Width() - s.width) / 2;
    }
  }
  if (top != NDEF) {
    y = rcParent.top + top;
    y += pWindow->GetMarginT();
  } else {
    if (bottom != NDEF) {
      y = rcParent.bottom - bottom - s.height; // 同right
      y -= pWindow->GetMarginB();
    } else {
      // 居中
      y = rcParent.top + (rcParent.Height() - s.height) / 2;
    }
  }

  // 限制在屏幕之内
  if (x < rcLimit.left)
    x = rcLimit.left;
  else if (x + s.width > rcLimit.right)
    x = rcLimit.right - s.width;

  if (y < rcLimit.top)
    y = rcLimit.top;
  else if (y + s.height > rcLimit.bottom)
    y = rcLimit.bottom - s.height;

  Rect rcClientOld, rcClientNew;
  GetClientRect(pWindow->m_hWnd, &rcClientOld);

  pWindow->SetObjectPos(x, y, s.width, s.height, SWP_NO_REDRAW);

  // 解决如果窗口大小没有发生改变，改变窗口没有收到WM_SIZE时，手动布局一次
  GetClientRect(pWindow->m_hWnd, &rcClientNew);
  if (rcClientNew.Width() == rcClientOld.Width() &&
      rcClientNew.Height() == rcClientOld.Height()) {
    pWindow->notify_WM_SIZE(0, rcClientNew.Width(), rcClientNew.Height());
  }

  // 递归
  // pWindow->GetLayout()->Arrange(nullptr); // <--
  // SetObjectPos会触发OnSize，在Window的OnSize消息中会进行layout处理，因此这里可以不调用
#endif
}

}