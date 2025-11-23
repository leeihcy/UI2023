#include <QuartzCore/QuartzCore.h>
#import "Appkit/NSWindow.h"
#import "Appkit/NSScreen.h"
#import "Metal/MTLDevice.h"
#include <string>

namespace ui {

// 获取窗口所以屏幕对应的显卡名称。
std::string GetNSViewMetalDeviceName(/*NSView*/void *view) {
  NSWindow* window = ((NSView*)view).window;
  if (!window) {
    return "";
  }
  NSScreen *screen = window.screen;
  if (!screen) {
    return "";
  }

  CGDirectDisplayID view_display_id =
      [screen.deviceDescription[@"NSScreenNumber"] unsignedIntegerValue];

  // TBD: 这个函数是否太重了。
  id<MTLDevice> device = CGDirectDisplayCopyCurrentMetalDevice(view_display_id);
  if (!device) {
    return "";
  }
  std::string name(device.name.UTF8String);
  [device release];
  return name;
}
}