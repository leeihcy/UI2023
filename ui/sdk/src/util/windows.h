#ifndef _UI_SDK_INCLUDE_UTIL_WINDOWS_H_
#define _UI_SDK_INCLUDE_UTIL_WINDOWS_H_
#include <string>
#if defined(OS_WIN)

// #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#pragma warning(disable:4996)
#pragma warning(disable:4302)
#pragma warning(disable:4838)
#define _CSTDDEF_

#include <windows.h>
#include <windowsx.h>
#include <wingdi.h>


#include <atlbase.h>
#include <atlcore.h>
#include "ui/3rd/wtl90/Include/atlapp.h"


// #define _WTL_NO_CSTRING
// #include <../3rd/wtl90/Include/atlmisc.h>
// using namespace WTL;

// #include "Inc/inc.h"
// #include "Inc/private_inc.h"
// using namespace ui;

// extern HINSTANCE g_hInstance;


///////////////////////////////////////////////////////////////////////////////
// Message map macro for cracked handlers

// Note about message maps with cracked handlers:
// For ATL 3.0, a message map using cracked handlers MUST use BEGIN_MSG_MAP_EX.
// For ATL 7.0 or higher, you can use BEGIN_MSG_MAP for CWindowImpl/CDialogImpl derived classes,
// but must use BEGIN_MSG_MAP_EX for classes that don't derive from CWindowImpl/CDialogImpl.

#define BEGIN_MSG_MAP_EX(theClass) \
public: \
	BOOL m_bMsgHandled; \
	/* "handled" management for cracked handlers */ \
	BOOL IsMsgHandled() const \
	{ \
		return m_bMsgHandled; \
	} \
	void SetMsgHandled(BOOL bHandled) \
	{ \
		m_bMsgHandled = bHandled; \
	} \
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) \
	{ \
		BOOL bOldMsgHandled = m_bMsgHandled; \
		BOOL bRet = _ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID); \
		m_bMsgHandled = bOldMsgHandled; \
		return bRet; \
	} \
	BOOL _ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID) \
	{ \
		BOOL bHandled = TRUE; \
		hWnd; \
		uMsg; \
		wParam; \
		lParam; \
		lResult; \
		bHandled; \
		switch(dwMsgMapID) \
		{ \
		case 0:



// LRESULT OnMessageHandlerEX(UINT uMsg, WPARAM wParam, LPARAM lParam)
#define MESSAGE_HANDLER_EX(msg, func) \
	if(uMsg == msg) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = func(uMsg, wParam, lParam); \
		if(IsMsgHandled()) \
			return TRUE; \
	}


#define MESSAGE_HANDLER(msg, func) \
	if(uMsg == msg) \
	{ \
		bHandled = TRUE; \
		lResult = func(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}


#define END_MSG_MAP() \
			break; \
		default: \
			ATLTRACE(static_cast<int>(ATL::atlTraceWindowing), 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID); \
			ATLASSERT(FALSE); \
			break; \
		} \
		return FALSE; \
	}


#undef SendMessage
#undef LoadBitmap
#undef DrawState
#undef GetObject
#undef FindWindow
#undef GetWindowStyle
#undef GetFirstChild
#undef GetTopWindow
#undef max
#undef min
#endif

namespace ui {

#if defined(OS_WIN)
#else
#define HWND long
#define HDC long
#define HBITMAP long
#define HFONT long
#define HRGN long
#define HCURSOR long
#define HMODULE long
#define MAX_PATH 256

#define HTERROR -2
#define HTTRANSPARENT -1
#define HTNOWHERE 0
#define HTCLIENT 1
#define HTCAPTION 2
#define HTSYSMENU 3
#define HTGROWBOX 4
#define HTSIZE HTGROWBOX
#define HTMENU 5
#define HTHSCROLL 6
#define HTVSCROLL 7
#define HTMINBUTTON 8
#define HTMAXBUTTON 9
#define HTLEFT 10
#define HTRIGHT 11
#define HTTOP 12
#define HTTOPLEFT 13
#define HTTOPRIGHT 14
#define HTBOTTOM 15
#define HTBOTTOMLEFT 16
#define HTBOTTOMRIGHT 17
#define HTBORDER 18
#define HTREDUCE HTMINBUTTON
#define HTZOOM HTMAXBUTTON
#define HTSIZEFIRST HTLEFT
#define HTSIZELAST HTBOTTOMRIGHT
#define HTOBJECT 19
#define HTCLOSE 20

#define LF_FACESIZE 32
#define FW_NORMAL 400
#define FW_BOLD 700

#define WM_MOVE 0x0003
#define WM_SIZE 0x0005
#define WM_PAINT 0x000F
#define WM_QUIT 0x0012
#define WM_ERASEBKGND 0x0014
#define WM_THEMECHANGED 0x031A
#define WM_WINDOWPOSCHANGING 0x0046
#define WM_WINDOWPOSCHANGED 0x0047

typedef struct tagWINDOWPOS {
  HWND hwnd;
  HWND hwndInsertAfter;
  int  x;
  int  y;
  int  cx;
  int  cy;
  unsigned int flags;
} WINDOWPOS, *LPWINDOWPOS, *PWINDOWPOS;

#define DT_SINGLELINE 1
#define DT_END_ELLIPSIS 2
#define DT_VCENTER 4
#define DT_NOPREFIX 8
#define DT_BOTTOM 0x10
#define DT_WORDBREAK 0x20
#define DT_CENTER 0x40
#define DT_EDITCONTROL 0x80
#define DT_RIGHT 0x100

#define TEXT(xxx) L##xxx
#define _T(xxx) L##xxx

#if defined(CPU_X64)
#define __thiscall
#define __stdcall
#endif

#define GetRValue(rgb) ((unsigned char)(rgb))
#define GetGValue(rgb) ((unsigned char)(((unsigned short)(rgb)) >> 8))
#define GetBValue(rgb) ((unsigned short)((rgb) >> 16))
#define RGB(r, g, b)                                                           \
  ((unsigned char)r | ((unsigned char)g << 8) | ((unsigned char)b << 16))

#define MAKELONG(a, b)                                                         \
  ((long)(((short)(a)) | ((unsigned int)((short)(b))) << 16))
#define MAKELPARAM(l, h) ((long)MAKELONG(l, h))

#define wcsicmp wcscasecmp

#endif



struct FontDesc {
  long lfHeight;
  long lfWidth;
  long lfEscapement;
  long lfOrientation;
  long lfWeight;
  unsigned char lfItalic;
  unsigned char lfUnderline;
  unsigned char lfStrikeOut;
  unsigned char lfCharSet;
  unsigned char lfOutPrecision;
  unsigned char lfClipPrecision;
  unsigned char lfQuality;
  unsigned char lfPitchAndFamily;
  char lfFaceName[LF_FACESIZE];
};
}

#endif