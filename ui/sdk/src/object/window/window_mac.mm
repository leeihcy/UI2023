#include "window_mac.h"
#import "Cocoa/Cocoa.h"
#include <string.h>

namespace ui {

void WindowPlatformMac::Initialize(ui::Window *ui_window) {
  m_ui_window = ui_window;
}

void WindowPlatformMac::Release() { Destroy(); }

void WindowPlatformMac::Create(const Rect &rect) {
  NSUInteger windowStyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                           NSWindowStyleMaskResizable;

  NSRect windowRect = NSMakeRect(100, 100, 400, 400);
  m_window = [[NSWindow alloc] initWithContentRect:windowRect
                                         styleMask:windowStyle
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
//   [m_window autorelease];

  // Window controller:
//   NSWindowController *windowController =
//       [[NSWindowController alloc] initWithWindow:m_window];
//   [windowController autorelease];

//   NSTextView *textView = [[NSTextView alloc] initWithFrame:windowRect];
//   [textView autorelease];
//   [m_window setContentView:textView];
//   [textView insertText:@"Hello OSX/Cocoa world!" replacementRange:NSRange()];
printf("create window\n");
} 

void WindowPlatformMac::Destroy() {}

void WindowPlatformMac::Show() { 
    // [m_window orderFrontRegardless]; 
    // [[NSApp mainWindow] makeKeyAndOrderFront:nil];

    [m_window makeKeyAndOrderFront:nil];
  printf("Show window\n");
}

} // namespace ui