#ifndef _UI_SDK_SRC_LAYOUT_DESKTOP_LAYOUT_H_
#define _UI_SDK_SRC_LAYOUT_DESKTOP_LAYOUT_H_

namespace ui {
class Window;

// 桌面布局管理器，专门用于设置顶层窗口在屏幕中的大小与位置
// 桌面布局有些特殊，它直接管理窗口的位置，但窗口和控件、Panel有很多不一样的地方，
// 如窗口分客户区和非客户区。而且DesktopLayout只需要管理一个对象，即Window，没有其他
// 子对象，因此这里不将DesktopLayout派生自LayoutManager
class DesktopLayout {
public:
  void Arrange(Window *pWindow);
};

}

#endif