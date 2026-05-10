
## 主窗口初始化过程

```
 	chrome.dll!BrowserFrameView::BrowserFrameView
 	chrome.dll!BrowserFrameViewWin::BrowserFrameViewWin
 	chrome.dll!std::__Cr::make_unique<...>
 	chrome.dll!chrome::`anonymous namespace'::CreateBrowserFrameViewWin
 	chrome.dll!chrome::CreateBrowserFrameView
 	chrome.dll!BrowserWidget::CreateFrameView
 	ui_views.dll!views::Widget::Init
 	chrome.dll!BrowserWidget::InitBrowserWidget
>	chrome.dll!BrowserWindow::CreateBrowserWindow
 	chrome.dll!Browser::Browser
 	chrome.dll!Browser::Create
```

### 首先创建 Browser 对象。

```cpp
class Browser {

  // This Browser's window.
  std::unique_ptr<BrowserWindow> window_;

}
```

### 调用 BrowserWindow::CreateBrowserWindow 构造窗口类

```cpp
namespace ui {
// ui\base\base_window.h
// 纯虚类。
class BaseWindow {
};
}

// chrome\browser\ui\browser_window.h
// 虚类。
class BrowserWindow : public ui::BaseWindow {

  // chrome\browser\ui\views\frame\browser_window_factory.cc
  static std::unique_ptr<BrowserWindow> CreateBrowserWindow(Browser* browser) {
    BrowserView* view = new BrowserView(browser);
    auto browser_widget = std::make_unique<BrowserWidget>(view);
    view->set_browser_widget(std::move(browser_widget));

    view->browser_widget()->InitBrowserWidget();

    return std::unique_ptr<BrowserWindow>(view);
  }
}

```

BrowserWindow是一个虚类，由BrowserView实现。（奇怪的命名）

```cpp
// chrome\browser\ui\views\frame\browser_view.h
class BrowserView : 
    public BrowserWindow, 
    public views::ClientView {
      
  // The owning Browser object. `browser_` will outlive this.
  const raw_ptr<Browser> browser_;

  // The BrowserWidget that owns this view.
  std::unique_ptr<BrowserWidget> browser_widget_;
}
```

BrowserWindow保存在Browser的window_成员中。

```cpp
class Browser {
  BrowserView& Browser::GetBrowserView() {
    return window_->AsBrowserView();
  }

  // This Browser's window.
  std::unique_ptr<BrowserWindow> window_;
}
```

### NativeWidget 初始化


```cpp
class BrowserWidget : public views::Widget {
  void InitBrowserWidget() {
    browser_native_widget_ =
      BrowserNativeWidgetFactory::CreateBrowserNativeWidget(this,
                                                           browser_view_);

    // return new BrowserNativeWidgetAura(browser_widget, browser_view);
  }
  
  raw_ptr<BrowserNativeWidget> browser_native_widget_;
}

```

browser_native_widget也会透传到 Widget 类中。Widget直接复用这个对象，不会再单独创建一个NativeWidget.


### WindowTreeHost 初始化

```cpp
class BrowserNativeWidgetAura : public views::DesktopNativeWidgetAura,
                                public BrowserNativeWidget {
  void InitNativeWidget(
      views::Widget::InitParams params) {
    browser_desktop_window_tree_host_ =
        BrowserDesktopWindowTreeHost::CreateBrowserDesktopWindowTreeHost(
            browser_widget_, this, browser_view_, browser_widget_);
  }
}

class BrowserDesktopWindowTreeHostWin
    : public BrowserDesktopWindowTreeHost,
      public views::DesktopWindowTreeHostWin {

}
```

BrowserNativeWidgetAura 管“Widget语义和浏览器行为”
DesktopWindowTreeHost 管“系统窗口和事件入口”

BrowserWidget
    ↓
BrowserNativeWidgetAura
    ↓
DesktopNativeWidgetAura
    ↓
DesktopWindowTreeHost
    ↓
aura::WindowTreeHost
    ↓
OS Window(HWND/NSWindow)


### TreeHost是什么

Aura窗口树的宿主。
Aura里所有窗口是树结构：

RootWindow
 ├─ Window1
 ├─ Window2
 │   └─ ChildWindow
 └─ Window3

这个树必须“挂”在某个真正的系统窗口上。
它主要负责持有 RootWindow，就是根节点。  （aura::Window*）


> 对象模型
> https://www.chromium.org/developers/design-documents/aura/


Widget是一个跨平台类型，依赖于NativeWidget实现。例如在Windows上，NativeWidgetWin封装了一个HWND，NativeWidgetGtk封装了GtkWidget。NativeWidget将平台上的消息转换成跨平台的 views::Events。

aura::Window代表虚拟窗口。每个aura::Window会分配一个Layer。

一个实体HWND下，可以包含多个aura::Window，这个aura::Window以树的形式组织起来，例如：`window2.AddChild(&window3);`

NativeWidgetAura作用：推出新的aura窗口架构后，为了兼容之前的view体系，用NativeWidgetAura 用为 aura::Window的delegate，接收aura::Event，传递给view体系。aura将完全负责整个窗口体系、事件分发、焦点、激活等。

aura::WindowTreeHost中的window_是一个 aura::Window，代表一个 aura RootWindow。而 DesktopNativeWidgetAura中创建的另一个aura::Window，是一个content window，作为 root window的子窗口。二者的关系如下代码：

例如Ctrl+F弹出的搜索栏，就是一个aura::Window，它不会对应一个HWND。它创建的是NativeWidgetAura，不是DesktopNativeWidgetAura。

```
>	ui_aura.dll!aura::Window::Window
 	ui_views.dll!views::NativeWidgetAura::NativeWidgetAura
 	chrome.dll!CreateNativeWidget
 	chrome.dll!ChromeViewsDelegate::CreateNativeWidget
 	chrome.dll!ChromeViewsDelegate::OnBeforeWidgetInit
 	ui_views.dll!views::Widget::Init
 	chrome.dll!FindBarHost::FindBarHost
```


### 类结构图

```

                      Browser
                    ^  
                    |        |
         (browser_) |        |（window_）
                    |        |
                    |        |
                             V
            BrowserView 
                    ├──>  BrowserWindow
                    |              └──>  ui::BaseWindow
                    └──>  views::ClientView 

                |
                |
                |（browser_widget_）      
                |
                V
                BrowserWidget 
                           └──> views::Widget       ---------(root_view_)------------->   views::internal::RootView

                         |         |
                         |         |
(browser_native_widget_) |         | (native_widget_)
                         |         |
                         V         V
                BrowserNativeWidgetAura
                                ├──> views::DesktopNativeWidgetAura
                                |                 └──> internal::NativeWidgetPrivate
                                |                                       └──> views::NativeWidget
                                └──> BrowserNativeWidget
                       |
                       |
  (browser_desktop_window_tree_host_)
                       |
                       V
                BrowserDesktopWindowTreeHostWin
                                ├──> BrowserDesktopWindowTreeHost,
                                └──> views::DesktopWindowTreeHostWin
                                                     ├──> DesktopWindowTreeHost (纯虚类)
                                                     ├──> aura::WindowTreeHost
                                                     └──> HWNDMessageHandlerDelegate







      BrowserWidget
           |
           |
     (browser_frame_view_)
           |
           V
      BrowserFrameViewWin
          └──> BrowserFrameView
                  └──> views::FrameView
                          └──> views::View


// 嵌入式的子窗口（不对应一个HWND）。
views::NativeWidgetAura
           └──> internal::NativeWidgetPrivate
                               └──> views::NativeWidget

```


