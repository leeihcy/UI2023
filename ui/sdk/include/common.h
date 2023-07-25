#ifndef _UI_SDK_COMMON_H_
#define _UI_SDK_COMMON_H_
#include <string>

namespace ui
{

using byte = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;
using wchar = wchar_t;


struct Point {
    int x;
    int y;
};
struct Size
{
    int width;
    int height;
};
struct Rect
{
    int x;
    int y;
    int width;
    int height;
};
struct RectLTRB
{
    int left;
    int top;
    int right;
    int bottom;
};

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
#define HTCLIENT  1
#define HTCAPTION  2
#define HTSYSMENU  3
#define HTGROWBOX  4
#define HTSIZE  HTGROWBOX
#define HTMENU  5
#define HTHSCROLL  6
#define HTVSCROLL  7
#define HTMINBUTTON  8
#define HTMAXBUTTON  9
#define HTLEFT 10
#define HTRIGHT  11
#define HTTOP 12
#define HTTOPLEFT 13
#define HTTOPRIGHT 14
#define HTBOTTOM  15
#define HTBOTTOMLEFT  16
#define HTBOTTOMRIGHT  17
#define HTBORDER  18
#define HTREDUCE  HTMINBUTTON
#define HTZOOM  HTMAXBUTTON
#define HTSIZEFIRST HTLEFT
#define HTSIZELAST  HTBOTTOMRIGHT
#define HTOBJECT  19
#define HTCLOSE  20

struct SIZE {
    long cx;
    long cy;
};
struct POINT {
    long x;
    long y;
};
struct RECT
{
    //Top-left point coordinate pair
    long left;
    long top;
    //Bottom-right point coordinate pair
    long right;
    long bottom;

    void SetEmpty() {
        this->left = 0;
        this->top = 0;
        this->right = 0;
        this->bottom = 0;
    }
    bool IsEmpty() {
        return left==right && top==bottom;
    }
    void Set(int l, int t, int r, int b) {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }
    void CopyFrom(const RECT& p) {
        this->left = p.left;
        this->top = p.top;
        this->right = p.right;
        this->bottom = p.bottom;
    }
    bool PtIn(const POINT& pt) {
        return pt.x >= left && pt.x <= right &&
            pt.y >= top && pt.y <= bottom;
    }
    bool Intersect(const RECT& rc, RECT* out) {
        if (rc.right < left || rc.left > right || 
            rc.bottom < top || rc.top > bottom) {
            return false;
        }

        out->Set(
            std::max(rc.left, left),
            std::min(rc.right, right),
            std::max(rc.top, top),
            std::min(rc.bottom, bottom)
        );
        return true;
    }

    void Offset(int x, int y) {
        left += x;
        right += x;
        top += y;
        bottom += y;
    }

    int width() const {
        return right-left;
    }
    int Width() const {
        return width();
    }
    int height() const {
        return bottom-top;
    }
    int Height() const {
        return height();
    }
    bool operator==(const RECT& rc) {
        return memcmp(this, &rc, sizeof(RECT)) == 0;
    }
};

#define LF_FACESIZE 32
typedef struct tagLOGFONT { 
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
  wchar_t lfFaceName[LF_FACESIZE]; 
} LOGFONT; 
#define FW_NORMAL 400
#define FW_BOLD 700

#define WM_MOVE 0x0003
#define WM_SIZE 0x0005
#define WM_PAINT 0x000F 
#define WM_QUIT 0x0012 
#define WM_ERASEBKGND 0x0014 
#define WM_THEMECHANGED 0x031A

#define SWP_NOMOVE 1
#define SWP_NOSIZE 2
#define SWP_NOZORDER 4
#define SWP_NOREDRAW 8

#define DT_SINGLELINE 1
#define DT_END_ELLIPSIS 2
#define DT_VCENTER 4
#define DT_NOPREFIX 8
#define DT_BOTTOM  0x10
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

#define GetRValue(rgb)   ((unsigned char) (rgb)) 
#define GetGValue(rgb)   ((unsigned char) (((unsigned short) (rgb)) >> 8)) 
#define GetBValue(rgb)   ((unsigned short) ((rgb) >> 16)) 
#define RGB(r,g,b)  ((unsigned char)r | ((unsigned char)g << 8) | ((unsigned char)b << 16))

#define MAKELONG(a, b) ((long) (((short) (a)) | ((unsigned int) ((short) (b))) << 16))
#define MAKELPARAM(l, h) ((long) MAKELONG(l, h))

#define wcsicmp wcscasecmp

#endif

//
//	release版本下断言输出，可以利用debugview.exe看到断言错误
//  remark: __FILE__ ...这些都是char*类型的
//
#ifdef ASSERT
#undef ASSERT  // 避免使用ASSERT，请使用UIASSERT
#endif

#ifdef _DEBUG
#define UIASSERT(x)   \
	if(!(x))                          \
	{                                 \
		char szInfo[2048] = "";       \
		sprintf_s(szInfo, 2047, "UIASSERT expr(%s) %s(L%d) ## %s\r\n", #x, __FILE__, __LINE__, __FUNCTION__ );  \
		::OutputDebugStringA(szInfo); \
		DebugBreak(); \
	} 
#else
#define UIASSERT(x)    
#endif


	
// 常用缩写定义
#define SAFE_DELETE(p) \
	if (p) \
{ \
	delete (p); \
	(p) = NULL; \
}

#define SAFE_ARRAY_DELETE(p) \
	if (p) \
{ \
	delete[] (p); \
	(p) = NULL; \
}

#define SAFE_RELEASE(p) \
	if (p) \
{ \
	(p)->Release(); \
	(p) = NULL; \
}

#define SAFE_RELEASE2(p) \
	if (p) \
{ \
	(p).Release(); \
	(p) = NULL; \
}

#define SAFE_DELETE_GDIOBJECT(h) \
{\
	if (h)\
	{\
		/*bool b = */::DeleteObject(h);\
		/*if (0 == b) UIASSERT(b);*/ \
		(h) = NULL;\
	}\
}

// #ifdef _UNICODE
	typedef std::wstring String;
// #else
//     typedef std::string  String;
// #endif


inline int _round(const float& f)
{
	if (f < 0.0f)
		return int(f - 0.5f);
	else
		return int(f + 0.5f);
}
inline int _round(const double& f)
{
	if (f < 0.0f)
		return int(f - 0.5);
	else
		return int(f + 0.5);
}

}
#endif