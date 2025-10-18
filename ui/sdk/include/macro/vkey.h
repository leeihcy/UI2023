#ifndef _UI_SDK_INCLUDE_MACRO_VIRTUAL_KEY_CODES_H_
#define _UI_SDK_INCLUDE_MACRO_VIRTUAL_KEY_CODES_H_

typedef int KeyboardCode;
#define VKEY_UNKNOWN 0

#define VKEY_FLAG_CTRL 0x01
#define VKEY_FLAG_SHIFT 0x02
#define VKEY_FLAG_ALT 0x04
#define VKEY_FLAG_META 0x08  // Windows键或Command键
#define VKEY_FLAG_CAPSLOCK 0x10

#define VKEY_FLAG_LBUTTON 0x20
#define VKEY_FLAG_RBUTTON 0x40
#define VKEY_FLAG_MBUTTON 0x80

// https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

// Constant Value Description
#define VKEY_LBUTTON 0x01  // Left mouse button
#define VKEY_RBUTTON 0x02  // Right mouse button
#define VKEY_CANCEL 0x03   // Control-break processing
#define VKEY_MBUTTON 0x04  // Middle mouse button
#define VKEY_XBUTTON1 0x05 // X1 mouse button
#define VKEY_XBUTTON2 0x06 // X2 mouse button
// 0x07	Reserved
#define VKEY_BACK 0x08 // Backspace key
#define VKEY_TAB 0x09  // Tab key
// 0x0A-0B	Reserved
#define VKEY_CLEAR 0x0C  // Clear key
#define VKEY_RETURN 0x0D // Enter key
// 0x0E-0F	Unassigned
#define VKEY_SHIFT 0x10      // Shift key
#define VKEY_CONTROL 0x11    // Ctrl key
#define VKEY_MENU 0x12       // Alt key
#define VKEY_PAUSE 0x13      // Pause key
#define VKEY_CAPITAL 0x14    // Caps lock key
#define VKEY_KANA 0x15       // IME Kana mode
#define VKEY_HANGUL 0x15     // IME Hangul mode
#define VKEY_IME_ON 0x16     // IME On
#define VKEY_JUNJA 0x17      // IME Junja mode
#define VKEY_FINAL 0x18      // IME final mode
#define VKEY_HANJA 0x19      // IME Hanja mode
#define VKEY_KANJI 0x19      // IME Kanji mode
#define VKEY_IME_OFF 0x1A    // IME Off
#define VKEY_ESCAPE 0x1B     // Esc key
#define VKEY_CONVERT 0x1C    // IME convert
#define VKEY_NONCONVERT 0x1D // IME nonconvert
#define VKEY_ACCEPT 0x1E     // IME accept
#define VKEY_MODECHANGE 0x1F // IME mode change request
#define VKEY_SPACE 0x20      // Spacebar key
#define VKEY_PRIOR 0x21      // Page up key
#define VKEY_NEXT 0x22       // Page down key
#define VKEY_END 0x23        // End key
#define VKEY_HOME 0x24       // Home key
#define VKEY_LEFT 0x25       // Left arrow key
#define VKEY_UP 0x26         // Up arrow key
#define VKEY_RIGHT 0x27      // Right arrow key
#define VKEY_DOWN 0x28       // Down arrow key
#define VKEY_SELECT 0x29     // Select key
#define VKEY_PRINT 0x2A      // Print key
#define VKEY_EXECUTE 0x2B    // Execute key
#define VKEY_SNAPSHOT 0x2C   // Print screen key
#define VKEY_INSERT 0x2D     // Insert key
#define VKEY_DELETE 0x2E     // Delete key
#define VKEY_HELP 0x2F       // Help key
#define VKEY_0 0x30          // 0 key
#define VKEY_1 0x31          // 1 key
#define VKEY_2 0x32          // 2 key
#define VKEY_3 0x33          // 3 key
#define VKEY_4 0x34          // 4 key
#define VKEY_5 0x35          // 5 key
#define VKEY_6 0x36          // 6 key
#define VKEY_7 0x37          // 7 key
#define VKEY_8 0x38          // 8 key
#define VKEY_9 0x39          // 9 key
// 0x3A-40	Undefined
#define VKEY_A 0x41    // A key
#define VKEY_B 0x42    // B key
#define VKEY_C 0x43    // C key
#define VKEY_D 0x44    // D key
#define VKEY_E 0x45    // E key
#define VKEY_F 0x46    // F key
#define VKEY_G 0x47    // G key
#define VKEY_H 0x48    // H key
#define VKEY_I 0x49    // I key
#define VKEY_J 0x4A    // J key
#define VKEY_K 0x4B    // K key
#define VKEY_L 0x4C    // L key
#define VKEY_M 0x4D    // M key
#define VKEY_N 0x4E    // N key
#define VKEY_O 0x4F    // O key
#define VKEY_P 0x50    // P key
#define VKEY_Q 0x51    // Q key
#define VKEY_R 0x52    // R key
#define VKEY_S 0x53    // S key
#define VKEY_T 0x54    // T key
#define VKEY_U 0x55    // U key
#define VKEY_V 0x56    // V key
#define VKEY_W 0x57    // W key
#define VKEY_X 0x58    // X key
#define VKEY_Y 0x59    // Y key
#define VKEY_Z 0x5A    // Z key
#define VKEY_LWIN 0x5B // Left Windows logo key
#define VKEY_RWIN 0x5C // Right Windows logo key
#define VKEY_APPS 0x5D // Application key
// 0x5E	Reserved
#define VKEY_SLEEP 0x5F     // Computer Sleep key
#define VKEY_NUMPAD0 0x60   // Numeric keypad 0 key
#define VKEY_NUMPAD1 0x61   // Numeric keypad 1 key
#define VKEY_NUMPAD2 0x62   // Numeric keypad 2 key
#define VKEY_NUMPAD3 0x63   // Numeric keypad 3 key
#define VKEY_NUMPAD4 0x64   // Numeric keypad 4 key
#define VKEY_NUMPAD5 0x65   // Numeric keypad 5 key
#define VKEY_NUMPAD6 0x66   // Numeric keypad 6 key
#define VKEY_NUMPAD7 0x67   // Numeric keypad 7 key
#define VKEY_NUMPAD8 0x68   // Numeric keypad 8 key
#define VKEY_NUMPAD9 0x69   // Numeric keypad 9 key
#define VKEY_MULTIPLY 0x6A  // Multiply key
#define VKEY_ADD 0x6B       // Add key
#define VKEY_SEPARATOR 0x6C // Separator key
#define VKEY_SUBTRACT 0x6D  // Subtract key
#define VKEY_DECIMAL 0x6E   // Decimal key
#define VKEY_DIVIDE 0x6F    // Divide key
#define VKEY_F1 0x70        // F1 key
#define VKEY_F2 0x71        // F2 key
#define VKEY_F3 0x72        // F3 key
#define VKEY_F4 0x73        // F4 key
#define VKEY_F5 0x74        // F5 key
#define VKEY_F6 0x75        // F6 key
#define VKEY_F7 0x76        // F7 key
#define VKEY_F8 0x77        // F8 key
#define VKEY_F9 0x78        // F9 key
#define VKEY_F10 0x79       // F10 key
#define VKEY_F11 0x7A       // F11 key
#define VKEY_F12 0x7B       // F12 key
#define VKEY_F13 0x7C       // F13 key
#define VKEY_F14 0x7D       // F14 key
#define VKEY_F15 0x7E       // F15 key
#define VKEY_F16 0x7F       // F16 key
#define VKEY_F17 0x80       // F17 key
#define VKEY_F18 0x81       // F18 key
#define VKEY_F19 0x82       // F19 key
#define VKEY_F20 0x83       // F20 key
#define VKEY_F21 0x84       // F21 key
#define VKEY_F22 0x85       // F22 key
#define VKEY_F23 0x86       // F23 key
#define VKEY_F24 0x87       // F24 key
// 0x88-8F	Reserved
#define VKEY_NUMLOCK 0x90 // Num lock key
#define VKEY_SCROLL 0x91  // Scroll lock key
// 0x92-96	OEM specific
// 0x97-9F	Unassigned
#define VKEY_LSHIFT 0xA0              // Left Shift key
#define VKEY_RSHIFT 0xA1              // Right Shift key
#define VKEY_LCONTROL 0xA2            // Left Ctrl key
#define VKEY_RCONTROL 0xA3            // Right Ctrl key
#define VKEY_LMENU 0xA4               // Left Alt key
#define VKEY_RMENU 0xA5               // Right Alt key
#define VKEY_BROWSER_BACK 0xA6        // Browser Back key
#define VKEY_BROWSER_FORWARD 0xA7     // Browser Forward key
#define VKEY_BROWSER_REFRESH 0xA8     // Browser Refresh key
#define VKEY_BROWSER_STOP 0xA9        // Browser Stop key
#define VKEY_BROWSER_SEARCH 0xAA      // Browser Search key
#define VKEY_BROWSER_FAVORITES 0xAB   // Browser Favorites key
#define VKEY_BROWSER_HOME 0xAC        // Browser Start and Home key
#define VKEY_VOLUME_MUTE 0xAD         // Volume Mute key
#define VKEY_VOLUME_DOWN 0xAE         // Volume Down key
#define VKEY_VOLUME_UP 0xAF           // Volume Up key
#define VKEY_MEDIA_NEXT_TRACK 0xB0    // Next Track key
#define VKEY_MEDIA_PREV_TRACK 0xB1    // Previous Track key
#define VKEY_MEDIA_STOP 0xB2          // Stop Media key
#define VKEY_MEDIA_PLAY_PAUSE 0xB3    // Play/Pause Media key
#define VKEY_LAUNCH_MAIL 0xB4         // Start Mail key
#define VKEY_LAUNCH_MEDIA_SELECT 0xB5 // Select Media key
#define VKEY_LAUNCH_APP1 0xB6         // Start Application 1 key
#define VKEY_LAUNCH_APP2 0xB7         // Start Application 2 key
// 0xB8-B9	Reserved
#define VKEY_OEM_1                                                            \
  0xBA // It can vary by keyboard. For the US ANSI keyboard , the Semiсolon and
       // Colon key
#define VKEY_OEM_PLUS 0xBB // For any country/region, the Equals and Plus key
#define VKEY_OEM_COMMA                                                        \
  0xBC // For any country/region, the Comma and Less Than key
#define VKEY_OEM_MINUS                                                        \
  0xBD // For any country/region, the Dash and Underscore key
#define VKEY_OEM_PERIOD                                                       \
  0xBE // For any country/region, the Period and Greater Than key
#define VKEY_OEM_2                                                            \
  0xBF // It can vary by keyboard. For the US ANSI keyboard, the Forward Slash
       // and Question Mark key
#define VKEY_OEM_3                                                            \
  0xC0 // It can vary by keyboard. For the US ANSI keyboard, the Grave Accent
       // and Tilde key
// 0xC1-C2	Reserved
#define VKEY_GAMEPAD_A 0xC3              // Gamepad A button
#define VKEY_GAMEPAD_B 0xC4              // Gamepad B button
#define VKEY_GAMEPAD_X 0xC5              // Gamepad X button
#define VKEY_GAMEPAD_Y 0xC6              // Gamepad Y button
#define VKEY_GAMEPAD_RIGHT_SHOULDER 0xC7 // Gamepad Right Shoulder button
#define VKEY_GAMEPAD_LEFT_SHOULDER 0xC8  // Gamepad Left Shoulder button
#define VKEY_GAMEPAD_LEFT_TRIGGER 0xC9   // Gamepad Left Trigger button
#define VKEY_GAMEPAD_RIGHT_TRIGGER 0xCA  // Gamepad Right Trigger button
#define VKEY_GAMEPAD_DPAD_UP 0xCB        // Gamepad D-pad Up button
#define VKEY_GAMEPAD_DPAD_DOWN 0xCC      // Gamepad D-pad Down button
#define VKEY_GAMEPAD_DPAD_LEFT 0xCD      // Gamepad D-pad Left button
#define VKEY_GAMEPAD_DPAD_RIGHT 0xCE     // Gamepad D-pad Right button
#define VKEY_GAMEPAD_MENU 0xCF           // Gamepad Menu/Start button
#define VKEY_GAMEPAD_VIEW 0xD0           // Gamepad View/Back button
#define VKEY_GAMEPAD_LEFT_THUMBSTICK_BUTTON                                   \
  0xD1 // Gamepad Left Thumbstick button
#define VKEY_GAMEPAD_RIGHT_THUMBSTICK_BUTTON                                  \
  0xD2                                        // Gamepad Right Thumbstick button
#define VKEY_GAMEPAD_LEFT_THUMBSTICK_UP 0xD3 // Gamepad Left Thumbstick up
#define VKEY_GAMEPAD_LEFT_THUMBSTICK_DOWN 0xD4 // Gamepad Left Thumbstick down
#define VKEY_GAMEPAD_LEFT_THUMBSTICK_RIGHT                                    \
  0xD5                                          // Gamepad Left Thumbstick right
#define VKEY_GAMEPAD_LEFT_THUMBSTICK_LEFT 0xD6 // Gamepad Left Thumbstick left
#define VKEY_GAMEPAD_RIGHT_THUMBSTICK_UP 0xD7  // Gamepad Right Thumbstick up
#define VKEY_GAMEPAD_RIGHT_THUMBSTICK_DOWN                                    \
  0xD8 //	Gamepad Right Thumbstick down
#define VKEY_GAMEPAD_RIGHT_THUMBSTICK_RIGHT                                   \
  0xD9 // Gamepad Right Thumbstick right
#define VKEY_GAMEPAD_RIGHT_THUMBSTICK_LEFT                                    \
  0xDA // Gamepad Right Thumbstick left
#define VKEY_OEM_4                                                            \
  0xDB // It can vary by keyboard. For the US ANSI keyboard, the Left Brace key
#define VKEY_OEM_5                                                            \
  0xDC // It can vary by keyboard. For the US ANSI keyboard, the Backslash and
       // Pipe key
#define VKEY_OEM_6                                                            \
  0xDD // It can vary by keyboard. For the US ANSI keyboard, the Right Brace key
#define VKEY_OEM_7                                                            \
  0xDE // It can vary by keyboard. For the US ANSI keyboard, the Apostrophe and
       // Double Quotation Mark key
#define VKEY_OEM_8                                                            \
  0xDF // It can vary by keyboard. For the Canadian CSA keyboard, the Right Ctrl
       // key
// 0xE0	Reserved
// 0xE1	OEM specific
#define VKEY_OEM_102                                                          \
  0xE2 // It can vary by keyboard. For the European ISO keyboard, the Backslash
       // and Pipe key
// 0xE3-E4	OEM specific
#define VKEY_PROCESSKEY 0xE5 // IME PROCESS key
// 0xE6	OEM specific
#define VKEY_PACKET                                                           \
  0xE7 //	Used to pass Unicode characters as if they were keystrokes. The
       //VK_PACKET key is the low word of a 32-bit Virtual Key value used for
       //non-keyboard input methods. For more information, see Remark in
       //KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP
// 0xE8	Unassigned
// 0xE9-F5	OEM specific
#define VKEY_ATTN 0xF6      // Attn key
#define VKEY_CRSEL 0xF7     // CrSel key
#define VKEY_EXSEL 0xF8     // ExSel key
#define VKEY_EREOF 0xF9     // Erase EOF key
#define VKEY_PLAY 0xFA      // Play key
#define VKEY_ZOOM 0xFB      // Zoom key
#define VKEY_NONAME 0xFC    // Reserved
#define VKEY_PA1 0xFD       // PA1 key
#define VKEY_OEM_CLEAR 0xFE // Clear key

#endif