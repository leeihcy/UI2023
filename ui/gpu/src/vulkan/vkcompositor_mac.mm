#import "Cocoa/Cocoa.h"
#import "QuartzCore/CAMetalLayer.h"

void* GetNSWindowRootView(void* window) {
  NSView* view = ((NSWindow*)window).contentView;

  [view setWantsLayer:YES];
  view.layer = [CAMetalLayer layer];
  return (void*)(view);
}

// 直接获取capabilities.currentExtent.width，返回的是缩放后的尺寸。已经除了缩放系数（200%）
// 我们这里要直接获取实际的尺寸。
void GetNSWindowActureSize(void* window_, int* width, int* height) {
  NSWindow* window = ((NSWindow*)window_);
  if (width) {
    *width = window.contentLayoutRect.size.width *
                                   window.backingScaleFactor;
  }
  if (height) {
    *height = window.contentLayoutRect.size.height *
                                   window.backingScaleFactor;
  }
}