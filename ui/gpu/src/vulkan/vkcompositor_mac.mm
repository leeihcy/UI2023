#import "Cocoa/Cocoa.h"
#import "QuartzCore/CAMetalLayer.h"

void* GetNSWindowRootView(void* window) {
  NSView* view = ((NSWindow*)window).contentView;

  [view setWantsLayer:YES];
  view.layer = [CAMetalLayer layer];
  return (void*)(view);
}