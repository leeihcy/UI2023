
#import "Cocoa/Cocoa.h"
#import <QuartzCore/CAMetalLayer.h>
#import <MetalKit/MTKView.h>
#include <cstdio>

@interface MainViewDelegate : NSObject <MTKViewDelegate>

@end
@implementation MainViewDelegate {
}
-(void)drawInMTKView:(MTKView *)view {

}
-(void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {

}
@end

@interface MainView : MTKView
- (MainView *)initWithWindow:(NSWindow*)initWindow;
@end

@implementation MainView {
  NSWindow *m_window;
}
- (MainView *)initWithWindow:(NSWindow*)initWindow {
  self = [super init];
  m_window = initWindow;

  MainViewDelegate *delegate = [[MainViewDelegate alloc] init];
  self.delegate = delegate;

  self.enableSetNeedsDisplay = YES;
  self.device = MTLCreateSystemDefaultDevice();
  self.clearColor = MTLClearColorMake(0, 1, 0, 1.0);

  return self;
}
@end


@interface WindowDelegate : NSObject <NSWindowDelegate>
- (WindowDelegate *)initWithWindow:(NSWindow *)initWindow;
- (void)windowDidResize:(NSNotification *)notification;
- (void)windowWillStartLiveResize:(NSNotification *)notification;
- (void)windowDidEndLiveResize:(NSNotification *)notification;
@end
@implementation WindowDelegate {
  NSWindow *m_window;
}
- (WindowDelegate *)initWithWindow:(NSWindow*)initWindow {
  m_window = initWindow;
  return self;
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
  return FALSE;
}
@end
void demo_metal() {
  id app = [NSApplication sharedApplication];
  [app setActivationPolicy:NSApplicationActivationPolicyRegular];

  NSRect frame = NSMakeRect(0, 0, 300, 300);
  NSUInteger windowStyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                           NSWindowStyleMaskResizable;

  NSRect windowRect = NSMakeRect(100, 100, 400, 400);
  NSWindow *m_window =
      [[NSWindow alloc] initWithContentRect:windowRect
                                  styleMask:windowStyle
                                    backing:NSBackingStoreBuffered
                                      defer:NO];

  [m_window cascadeTopLeftFromPoint:NSMakePoint(10, 10)];
  [m_window setTitle:@"Hello"];
  [m_window makeKeyAndOrderFront:nil];

  MainView *view = [[MainView alloc] initWithWindow:m_window];
  [m_window setContentView:view];

  WindowDelegate *delegate = [[WindowDelegate alloc] initWithWindow:m_window];
  [m_window setDelegate:delegate];

  id button = [[[NSButton alloc] initWithFrame:frame] autorelease];
  [button setTarget:app];
  [button setAction:@selector(terminate:)];
  [button setTitle:@"Quit"];
  [button setFrame:NSMakeRect(0, 0, 100, 40)];
  [[m_window contentView] addSubview:button];

  [app activateIgnoringOtherApps:YES];
  [app run];
}

int main() {
  demo_metal();
  return 0;
}