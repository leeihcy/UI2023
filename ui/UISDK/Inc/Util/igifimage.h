#ifndef _UI_IGIFIMAGE_H_
#define _UI_IGIFIMAGE_H_

#include "iimage.h"
#include <functional>

namespace ui
{
interface  IGifImageRender;

// Gif动画绘制的方式
enum Gif_Timer_Notify_Type
{
    Gif_Timer_Notify_Direct_Hwnd,
    Gif_Timer_Notify_Send_Msg,
	Gif_Timer_Notify_Function,
};
struct Gif_Timer_Notify
{
    Gif_Timer_Notify_Type  eType;
    union
    {
        struct  // 直接显示在窗口的某个位置(未考虑裁剪)
        {
            HWND   hWnd;
            HDC    hDC;
            int    x;
            int    y;
        }notify_hwnd;

        struct  // 发送消息给界面线程，通知对象自己绘制
        {
            IMessage*  pNotifyMsgObj;
            LPARAM     lParam;

        }notify_ui_msg;

		struct 
		{
			void (*pfnCallback)(LPARAM lParam);
			LPARAM     lParam;
		}notify_function;
    };
};

class GifImageBase;
interface UIAPI IGifImage
{
public:
    IGifImage(GifImageBase* p);
    IGifImage();
    ~IGifImage();

    void  CreateGifImpl(IApplication* pUIApp);
    void  CreatePnglistGifImpl();
    GifImageBase*  GetImpl();
    void  SetImpl(GifImageBase* p);

public:
    bool  Load(const TCHAR* szPath, IMapAttribute* pMapAttrib=nullptr);
    bool  Destroy();

    IGifImageRender*  AddRender(Gif_Timer_Notify* pNotify, IApplication*  pUIApp, int* pIndex = nullptr);
    bool  ModifyRender(Gif_Timer_Notify* pNotify, int nIndex=-1);
    bool  DeleteRender(int nIndex=-1);

    bool  SetTransparentColor(COLORREF colTransparent = GetSysColor(COLOR_BTNFACE));
	ImageWrap*  GetFrameIImage( int nIndex );

private:
    GifImageBase*  m_pImpl;
    bool  m_bCreateGifImageImpl;
};

//
//	GIF动画状态
//	
enum GIF_DRAW_STATUS
{
    GIF_DRAW_STATUS_START,
    GIF_DRAW_STATUS_STOP,
    GIF_DRAW_STATUS_PAUSE
};

interface  IGifImageRender
{
    virtual void  Release() = 0;

    virtual void  Start() = 0;
    virtual void  Pause() = 0;
    virtual void  Stop() = 0;

    virtual int  GetWidth() = 0;
    virtual int  GetHeight() = 0;
    virtual void  OnPaint(HDC hDC, int x, int y) = 0;
    virtual void  OnAlphaPaint(HDC hDC, int x, int y) = 0;
	virtual void  OnAlphaPaint(HDC hDC, int x, int y, int cx, int cy) = 0;

    virtual GIF_DRAW_STATUS GetStatus() = 0;
	virtual IGifImage*  GetIGifImage() = 0;
};

}

#endif 