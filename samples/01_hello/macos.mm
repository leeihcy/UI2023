
#import "Cocoa/Cocoa.h"
#include <cstdio>

void demo1() {
  // Autorelease Pool:
  // Objects declared in this scope will be automatically
  // released at the end of it, when the pool is "drained".
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

  // Create a shared app instance.
  // This will initialize the global variable
  // 'NSApp' with the application instance.
  [NSApplication sharedApplication];

  // Style flags:
  NSUInteger windowStyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                           NSWindowStyleMaskResizable;

  NSRect windowRect = NSMakeRect(100, 100, 400, 400);
  NSWindow *window =
      [[NSWindow alloc] initWithContentRect:windowRect
                                  styleMask:windowStyle
                                    backing:NSBackingStoreBuffered
                                      defer:NO];
  [window autorelease];

  // Window controller:
  NSWindowController *windowController =
      [[NSWindowController alloc] initWithWindow:window];
  [windowController autorelease];

  // This will add a simple text view to the window,
  // so we can write a test string on it.
  NSTextView *textView = [[NSTextView alloc] initWithFrame:windowRect];
  [textView autorelease];

  [window setContentView:textView];
  [textView insertText:@"Hello OSX/Cocoa world!"];

  // TODO: Create app delegate to handle system events.
  // TODO: Create menus (especially Quit!)

  // Show window and run event loop.
  [window orderFrontRegardless];
  [NSApp run];

  [pool drain];
}

@interface MainView : NSView
- (MainView *)initWithWindow:(NSWindow*)initWindow;
@end

@implementation MainView {
  NSWindow *m_window;
}
- (MainView *)initWithWindow:(NSWindow*)initWindow {
  self = [super init];
  m_window = initWindow;
  return self;
}

- (void)drawRect:(NSRect)rect {
  printf("Drawing in MainView: %f x %f\n",
         self.bounds.size.width, self.bounds.size.height);

  // Fill the view with a light gray color.
  for (int i = 0; i < 20; i++) {
      [[NSColor greenColor] setFill];
      NSRectFill(self.bounds);
  }
  

  // Draw hello text.
  NSString *helloText = @"Hello, World!";
  NSDictionary *attributes = @{
    NSFontAttributeName: [NSFont systemFontOfSize:24],
    NSForegroundColorAttributeName: [NSColor blackColor]
  };
  NSSize textSize = [helloText sizeWithAttributes:attributes];
  NSPoint textPoint = NSMakePoint(
      (self.bounds.size.width - textSize.width) / 2,
      (self.bounds.size.height - textSize.height) / 2); // Center the text
  [helloText drawAtPoint:textPoint withAttributes:attributes];

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
- (void)windowDidResize:(NSNotification *)notification {
  printf("Window did resize: %f x %f\n",
         m_window.contentView.bounds.size.width,
         m_window.contentView.bounds.size.height);
}
- (void)windowWillStartLiveResize:(NSNotification *)notification {
  printf("Window will start live resize: %f x %f\n",
         m_window.contentView.bounds.size.width,
         m_window.contentView.bounds.size.height);
}
- (void)windowDidEndLiveResize:(NSNotification *)notification {
  printf("Window did end live resize: %f x %f\n",
         m_window.contentView.bounds.size.width,
         m_window.contentView.bounds.size.height);
}
- (BOOL)windowShouldClose:(NSWindow *)sender {
  return FALSE;
}
@end
void demo2() {
  //   [NSAutoreleasePool new];
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
  [[m_window contentView] addSubview:button];


  id menubar = [[NSMenu new] autorelease];
  id appMenuItem = [[NSMenuItem new] autorelease];
  [menubar addItem:appMenuItem];
  [NSApp setMainMenu:menubar];
  id appMenu = [[NSMenu new] autorelease];
  id appName = [[NSProcessInfo processInfo] processName];
  id quitTitle = [@"Quit " stringByAppendingString:appName];
  id quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle
                                                action:@selector(terminate:)
                                         keyEquivalent:@"q"] autorelease];
  [appMenu addItem:quitMenuItem];
  [appMenuItem setSubmenu:appMenu];

  [app activateIgnoringOtherApps:YES];
  [app run];
}

int main() {
  demo2();
  return 0;
}