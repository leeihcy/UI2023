
#import "Cocoa/Cocoa.h"

int mac_main()
{
#if 0
    // Autorelease Pool:
    // Objects declared in this scope will be automatically
    // released at the end of it, when the pool is "drained".
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    // Create a shared app instance.
    // This will initialize the global variable
    // 'NSApp' with the application instance.
    [NSApplication sharedApplication];

    //
    // Create a window:
    //

    // Style flags:
    NSUInteger windowStyle = NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask;

    // Window bounds (x, y, width, height).
    NSRect windowRect = NSMakeRect(100, 100, 400, 400);
    NSWindow * window = [[NSWindow alloc] initWithContentRect:windowRect
                                          styleMask:windowStyle
                                          backing:NSBackingStoreBuffered
                                          defer:NO];
    [window autorelease];

    // Window controller:
    NSWindowController * windowController = [[NSWindowController alloc] initWithWindow:window];
    [windowController autorelease];

    // This will add a simple text view to the window,
    // so we can write a test string on it.
    NSTextView * textView = [[NSTextView alloc] initWithFrame:windowRect];
    [textView autorelease];

    [window setContentView:textView];
    [textView insertText:@"Hello OSX/Cocoa world!"];
  
    // TODO: Create app delegate to handle system events.
    // TODO: Create menus (especially Quit!)

    // Show window and run event loop.
    [window orderFrontRegardless];
    [NSApp run];

    [pool drain];
#endif


//   [NSAutoreleasePool new];
  id app = [NSApplication sharedApplication];
//   [app setActivationPolicy:NSApplicationActivationPolicyRegular];
//   [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

  NSRect frame = NSMakeRect(0, 0, 300, 300);
//   id window = 
//     [[[NSWindow alloc] initWithContentRect:frame
//                                  styleMask:NSTitledWindowMask
//                                    backing:NSBackingStoreBuffered
//                                      defer:NO] autorelease];

NSUInteger windowStyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                           NSWindowStyleMaskResizable;

  NSRect windowRect = NSMakeRect(100, 100, 400, 400);
NSWindow* m_window = [[NSWindow alloc] initWithContentRect:windowRect
                                         styleMask:windowStyle
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
                                             
//   [window cascadeTopLeftFromPoint:NSMakePoint(10, 10)];
//   [window setTitle:@"Hello"];
//   [window makeKeyAndOrderFront:nil];
   [m_window makeKeyAndOrderFront:nil];
  
//   id button = [[[NSButton alloc] initWithFrame:frame] autorelease];
//   [button setTarget:app];
//   [button setAction:@selector(terminate:)];
//   [button setTitle:@"Quit"];
//   [[window contentView] addSubview:button];
  
//   [app activateIgnoringOtherApps:YES];
  [app run];
  return 0;
 
    return 0;
}