#include "window_linux_x11.h"
#include <string.h>

#include "src/graphics/skia/skia_render.h"

namespace ui {

WindowPlatformLinuxX11::WindowPlatformLinuxX11(ui::Window &w)
    : m_ui_window(w) {}
void WindowPlatformLinuxX11::Initialize() { m_display.Init(); }

WindowPlatformLinuxX11::~WindowPlatformLinuxX11() { m_display.Destroy(); }

bool WindowPlatformLinuxX11::Create(CreateWindowParam &param) {
  m_ui_window.m_dpi.SetSystemDpi(m_display.GetMonitorScaleFactor());

  assert(false);
  // TODO:
#if 0
  ::Window window =
      XCreateSimpleWindow(m_display, m_display.GetDefaultRootWindow(),
                          rect.left, rect.top, rect.width(), rect.height(), 300,
                          m_display.GetBlack(), m_display.GetWhite());

  // 必须再显示设置一次窗口位置，否则窗口一会坐标变成（0,0）
  XSizeHints hints;
  memset(&hints, 0, sizeof(hints));
  hints.x = rect.left;
  hints.y = rect.top;
  hints.width = rect.width();
  hints.height = rect.height();
  hints.flags |= USPosition | USSize | PPosition | PSize;
  XSetWMNormalHints(m_display, window, &hints);

  Attach(window);
#endif
  return true;
}

WINDOW_HANDLE WindowPlatformLinuxX11::GetWindowHandle() { return nullptr; }

bool WindowPlatformLinuxX11::CreateTransparent(const Rect &rect) {

  XVisualInfo info = {0};
  XMatchVisualInfo(m_display, m_display.GetDefaultRootWindow(), 32, TrueColor,
                   &info);

  XSetWindowAttributes attr = {0};
  attr.colormap = XCreateColormap(m_display, m_display.GetDefaultRootWindow(),
                                  info.visual, AllocNone);
  attr.border_pixel = 0;
  attr.background_pixel = 0x0; // 0x80808080;

  ::Window win = XCreateWindow(m_display, m_display.GetDefaultRootWindow(),
                               rect.left, rect.top, rect.width(), rect.height(),
                               0, info.depth, InputOutput, info.visual,
                               CWColormap | CWBorderPixel | CWBackPixel, &attr);
  this->Attach(win);
  return true;
}

void WindowPlatformLinuxX11::Destroy() {
  if (!m_window) {
    return;
  }
  ::Window window = Detach();
  // XFreeGC(m_display, gc);
  XDestroyWindow(m_display, window);
}

void WindowPlatformLinuxX11::Attach(::Window window) {
  if (!window) {
    return;
  }

  m_display.BindXEventDispatcher(window, static_cast<XEventDispatcher *>(this));

  // this->display = display_wrap.GetDisplay();
  // this->m_root_window = DefaultRootWindow(display);

  this->m_window = window;
  // s_window_mapper.RegisterWindow(this);

  this->initGC();
  this->initEvent();

  // Rect rc;
  // GetWindowRect(&rc);
  // resize(rc.width(), rc.height());

  // XFlush(display);
  // this->on_create();
}

::Window WindowPlatformLinuxX11::Detach() {
  ::Window window = m_window;

  m_display.BindXEventDispatcher(window, nullptr);

  XFreeGC(m_display, m_gc);
  m_gc = 0;
  m_window = 0;
  return window;
}

void WindowPlatformLinuxX11::initGC() {
  if (m_gc) {
    return;
  }

  XGCValues values;
  unsigned long valuemask = 0;
  m_gc = XCreateGC(m_display, m_window, valuemask, &values);

  XSetForeground(m_display, m_gc, m_display.GetBlack());
  XSetBackground(m_display, m_gc, m_display.GetBlack());
  XSetFillStyle(m_display, m_gc, FillSolid);

  // XFontStruct* font_info = XLoadQueryFont(display, "*-helvetica-*-12-*");
  // XSetFont(display, gc, font_info->fid);

  // XDrawString(display, window, gc, 0, 0, "hello", 5);
  // XFlush(display);
}

void WindowPlatformLinuxX11::initEvent() {
  long eventMask = StructureNotifyMask | ButtonReleaseMask | ButtonPressMask |
                   ExposureMask | PointerMotionMask | KeyPressMask |
                   KeyReleaseMask | FocusChangeMask | SubstructureRedirectMask;

  XSelectInput(m_display, m_window, eventMask);

  Atom protocols[] = {m_display.WM_DELETE(),
                      XInternAtom(m_display, "WM_TAKE_FOCUS", false)};
  XSetWMProtocols(m_display, m_window, protocols,
                  sizeof(protocols) / sizeof(Atom));
}

void WindowPlatformLinuxX11::Show() {
  XMapWindow(m_display, m_window);
  //   XFlush(m_display);
}

void WindowPlatformLinuxX11::Hide() { XUnmapWindow(m_display, m_window); }

struct MwmHints {
  unsigned long flags;
  unsigned long functions;
  unsigned long decorations;
  long input_mode;
  unsigned long status;
};
enum {
  MWM_HINTS_FUNCTIONS = (1L << 0),
  MWM_HINTS_DECORATIONS = (1L << 1),

  MWM_FUNC_ALL = (1L << 0),
  MWM_FUNC_RESIZE = (1L << 1),
  MWM_FUNC_MOVE = (1L << 2),
  MWM_FUNC_MINIMIZE = (1L << 3),
  MWM_FUNC_MAXIMIZE = (1L << 4),
  MWM_FUNC_CLOSE = (1L << 5)
};
void WindowPlatformLinuxX11::SetBorderless(bool no_border) {
  Atom mwmHintsProperty = XInternAtom(m_display, "_MOTIF_WM_HINTS", 0);
  struct MwmHints hints;
  hints.flags = MWM_HINTS_DECORATIONS;
  hints.decorations = 0;
  XChangeProperty(m_display, m_window, mwmHintsProperty, mwmHintsProperty, 32,
                  PropModeReplace, (unsigned char *)&hints, 5);
}

void WindowPlatformLinuxX11::Invalidate() {
  // XClearWindow(display, window);
  XClearArea(m_display, m_window, 0, 0, 1, 1, true);
  XFlush(m_display);
}

std::string WindowPlatformLinuxX11::GetWindowTitle() {
#if 0 // 方式一
    Atom type;
    int form;
    unsigned long remain, len;
    unsigned char* list;

    Atom prop = XInternAtom(m_display, "WM_NAME", False);
    XGetWindowProperty(display, window, prop, 0, 1024, False, 
        AnyPropertyType, &type, &form, &len, &remain, &list);
    if (!list) {
        return std::string();
    }

    return std::string((char*)list);
#else
  XTextProperty property;
  Atom prop = XInternAtom(m_display, "WM_NAME", False);

  // XGetTextProperty 内部会调用 XGetWindowProperty
  if (0 == XGetTextProperty(m_display, m_window, &property, prop)) {
    return std::string();
  }
  return std::string((char *)property.value);
#endif
}
void WindowPlatformLinuxX11::SetTitle(const char *title) {
#if 1
  // TBD：其它两个不支持中文？
  XTextProperty text_prop;
  if (Xutf8TextListToTextProperty(m_display, (char **)&title, 1,
                                  XUTF8StringStyle, &text_prop) == Success) {
    XSetWMName(m_display, m_window, &text_prop);
    XFree(text_prop.value);
  }
#elif 0
  XTextProperty property;
  if (0 == XStringListToTextProperty((char **)&title, 1, &property)) {
    return;
  }
  XSetWMName(m_display, m_window, &property);
#else
  XStoreName(m_display, m_window, title);
#endif
}

void WindowPlatformLinuxX11::SetMinMaxSize(int wMin, int hMin, int wMax,
                                           int hMax) {
#if 1
  XSizeHints hints;
  memset(&hints, 0, sizeof(hints));

  long ret = 0;
  XGetWMNormalHints(m_display, m_window, &hints, &ret);

  hints.min_width = wMin;
  hints.min_height = hMin;
  hints.max_width = wMax;
  hints.max_height = hMax;

  if (wMin > 0 || hMin > 0) {
    hints.flags |= PMinSize;
  } else {
    hints.flags &= ~PMinSize;
  }

  if (wMax > 0 || hMax > 0) {
    hints.flags |= PMaxSize;
  } else {
    hints.flags &= ~PMaxSize;
  }
  XSetWMNormalHints(m_display, m_window, &hints);
#endif
}

void WindowPlatformLinuxX11::SetWindowPos(int x, int y, int w, int h,
                                          SetPositionFlags flags) {
  assert(false); // TODO:
}

/**
 * 1. 只有在被Map后，才能进行窗口Move操作，否则窗口管理器将无视这些设置。
 */
void WindowPlatformLinuxX11::SetWindowRect(int x, int y, int width,
                                           int height) {
  /* 调用XSetWMNormalHints设置窗口位置不对。
  XSizeHints hints;
  memset(&hints, 0, sizeof(hints));

  long ret = 0;
  XGetWMNormalHints(display, window, &hints, &ret);

  hints.x = x;
  hints.y = y;
  hints.width = width;
  hints.height = height;
  hints.flags |= PPosition | PSize;
  XSetWMNormalHints(display, window, &hints);*/

#if 1
  // XMoveWindow(display, window, x, y);
  XMoveResizeWindow(m_display, m_window, x, y, width, height);
  // printf("resize window: %d %d,%d %d,%d\n", window, x, y, width, height);
#else
  XWindowChanges changes = {0};
  changes.x = x;
  changes.y = y;
  changes.width = width;
  changes.height = height;
  unsigned int mask = CWX | CWY | CWWidth | CWHeight;
  XConfigureWindow(m_display, m_window, mask, &changes);
#endif
}

void WindowPlatformLinuxX11::GetClientRect(Rect *prect) {
  if (!prect) {
    return;
  }

  int x, y;
  unsigned int width, height, border_width, depth;
  ::Window root;
  // Window parent_window = GetParentWindow();
  XGetGeometry(m_display, m_window, &root, &x, &y, &width, &height,
               &border_width, &depth);

  *prect = Rect::MakeXYWH(x, y, width, height);
}

void WindowPlatformLinuxX11::GetWindowRect(Rect *prect) {
  if (!prect) {
    return;
  }

  /* 不对
  XSizeHints hints;
  memset(&hints, 0, sizeof(hints));

  long ret = 0;
  XGetWMNormalHints(display, window, &hints, &ret);
  prect->left = hints.x;
  prect->top = hints.y;
  prect->right = hints.x + hints.width;
  prect->bottom = hints.y + hints.height;*/

#if 0
    Window root;
    int x;
    int y;
    unsigned int width, height, border_width, depth;

    XGetGeometry(m_display, m_window, &root, &x, &y, &width, &height, &border_width, &depth);

    Window root2, parent;
    Window* children;
    unsigned int nchildren;
    XQueryTree(m_display, m_window, &root2, &parent, &children, &nchildren);
    XFree(children);

    XWindowAttributes attr1 = { 0 };
    XGetWindowAttributes(m_display, parent, &attr1);

    XWindowAttributes attr2 = { 0 };
    XGetWindowAttributes(m_display, root2, &attr2);
#endif

  // XWindowAttributes attr = { 0 };
  // XGetWindowAttributes(display, window, &attr);

  ::Window root;
  int x, y;
  unsigned int width, height, border_width, depth;
  // Window parent_window = GetParentWindow();
  XGetGeometry(m_display, m_window, &root, &x, &y, &width, &height,
               &border_width, &depth);

  ::Window window_child;
  XTranslateCoordinates(m_display, m_window, root, x, y, &prect->left,
                        &prect->top, &window_child);

  prect->right = prect->left + width;
  prect->bottom = prect->top + height;
}

void WindowPlatformLinuxX11::CenterWindow() {
  Rect rc = {0};
  this->GetWindowRect(&rc);

  // XWidthOfScreen();
  int screen_width = m_display.GetScreenWidth();
  int screen_height = m_display.GetScreenHeight();

  int x = (screen_width - rc.width()) >> 1;
  int y = (screen_height - rc.height()) >> 1;

  this->SetWindowRect(x, y, rc.width(), rc.height());
}

::Window WindowPlatformLinuxX11::GetParentWindow() {
  ::Window parent = 0;
  ::Window root = 0;
  ::Window *children = nullptr;
  unsigned int nchildren = 0;
  XQueryTree(m_display, m_window, &root, &parent, &children, &nchildren);
  if (children) {
    XFree(children);
    children = nullptr;
  }
  return parent;
}

int get_window_depth(Display *display, ::Window window) {
  XWindowAttributes attr;
  XGetWindowAttributes(display, window, &attr);
  return attr.depth;
}

void WindowPlatformLinuxX11::Commit(ui::IRenderTarget *pRT, const Rect *prect,
                                    int count) {
  if (!m_gc) {
    return;
  }

  if (pRT->GetGraphicsRenderLibraryType() !=
      GRAPHICS_RENDER_LIBRARY_TYPE_SKIA) {
    return;
  }
  SkiaRenderTarget *skiaRT = static_cast<SkiaRenderTarget *>(pRT);
  SkSurface *surface = skiaRT->GetSkiaSurface();
  if (!surface) {
    return;
  }

  SkPixmap pm;
  if (!surface->peekPixels(&pm)) {
    return;
  }

  int bitsPerPixel = pm.info().bytesPerPixel() * 8;
  XImage image;
  memset(&image, 0, sizeof(image));
  image.width = pm.width();
  image.height = pm.height();
  image.format = ZPixmap;
  image.data = (char *)pm.addr();
  image.byte_order = LSBFirst;
  image.bitmap_unit = bitsPerPixel;
  image.bitmap_bit_order = LSBFirst;
  image.bitmap_pad = bitsPerPixel;
  image.depth = get_window_depth(m_display, m_window);
  image.bytes_per_line = pm.rowBytes() - pm.width() * pm.info().bytesPerPixel();
  image.bits_per_pixel = bitsPerPixel;
  if (!XInitImage(&image)) {
    return;
  }

  XPutImage(m_display, m_window, m_gc, &image, 0, 0, 0, 0, pm.width(),
            pm.height());
}

void WindowPlatformLinuxX11::OnXEvent(const XEvent &event) {
  if (event.type == ClientMessage) {
    XClientMessageEvent *client_event = (XClientMessageEvent *)&event;
    auto type = client_event->data.l[0];
    if (type == m_display.WM_DELETE()) {
      m_ui_window.onDestroy();
    } else if (type == m_display.WM_TAKE_FOCUS()) {
      // m_ui_window->onFocus();
    }
  } else if (event.type == Expose) {
    // 仅处理最后一个expose事件??
    if (event.xexpose.count > 0) {
      return;
    }

    // Rect dirty = {0, 0, 0, 0};
    // TODO:
    m_ui_window.onPaint(nullptr);
  } else if (event.type == ConfigureNotify) {
    // x, y 不靠谱，有时一直返回的是0
    printf("Configure Notify: %d,%d, %d %d\n", event.xconfigure.x,
           event.xconfigure.y, event.xconfigure.width, event.xconfigure.height);
    m_ui_window.onSize(event.xconfigure.width, event.xconfigure.height);
    // if (m_width != event.xconfigure.width ||
    //     m_height != event.xconfigure.height) {
    //   m_width = event.xconfigure.width;
    //   m_height = event.xconfigure.height;

    //   resize(m_width, m_height);
    //   this->on_size(event.xconfigure.width, event.xconfigure.height);
    // }
  }

#if 0
  if (event.type == ButtonRelease || event.type == ButtonPress) {
    on_button_event(event);

  } else if (event.type == MotionNotify) {
    on_mouse_move(event);
  } else if (event.type == KeyPress || event.type == KeyRelease) {
    // printf("on key event\n");
  } else if (event.type == FocusIn) {
    // printf("focus in\n");
  } else if (event.type == FocusOut) {
    // printf("focus out\n");
  } else if (event.type == ConfigureRequest) {
    XConfigureRequestEvent *request_event = (XConfigureRequestEvent *)&event;

    XWindowChanges changes = {0};
    changes.x = request_event->x;
    changes.y = request_event->y;
    changes.width = request_event->width;
    changes.height = request_event->height;
    changes.border_width = request_event->border_width;
    changes.sibling = request_event->above;
    changes.stack_mode = request_event->detail;
    unsigned int mask = request_event->value_mask;
    XConfigureWindow(display, window, mask, &changes);
    printf("XConfigureWindow window=%d, x=%d y=%d\n", request_event->width,
           changes.x, changes.y);
  }
#endif
}

void WindowPlatformLinuxX11::Invalidate(const Rect *prect) {
  // TODO:
}
bool WindowPlatformLinuxX11::IsChildWindow() {
  // TODO:
  return false;
}
bool WindowPlatformLinuxX11::IsWindowVisible() {
  // TODO:
  return true;
}
float WindowPlatformLinuxX11::GetScaleFactor() { return 2.0f; }


} // namespace ui