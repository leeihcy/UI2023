
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

int main() {
  demo1();
  return 0;
}