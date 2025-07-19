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
}

}