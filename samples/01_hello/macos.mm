
#import "Cocoa/Cocoa.h"

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