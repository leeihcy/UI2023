#include "window_mac.h"
#import "Cocoa/Cocoa.h"
#include <string.h>

@interface WindowDelegate : NSObject <NSWindowDelegate>

- (WindowDelegate *)initWithWindow:(ui::WindowPlatformMac *)initWindow;

@end

@interface MainView : NSView

- (MainView *)initWithWindow:(ui::WindowPlatformMac *)initWindow;

@end

namespace ui {

WindowPlatformMac::WindowPlatformMac(ui::Window &w) : m_ui_window(w) {}
void WindowPlatformMac::Initialize() {}

void WindowPlatformMac::Release() { Destroy(); }

bool WindowPlatformMac::Create(const Rect &rect) {
  NSUInteger windowStyle =
      (NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
       NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable);
  NSRect windowRect = NSMakeRect(100, 100, 400, 400);
  m_window = [[NSWindow alloc] initWithContentRect:windowRect
                                         styleMask:windowStyle
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
  WindowDelegate *delegate = [[WindowDelegate alloc] initWithWindow:this];

  // create view
  MainView *view = [[MainView alloc] initWithWindow:this];
  if (nil == view) {
    [m_window release];
    [delegate release];
    return false;
  }

  [m_window setContentView:view];
  [m_window makeFirstResponder:view];
  [m_window setDelegate:delegate];
  [m_window setAcceptsMouseMovedEvents:YES];
  [m_window setRestorable:NO];

  // Window controller:
  //   NSWindowController *windowController =
  //       [[NSWindowController alloc] initWithWindow:m_window];
  //   [windowController autorelease];

  //   NSTextView *textView = [[NSTextView alloc] initWithFrame:windowRect];
  //   [textView autorelease];
  //   [m_window setContentView:textView];
  //   [textView insertText:@"Hello OSX/Cocoa world!"
  //   replacementRange:NSRange()];
  return true;
}

void WindowPlatformMac::Destroy() {}

void WindowPlatformMac::SetTitle(const char *title) {
  NSString *text = [NSString stringWithUTF8String:title];
  if (text) {
    [m_window setTitle:text];
  }
}

void WindowPlatformMac::Show() {
  [m_window orderFront:nil];

  [NSApp activateIgnoringOtherApps:YES];
  [m_window makeKeyAndOrderFront:NSApp];
}


void WindowPlatformMac::Submit(sk_sp<SkSurface> sksurface) 
{

}

} // namespace ui

@implementation WindowDelegate {
  ui::WindowPlatformMac *m_window;
}
- (WindowDelegate *)initWithWindow:(ui::WindowPlatformMac *)initWindow {
  m_window = initWindow;
  return self;
}

- (void)windowDidResize:(NSNotification *)notification {
    NSView* view = m_window->window().contentView;
    // CGFloat scale = skwindow::GetBackingScaleFactor(view);
    m_window->m_ui_window.onSize(view.bounds.size.width/* * scale*/, 
        view.bounds.size.height/* * scale*/);
    // fWindow->inval();
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
  m_window->m_ui_window.onDestroy();
  return FALSE;
}
@end

@implementation MainView {
  ui::WindowPlatformMac *m_window;
}

- (MainView *)initWithWindow:(ui::WindowPlatformMac *)initWindow {
  self = [super init];

  m_window = initWindow;
  return self;
}

- (BOOL)isOpaque {
  return YES;
}

- (BOOL)canBecomeKeyView {
  return YES;
}

- (BOOL)acceptsFirstResponder {
  return YES;
}

- (void)drawRect:(NSRect)rect {
  m_window->m_ui_window.onPaint(nullptr);
}

@end