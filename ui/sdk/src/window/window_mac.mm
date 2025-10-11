#include "window_mac.h"
#import "Cocoa/Cocoa.h"
#import "QuartzCore/CAMetalLayer.h"
#include "include/util/rect.h"
#include "src/graphics/skia/skia_render.h"
#include "third_party/skia/src/include/core/SkBitmap.h"
#include "third_party/skia/src/include/utils/mac/SkCGUtils.h"
#include "third_party/skia/src/src/utils/mac/SkUniqueCFRef.h"
#include <cassert>
#include <string.h>

@interface WindowDelegate : NSObject <NSWindowDelegate>

- (WindowDelegate *)initWithWindow:(ui::WindowPlatformMac *)initWindow;
- (void)windowDidResize:(NSNotification *)notification;
- (void)windowWillStartLiveResize:(NSNotification *)notification;
- (void)windowDidEndLiveResize:(NSNotification *)notification;
@end

@interface MainView : NSView

- (MainView *)initWithWindow:(ui::WindowPlatformMac *)initWindow;

@end

namespace ui {

WindowPlatformMac::WindowPlatformMac(ui::Window &w) : m_ui_window(w) {}
void WindowPlatformMac::Initialize() {}
WindowPlatformMac::~WindowPlatformMac() {}

// rect乘以了缩放系数。
bool WindowPlatformMac::Create(CreateWindowParam &param) {
  NSUInteger windowStyle =
      (NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
       NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable);

  float scale = [[NSScreen mainScreen] backingScaleFactor];
  m_ui_window.m_dpi.SetSystemDpi(scale);

  ui::Rect content_rect = ui::Rect::MakeXYWH(0, 0, 400, 400);
  if (param.position) {
    content_rect.Set(param.x, param.y, param.x + param.w, param.y + param.h);
  }

  NSRect ns_content_rect;
  if (param.is_pixel_unit) {
    ns_content_rect =
        NSMakeRect(content_rect.left / scale, content_rect.top / scale,
                   content_rect.Width() / scale, content_rect.Height() / scale);
  } else {
    ns_content_rect = NSMakeRect(content_rect.left, content_rect.top,
                                 content_rect.Width(), content_rect.Height());
  }

  if (!param.is_client_size) {
    // 指定的是窗口（包含标题栏）的大小，转换成客户区大小。
    ns_content_rect = [NSWindow contentRectForFrameRect:ns_content_rect
                                              styleMask:windowStyle];
  }

  // content rect，不带scale。
  m_window = [[NSWindow alloc] initWithContentRect:ns_content_rect
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

  // TBD:
  // view.wantsLayer = YES;
  // view.layer = [CALayer layer];

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
void WindowPlatformMac::PostCreate() {
  // 主动触发一次，初始化相关render target/invalidate rect
  notifySize();
}

void WindowPlatformMac::Destroy() {}

WINDOW_HANDLE WindowPlatformMac::GetWindowHandle() { return m_window; }
WINDOW_HANDLE WindowPlatformMac::GetRootViewHandle() {
  return m_window.contentView;
}

void WindowPlatformMac::SetTitle(const char *title) {
  NSString *text = [NSString stringWithUTF8String:title];
  if (text) {
    [m_window setTitle:text];
  }
}

void WindowPlatformMac::GetClientRect(Rect *prect) {
  prect->left = m_window.contentView.bounds.origin.x;
  prect->top = m_window.contentView.bounds.origin.y;
  prect->right = prect->left + m_window.contentView.bounds.size.width *
                                   m_window.backingScaleFactor;
  prect->bottom = prect->top + m_window.contentView.bounds.size.height *
                                   m_window.backingScaleFactor;
}
void WindowPlatformMac::GetWindowRect(Rect *prect) {
  prect->left = m_window.frame.origin.x;
  prect->top = m_window.frame.origin.y;
  prect->right =
      prect->left + m_window.frame.size.width * m_window.backingScaleFactor;
  prect->bottom =
      prect->top + m_window.frame.size.height * m_window.backingScaleFactor;
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

void WindowPlatformMac::UpdateNonClientRegion(Rect *pregion) {
  NSRect client_rect = NSMakeRect(100, 100, 100, 100);
  NSRect frame_rect = [m_window frameRectForContentRect:client_rect];

  int left = client_rect.origin.x - frame_rect.origin.x;
  int right = (frame_rect.origin.x + frame_rect.size.width -
               (client_rect.origin.x + client_rect.size.width));
  int top = client_rect.origin.y - frame_rect.origin.y;
  int bottom = (frame_rect.origin.y + frame_rect.size.height -
                (client_rect.origin.y + client_rect.size.height));

  pregion->left -= left * m_window.backingScaleFactor;
  pregion->top -= top * m_window.backingScaleFactor;
  pregion->right += right * m_window.backingScaleFactor;
  pregion->bottom += bottom * m_window.backingScaleFactor;
}

// xywh 乘以了缩放系数
void WindowPlatformMac::SetWindowPos(int x, int y, int w, int h,
                                     SetPositionFlags flags) {
  // printf("SetWindowPos %d,%d  %d,%d \n", x,y,w,h);
  if (!flags.move && !flags.size) {
    return;
  }

  CGFloat factor = m_window.backingScaleFactor;
  NSRect rect = NSMakeRect(0, 0, w / factor, h / factor);

  NSUInteger windowStyle =
      (NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
       NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable);

  NSRect frame_rect;
  if (flags.is_window_client_size) {
    frame_rect = [m_window frameRectForContentRect:rect];
  } else {
    frame_rect = rect;
  }

  // printf("SetWindowPos rect2 %f,%f  %f,%f \n",
  //   frame_rect.origin.x, frame_rect.origin.y,
  //   frame_rect.size.width, frame_rect.size.height);

  if (flags.size && flags.move) {
    //[m_window setFrame:NSMakeRect(x/factor, y/factor, w/factor, h/factor)
    // display:(true)];
    [m_window setFrame:NSMakeRect(x / factor, y / factor, frame_rect.size.width,
                                  frame_rect.size.height)
               display:(true)];
    return;
  }

  if (flags.size) {
    // [m_window setContentSize:NSMakeSize(w/factor, h/factor)];
    [m_window setContentSize:NSMakeSize(frame_rect.size.width,
                                        frame_rect.size.height)];
    return;
  }
  if (flags.move) {
    [m_window setFrameOrigin:NSMakePoint(x / factor, y / factor)];
    return;
  }
}

void WindowPlatformMac::GetMonitorWorkArea(Rect *rect) {
  if (!rect) {
    return;
  }

  NSScreen *screen = [m_window screen];
  if (!screen) {
    return;
  }
  NSRect rc_workarea = [screen visibleFrame];
  CGFloat factor = m_window.backingScaleFactor;

  *rect = Rect::MakeXYWH(
      rc_workarea.origin.x * factor, rc_workarea.origin.y * factor,
      rc_workarea.size.width * factor, rc_workarea.size.height * factor);
}

#if 0
void WindowPlatformMac::Invalidate(const Rect *prect) {
  return;
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
#endif

bool WindowPlatformMac::IsChildWindow() { return m_window.parentWindow != nil; }
bool WindowPlatformMac::IsWindowVisible() { return !!m_window.visible; }

void WindowPlatformMac::Show(bool activate) {
  [m_window orderFront:nil];

  if (activate) {
    [NSApp activateIgnoringOtherApps:YES];
  }
  [m_window makeKeyAndOrderFront:NSApp];
}
void WindowPlatformMac::Hide() { [m_window orderOut:nil]; }

// float WindowPlatformMac::GetScaleFactor() {
//   return m_window.backingScaleFactor;
// }


//
// CGContextDrawImage 属于 Core Graphics (Quartz)，
// 它不关心 NSView 的翻转状态，始终按照 左下角原点 绘制。
// 
void WindowPlatformMac::Commit2(const FrameBuffer& fb, const RectRegion &dirty_region_px) {
int client_width = m_window.contentView.bounds.size.width;
  int client_height = m_window.contentView.bounds.size.height;

  CGContext *context = [NSGraphicsContext currentContext].CGContext;

  // 目前不是处于drawRect调用中，调用displayRect触发窗口刷新。
  if (!context) {
    for (int i = 0; i < dirty_region_px.Count(); i++) {
      ui::Rect recti = dirty_region_px.RectPtr2()[i];
      m_ui_window.m_dpi.RestoreRect(&recti);
      NSRect rect = NSMakeRect(recti.left,
                               // 转换成左下角原点
                               client_height - recti.top - recti.height(),
                               recti.width(), recti.height());
      [m_window.contentView displayRect:rect];
    }
    return;
  }

  // CGContextSaveGState(context);
  // CGContextTranslateCTM(context, 0, m_window.contentView.bounds.size.height);
  // // 移动到顶部 CGContextScaleCTM(context, 1.0, -1.0); // Y 轴翻转

  CGDataProviderRef data_provider_ref = CGDataProviderCreateWithData(
      NULL, (char *)fb.data, fb.rowbytes * fb.height, NULL);

  CGImageRef image = CGImageCreate(
      fb.width, fb.height,
      // client_width, client_height,
      8, 32, fb.rowbytes,
      // colorspace
      CGColorSpaceCreateWithName(kCGColorSpaceSRGB),
      // CGBitmapInfo
      kCGBitmapByteOrder32Little | (CGBitmapInfo)kCGImageAlphaNoneSkipFirst,
      data_provider_ref,
      nullptr, // decode
      false,   // shouldInterpolate
      kCGRenderingIntentDefault);
  assert(image);

  for (int i = 0; i < dirty_region_px.Count(); i++) {
    const ui::Rect& rc_dirty = dirty_region_px.RectPtr2()[i];

    Rect rc_dirty_px = rc_dirty;

    ui::Rect rc_dirty_dip = rc_dirty;
    m_ui_window.m_dpi.RestoreRect(&rc_dirty_dip);

#if 1
    // 效率比使用clip低，废弃
    // 使用子图片实现脏区域提交
    NSRect nsrect = CGRectMake(rc_dirty_px.left, rc_dirty_px.top,
                               rc_dirty_px.width(), rc_dirty_px.height());
    CGImageRef part_image = CGImageCreateWithImageInRect(image, nsrect);

    NSRect dirty =
        CGRectMake(rc_dirty_dip.left, // * m_window.backingScaleFactor,
                                      // 转换成左下角原点
                   client_height - rc_dirty_dip.top -
                       rc_dirty_dip.height(), // * m_window.backingScaleFactor,
                   rc_dirty_dip.width(),      // * m_window.backingScaleFactor,
                   rc_dirty_dip.height());    // * m_window.backingScaleFactor);

    CGContextDrawImage(context, dirty, part_image);
    CGImageRelease(part_image);
#else
    // 使用clip实现脏区域提交
    // CGRect dirty = CGRectMake(rc.left,
    //                           rc.top,
    //                           rc.width(),
    //                           rc.height());
    int content_height = m_window.contentView.bounds.size.height;
    NSRect dirty =
        CGRectMake(rc.left / m_window.backingScaleFactor,
                   content_height - (rc.bottom / m_window.backingScaleFactor),
                   rc.width() / m_window.backingScaleFactor,
                   rc.height() / m_window.backingScaleFactor);
    CGContextClipToRect(context, dirty);

    printf("commit dirty region: origin: %f,%f,  size: %f,%f)\n",
           dirty.origin.x, dirty.origin.y, dirty.size.width, dirty.size.height);

    // 注：左下角为原点，图片也是从左下角开始绘制的。
    // 但缓存的尺寸是2指数倍，与窗口大小不一致。
    // 因此这里调整纵坐标，减去缓存底部的空闲区域
    // int image_bottom_offset = m_window.contentView.bounds.size.height -
    // pm.height()/2; NSRect target_rect = NSMakeRect(
    //   0,
    //   image_bottom_offset,
    //   pm.width()/2, pm.height()/2);
    NSRect target_rect =
        NSMakeRect(0, 0, m_window.contentView.bounds.size.width,
                   m_window.contentView.bounds.size.height);

    CGContextDrawImage(context, target_rect, image);
    CGContextResetClip(context);

    // CGRect redRect = CGRectMake(50, 50, 200, 200);
    // CGContextSetFillColorWithColor(context, [NSColor redColor].CGColor);
    // CGContextFillRect(context, redRect);
#endif
  }
  CGImageRelease(image);

  // CGContextRestoreGState(context);
}

void WindowPlatformMac::notifySize() {
  m_ui_window.onSize(m_window.frame.size.width * m_window.backingScaleFactor,
                     m_window.frame.size.height * m_window.backingScaleFactor);
}

void WindowPlatformMac::onPaint(const Rect &dirty) {
  ui::Rect rect_px = dirty;
  m_ui_window.m_dpi.ScaleRect(&rect_px);
  m_ui_window.onPaint(&rect_px);
}

// Vulkan调用vkCreateMacOSSurfaceMVK时使用。
// 目前测试vkCreateMacOSSurfaceMVK必须是在主线程调用才行。但之后的swap chain可以在
// 其它线程调用。
//
// VkMacOSSurfaceCreateInfoMVK::pView 字段说明：
// 1. if pView is a CAMetalLayer object，is must be a valid CAMetalLayer
// 2. if pView is an NSView object, it must be a valid NSView,
//    must be backed by a CALayer object of CAMetaLayer,
//    and vkCreateMacOSSurfaceMVK must be called on the main thread.
// 
void *WindowPlatformMac::GetNSWindowRootView() {
  NSView *view = m_window.contentView;
  [view setWantsLayer:YES];
  view.layer = [CAMetalLayer layer];

  // return (void *)(view);
  return (void*)view.layer;
};

} // namespace ui

@implementation WindowDelegate {
  ui::WindowPlatformMac *m_window;
}
- (WindowDelegate *)initWithWindow:(ui::WindowPlatformMac *)initWindow {
  m_window = initWindow;
  NSWindow *window = m_window->window();

  // 监听窗口DPI
  [[NSNotificationCenter defaultCenter]
      addObserver:self
         selector:@selector(windowDidChangeBackingProperties:)
             name:NSWindowDidChangeBackingPropertiesNotification
           object:window];
  m_window->m_ui_window.m_dpi.SetSystemDpi([window screen].backingScaleFactor);

  return self;
}

- (void)windowDidChangeBackingProperties:(NSNotification *)notification {
  NSWindow *window = notification.object;
  CGFloat newScaleFactor = window.backingScaleFactor;
  m_window->m_ui_window.m_dpi.SetSystemDpi(newScaleFactor);
}

- (void)windowDidResize:(NSNotification *)notification {
  NSView *view = m_window->window().contentView;
  // CGFloat scale = skwindow::GetBackingScaleFactor(view);
  // m_window->m_ui_window.onSize(view.bounds.size.width /* * scale*/,
  //                              view.bounds.size.height /* * scale*/);

  m_window->notifySize();
}

- (void)windowWillStartLiveResize:(NSNotification *)notification {
  m_window->m_ui_window.enterResize(true);
}
- (void)windowDidEndLiveResize:(NSNotification *)notification {
  m_window->m_ui_window.enterResize(false);
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
  m_window->m_ui_window.onDestroy();
  return FALSE;
}
@end

@implementation MainView {
  ui::WindowPlatformMac *m_window;
}

// isFlipped 只影响 NSView 的坐标系，不影响Core Graphics的坐标系。
// - (BOOL)isFlipped {
//     return YES; // 坐标系原点改为左上角
// }

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
  
  // 转换成左上角坐标
  ui::Rect r = {
      (int)rect.origin.x,
      (int)(self.bounds.size.height - rect.origin.y - rect.size.height),
      (int)rect.size.width, (int)rect.size.height};
  m_window->onPaint(r);
}

- (void)keyDown:(NSEvent *)event {
  printf("keyDown\n");
}
- (void)keyUp:(NSEvent *)event {
  printf("keyUp\n");
}
- (void)mouseDown:(NSEvent *)event {
  const NSPoint pos = [event locationInWindow];
  const NSRect rect = [self frame];

  int x = pos.x;
  int y = (rect.size.height - pos.y);
  m_window->m_ui_window.m_mouse_key.OnLButtonDown(x, y);
}

- (void)mouseUp:(NSEvent *)event {
  const NSPoint pos = [event locationInWindow];
  const NSRect rect = [self frame];

  int x = pos.x;
  int y = (rect.size.height - pos.y);
  m_window->m_ui_window.m_mouse_key.OnLButtonUp(x, y);
}

- (void)mouseMoved:(NSEvent *)event {
  const NSPoint pos = [event locationInWindow];
  const NSRect rect = [self frame];

  int x = pos.x;
  int y = (rect.size.height - pos.y);
  m_window->m_ui_window.m_mouse_key.OnMouseMove(x, y);
}

- (void)scrollWheel:(NSEvent *)event {
  printf("scrollWheel\n");
}
@end