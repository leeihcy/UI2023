#include "application_mac.h"
#import <Cocoa/Cocoa.h>

namespace ui {

static bool init = false;

// static
void ApplicationMac::Init() {
  if (init) {
    return;
  }
  init = true;

  [NSApplication sharedApplication];

  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
//   [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

//   id menubar = [[NSMenu new] autorelease];
//   id appMenuItem = [[NSMenuItem new] autorelease];
//   [menubar addItem:appMenuItem];
//   [NSApp setMainMenu:menubar];
//   id appMenu = [[NSMenu new] autorelease];
//   id appName = [[NSProcessInfo processInfo] processName];
//   id quitTitle = [@"Quit " stringByAppendingString:appName];
//   id quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle
//                                                 action:@selector(terminate:)
//                                          keyEquivalent:@"q"] autorelease];
//   [appMenu addItem:quitMenuItem];
//   [appMenuItem setSubmenu:appMenu];

  printf("ApplicationMac::Init\n");
}

}