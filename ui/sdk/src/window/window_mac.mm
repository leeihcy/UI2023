#include "window_mac.h"
#include "../../../3rd/skia/src/include/core/SkBitmap.h"
#include "../../../3rd/skia/src/include/utils/mac/SkCGUtils.h"
#include "../../../3rd/skia/src/src/utils/mac/SkUniqueCFRef.h"
#import "Cocoa/Cocoa.h"
#include "src/graphics/skia/skia_render.h"
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
WindowPlatformMac::~WindowPlatformMac() {
}

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

  // 主动触发一次，初始化相关render target
  notifySize();

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

void WindowPlatformMac::GetClientRect(Rect *prect) {
  prect->left = m_window.contentLayoutRect.origin.x;
  prect->right = m_window.contentLayoutRect.origin.y;
  prect->right = prect->left + m_window.contentLayoutRect.size.width;
  prect->bottom = prect->top + m_window.contentLayoutRect.size.height;
}
void WindowPlatformMac::GetWindowRect(Rect *prect) {
  prect->left = m_window.frame.origin.x;
  prect->right = m_window.frame.origin.y;
  prect->right = prect->left + m_window.frame.size.width;
  prect->bottom = prect->top + m_window.frame.size.height;
}
void SetWindowRect(Rect *prect) {
  // // Given CG and NS's coordinate system, the "Y" position of a window is the
  // Y coordinate
  // // of the bottom of the window.
  // int newBottom    = (int)([mWindow frame].size.height) + y;
  // NSRect emptyRect = NSMakeRect(x, YCoordToFromCG(newBottom), 0, 0);
  // [mWindow setFrameOrigin:[mWindow
  // frameRectForContentRect:emptyRect].origin];

  // [mWindow setContentSize:NSMakeSize(width, height)];
  // return true;
}
void WindowPlatformMac::Invalidate(const Rect *prect) {
  if (!prect) {
    [m_window.contentView display];
    return;
  }
  NSRect rect;
  rect.origin.x = prect->left;
  rect.origin.y = prect->top;
  rect.size.width = prect->width();
  rect.size.height = prect->height();
  [m_window.contentView displayRect:rect];
}

bool WindowPlatformMac::IsChildWindow() { return m_window.parentWindow != nil; }
bool WindowPlatformMac::IsWindowVisible() { return !!m_window.visible; }

void WindowPlatformMac::Show() {
  [m_window orderFront:nil];

  [NSApp activateIgnoringOtherApps:YES];
  [m_window makeKeyAndOrderFront:NSApp];
}
void WindowPlatformMac::Hide() { [m_window orderOut:nil]; }

// void WindowPlatformMac::Submit(sk_sp<SkSurface> sksurface) {
void WindowPlatformMac::Commit(IRenderTarget *pRT, const Rect *prect,
                               int count) {
  CGContext *context = [NSGraphicsContext currentContext].CGContext;
  if (!context) {
    return;
  }

  if (pRT->GetGraphicsRenderLibraryType() ==
      GRAPHICS_RENDER_LIBRARY_TYPE_SKIA) {
    SkiaRenderTarget *skiaRT = static_cast<SkiaRenderTarget *>(pRT);
    SkSurface *surface = skiaRT->GetSkiaSurface();
    if (!surface) {
      return;
    }

    // printf("2\n");
    // SkBitmap bm;
    // surface->readPixels(bm, 0, 0);
    // printf("3\n");
    // // SkCGDrawBitmap(context, bm, 0, 0);
    // SkUniqueCFRef<CGImageRef> img(SkCreateCGImageRef(bm));

    // int x = 0;
    // int y = 0;
    // auto* cg = context;
    // if (img) {
    //     CGRect r = CGRectMake(0, 0, bm.width(), bm.height());

    //     CGContextSaveGState(cg);
    //     CGContextTranslateCTM(cg, x, r.size.height + y);
    //     CGContextScaleCTM(cg, 1, -1);

    //     CGContextDrawImage(cg, r, img.get());

    //     CGContextRestoreGState(cg);
    // }

    SkPixmap pm;
    if (!surface->peekPixels(&pm)) {
      return;
    }

    CGDataProviderRef ref = CGDataProviderCreateWithData(
        NULL, (char *)pm.addr(), pm.rowBytes() * pm.height(), NULL);
    CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();

    CGBitmapInfo bitmapInfo = kCGImageAlphaNoneSkipLast;
    //  bitmapInfo = kCGBitmapByteOrder32Big | kCGImageAlphaNoneSkipFirst;
    bitmapInfo = kCGBitmapByteOrder32Little | kCGImageAlphaNoneSkipFirst;

    CGImageRef image = CGImageCreate(
        pm.width(), pm.height(), 8, 32, pm.width() * 4, colorspace, bitmapInfo,
        ref, NULL, true, kCGRenderingIntentDefault);

    for (int i = 0; i < count; i++) {
      const Rect &rc = prect[i];
      NSRect nsrect;
      nsrect.origin.x = rc.left;
      nsrect.origin.y = rc.top;
      nsrect.size.width = rc.right - rc.left;
      nsrect.size.height = rc.bottom - rc.top;
      CGImageRef part_image = CGImageCreateWithImageInRect(image, nsrect);

      static int count = 0;
      // count ++;
      if (count < 100) {
        CGContextDrawImage(context,
                           nsrect, // CGRectMake(0, 0, pm.width(), pm.height()),
                           part_image);
      }
      CGImageRelease(part_image);
    }
    CGImageRelease(image);
  }
}

void WindowPlatformMac::notifySize() {
  m_ui_window.onSize(m_window.frame.size.width, m_window.frame.size.height);
}

// NS's and CG's coordinate systems start at the bottom left, while OSWindow's
// coordinate system starts at the top left. This function converts the Y
// coordinate accordingly. static float YCoordToFromCG(float y)
// {
//     float screenHeight = CGDisplayBounds(CGMainDisplayID()).size.height;
//     return screenHeight - y;
// }

} // namespace ui

@implementation WindowDelegate {
  ui::WindowPlatformMac *m_window;
}
- (WindowDelegate *)initWithWindow:(ui::WindowPlatformMac *)initWindow {
  m_window = initWindow;
  return self;
}

- (void)windowDidResize:(NSNotification *)notification {
  NSView *view = m_window->window().contentView;
  // CGFloat scale = skwindow::GetBackingScaleFactor(view);
  // m_window->m_ui_window.onSize(view.bounds.size.width /* * scale*/,
  //                              view.bounds.size.height /* * scale*/);
  m_window->notifySize();
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
  // CGContext* context = UIGraphicsGetCurrentContext();
  // [NSGraphicsContext currentContext].CGContext
  // CGContextRef myContext = [[NSGraphicsContext currentContext] graphicsPort];

  //  CGContextRef ctx =
  //       (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
  ui::Rect r = {(int)rect.origin.x, (int)rect.origin.y, (int)rect.size.width,
                (int)rect.size.height};
  m_window->m_ui_window.onPaint(&r);
}

@end