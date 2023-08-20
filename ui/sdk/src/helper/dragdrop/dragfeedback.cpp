#include "stdafx.h"
#include "dragfeedback.h"
#include <shlobj.h>

//
// 系统自带的拖拽图像缺陷较多，自己实现一个
// 1. 移出窗口是隐藏，进入窗口时显示，造成闪烁
// 2. 透明色限制，会生成一些莫名其妙的问题
// 3. 大小限制，小于某个值时不会显示
// 4. 仅win7上有效
//

namespace ui
{
CDragFeedback*  g_pCurDragFeedback = nullptr;

CDragFeedback::CDragFeedback()
{
    g_pCurDragFeedback = this;
    m_hHook = nullptr;
    m_hImageWnd = nullptr;
    m_hTextWnd = nullptr;
    m_ptOffset.x = m_ptOffset.y = 0;
    m_hFont = nullptr;
	m_hIcon = nullptr;
}
CDragFeedback::~CDragFeedback()
{
    g_pCurDragFeedback = nullptr;
    if (m_hFont)
    {
        DeleteObject(m_hFont);
        m_hFont = nullptr;
    }
}

// 由drop target调用，设置当前显示文本。
// 每次鼠标拖拽移动通知外部都要重新设置
// 在接下来触发的GiveFeedback中渲染该文本
void  CDragFeedback::SetMeta(HBITMAP hIcon, LPCWSTR szText)
{
    if (szText)
        m_strDescription = szText;
    else
        m_strDescription.clear();

	m_hIcon = hIcon;
}

// 由drop source调用，显示光标样式。此时更新描述文本
HRESULT  CDragFeedback::GiveFeedback(unsigned int dwEffect)
{
    if (m_strPrevDescription != m_strDescription)
    {
        Point pt;
        ::GetCursorPos(&pt);
        UpdateTextWnd(m_hIcon, m_strDescription.c_str(), pt.x+14, pt.y+16);
    }

    // 用完之后立即清空，下一步鼠标可能移到空白处
    m_strPrevDescription = m_strDescription;
    m_strDescription.clear();
	m_hIcon = nullptr;

    if (dwEffect == DROPEFFECT_NONE ||
        dwEffect == DROPEFFECT_SCROLL)
    {
        return DRAGDROP_S_USEDEFAULTCURSORS;
    }
    else if (dwEffect == DROPEFFECT_NOTREADY)
    {
        static HCURSOR hCursorWait = LoadCursor(nullptr, IDC_WAIT);
        SetCursor(hCursorWait);
        return 0;
    }
    else if (m_strPrevDescription.length())
    {
        // 普通光标 加 文本描述
        static HCURSOR hCursorArrow = LoadCursor(nullptr, IDC_ARROW);
        SetCursor(hCursorArrow);
        return 0;
    }
    else
    {
        return DRAGDROP_S_USEDEFAULTCURSORS;
    }
}

void  CDragFeedback::Create(HBITMAP hDragImage, Point ptOffset)
{
    Point pt = {0};
    GetCursorPos(&pt);
    pt.x -= ptOffset.x;
    pt.y -= ptOffset.y;

    m_ptOffset = ptOffset;
    m_hImageWnd = CreateDragImageWnd(hDragImage, pt.x, pt.y);
    InstallDragMouseHook();
}

void  CDragFeedback::Destroy()
{
    UninstallDragMouseHook();
    if (m_hTextWnd)
    {
        DestroyWindow(m_hTextWnd);
        m_hTextWnd = nullptr;
    }
    DestroyWindow(m_hImageWnd);
    m_hImageWnd = nullptr;
}

void  CDragFeedback::UpdateTextWnd(
		HBITMAP hIcon, LPCWSTR szText, int x, int y)
{
    if (!szText || !szText[0])
    {
        if (m_hTextWnd)
        {
            ShowWindow(m_hTextWnd, SW_HIDE);
            return;
        }
        return;
    }

    // 创建窗口
    if (!m_hTextWnd)
    {
        if (!m_hImageWnd)
            return;

        m_hTextWnd = CreateWindowEx(
            WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_TOPMOST|WS_EX_TOOLWINDOW, 
            WND_DRAGBITMAPWND_CLASS_NAME,
            nullptr, 
            WS_POPUP,
            0, 
            0,
            1, 
            1,
            m_hImageWnd,
            nullptr, nullptr, nullptr);

        if (!m_hTextWnd)
            return;
    }

    if (!m_hFont)
    {
        m_hFont = CreateFont(-12, 0, 0, 0, FW_NORMAL,0,0,0,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS,
				TEXT("微软雅黑"));
        if (!m_hFont)
            return;
    }

	if (m_imageBkg.IsNull())
	{
		HMODULE  hModule = GetModuleHandle(TEXT("UISDK.dll"));
		if (hModule)
		{
#ifndef IDB_PNG_TOOLTIP_BKG  
#define IDB_PNG_TOOLTIP_BKG  167
#endif
			m_imageBkg.LoadFromResource(hModule, 
					IDB_PNG_TOOLTIP_BKG, TEXT("PNG"));
		}
	}
    // 测量
    HDC hDC = CreateCompatibleDC(nullptr);
    HFONT hOldFont = (HFONT)SelectObject(hDC, m_hFont);
    Size sizeText = {0};
    GetTextExtentPoint32(hDC, szText, (int)wcslen(szText), &sizeText);
    SelectObject(hDC, hOldFont);
    DeleteDC(hDC);

	Size sizeWnd = sizeText;

	const int icon_text_margin = 3;
	unsigned int nIconW = 0;
	unsigned int nIconH = 0;
	if (hIcon)
	{	
		BITMAP  bm = {0};
		GetObject(hIcon, sizeof(bm), &bm);
		nIconW = bm.bmWidth;
		nIconH = bm.bmHeight;

		sizeWnd.cx += nIconW + icon_text_margin;

// 		if (sizeWnd.cy < nIconH)
// 			sizeWnd.cy = nIconH;
	}

    
    REGION4  padding = {6,6,10,6};
    sizeWnd.cx += padding.left + padding.right;
    sizeWnd.cy += padding.top + padding.bottom;
    
    const unsigned int maxWidth = 300;
    if (sizeWnd.cx > maxWidth)
        sizeWnd.cx = maxWidth;

    UI::Image image;
    image.Create(sizeWnd.cx, -sizeWnd.cy, 32, 
		UI::Image::createAlphaChannel);
    hDC = image.GetDC();
    
    // 背景
	if (m_imageBkg.IsNull())
	{
		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(118,118,118));
		HPEN hOldPen = (HPEN)SelectObject(hDC, hPen); 
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC,
				(HBRUSH)GetStockObject(WHITE_BRUSH));
		Rectangle(hDC, 0, 0, sizeWnd.cx, sizeWnd.cy);
		SelectObject(hDC, hOldPen);
		SelectObject(hDC, hOldBrush);
		DeleteObject(hPen);
	}
	else
	{
		C9Region r;
		r.Set(
			(short)padding.left,
			(short)padding.top,
			(short)padding.right,
			(short)padding.bottom);

		m_imageBkg.Draw(hDC, 0, 0, sizeWnd.cx, sizeWnd.cy,
			0, 0, m_imageBkg.GetWidth(), m_imageBkg.GetHeight(),
			&r, true, 255);
	}

    // 文字 
    SetBkMode(hDC, TRANSPARENT);
    Rect rcText = {0, 0, sizeWnd.cx, sizeWnd.cy};
    rcText.left += padding.left;
    rcText.top += padding.top;
    rcText.right -= padding.right;
    rcText.bottom -= padding.bottom;

	if (hIcon)
	{
		int x = padding.left;
		int y = rcText.top + (rcText.bottom-rcText.top - (int)nIconH)/2;

		HDC hDCMem = CreateCompatibleDC(hDC);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDCMem, hIcon);
		BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
		AlphaBlend(hDC, x, y, nIconW, nIconH,
			hDCMem, 0, 0, nIconW, nIconH, bf);
		SelectObject(hDCMem, hOldBitmap);
		DeleteDC(hDCMem);
		
		rcText.left += nIconW + icon_text_margin;
	}

    hOldFont = (HFONT)SelectObject(hDC, m_hFont);
    ::SetTextColor(hDC, RGB(36,36,36));
    ::DrawText(hDC, szText, -1, &rcText, 
			DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX);
    SelectObject(hDC, hOldFont);

    // 将alpha --> 255
    byte* pBits = (byte*)image.GetBits() + rcText.top * image.GetPitch();
    for (int h = rcText.top; h < rcText.bottom; h++)
    {
        for (int x = rcText.left; x < rcText.right; x++)
            pBits[x*4 +3] = 255;

        pBits += image.GetPitch();
    }

    // 提交到屏幕
    Point ptSrc = {0, 0};
    Point ptDst = {x, y};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(m_hTextWnd, nullptr, 
			&ptDst, &sizeWnd, hDC,
			&ptSrc, 0, &bf, ULW_ALPHA);
    
    image.ReleaseDC();
    ShowWindow(m_hTextWnd, SW_SHOWNOACTIVATE);
}

HWND  CDragFeedback::CreateDragImageWnd(HBITMAP hBitmap, int x, int y)
{
	if (!hBitmap)
		return nullptr;

    RegisterDragBitmapWndClass();

	BITMAP bm = {sizeof(bm)};
	GetObject(hBitmap, sizeof(bm), &bm);

	Point ptDst = {x, y};
	Size  sizeDst = {bm.bmWidth, bm.bmHeight};

	HWND hWnd = CreateWindowEx(
		WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_TOPMOST|WS_EX_TOOLWINDOW, 
		WND_DRAGBITMAPWND_CLASS_NAME,
		nullptr, 
		WS_POPUP,
		ptDst.x, 
		ptDst.y,
		sizeDst.cx, 
		sizeDst.cy,
		nullptr, nullptr, nullptr, nullptr);
	if (!hWnd)
		return nullptr;

	ShowWindow(hWnd, SW_SHOWNOACTIVATE);

	Point ptSrc = {0, 0};
	HDC hDC = CreateCompatibleDC(nullptr);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDC, hBitmap);
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 180, AC_SRC_ALPHA};
	UpdateLayeredWindow(hWnd, nullptr, 
			&ptDst, &sizeDst, hDC, 
			&ptSrc, 0, &bf, ULW_ALPHA);
	SelectObject(hDC, hOldBitmap);
	DeleteDC(hDC);

	
	return hWnd;
}

long CALLBACK MouseProc(
	int nCode,
    long wParam,
    long lParam
)
{
    if (!g_pCurDragFeedback)
        return 0;

    HHOOK hHook = g_pCurDragFeedback->m_hHook;

	if (nCode < 0)
		return CallNextHookEx(hHook, nCode, wParam, lParam);

	if (wParam == WM_MOUSEMOVE || wParam == WM_NCMOUSEMOVE)
	{
		MOUSEHOOKSTRUCT* pStruct = (MOUSEHOOKSTRUCT*)lParam;
        g_pCurDragFeedback->OnMouseMove(pStruct->pt);
	}
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

void  CDragFeedback::OnMouseMove(Point pt)
{
    if (!m_hImageWnd)
        return;

    SetWindowPos(m_hImageWnd, 
        0,
        pt.x - m_ptOffset.x,
        pt.y - m_ptOffset.y,
        0, 0, 
        SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

    if (m_hTextWnd && IsWindowVisible(m_hTextWnd))
    {
        SetWindowPos(m_hTextWnd, 
            0,
            pt.x+14,
            pt.y+16,
            0, 0, 
            SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
    }
}

BOOL  CDragFeedback::InstallDragMouseHook()
{
	if (m_hHook)
		return TRUE;

	m_hHook = SetWindowsHookEx(
			WH_MOUSE, MouseProc, nullptr, GetCurrentThreadId());
	if (!m_hHook)
		return FALSE;

	return TRUE;
}

void  CDragFeedback::UninstallDragMouseHook()
{
	if (m_hHook)
	{
		UnhookWindowsHookEx(m_hHook);
		m_hHook = nullptr;
	}
}

void  CDragFeedback::RegisterDragBitmapWndClass()
{
    static bool s_bRegiste = false;
    if (s_bRegiste)
        return;

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style			= CS_DBLCLKS;  
    wcex.lpfnWndProc	= DefWindowProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= 0;
    wcex.hIcon			= 0;
    wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1); 
    wcex.lpszMenuName	= nullptr;
    wcex.lpszClassName	= WND_DRAGBITMAPWND_CLASS_NAME;
    wcex.hIconSm		= 0;
    RegisterClassEx(&wcex);

    s_bRegiste = true;
}

// 将feedback交给dataobject保存，这样在droptarget的回调中才能
// 通过IDataObject拿到idragfeedbac接口
void  CDragFeedback::Bind2DataObject(IDataObject* pDataObject)
{
    if (!pDataObject)
        return;

    IDataObjectEx* pDataObjectEx = nullptr;
    pDataObject->QueryInterface(IID_IDataObjectEx, (void**)&pDataObjectEx);
    if (pDataObjectEx)
    {
        pDataObjectEx->SetDragFeedback(this);
        pDataObjectEx->Release();
    }
}
void  CDragFeedback::Unbind2DataObject(IDataObject* pDataObject)
{
    if (!pDataObject)
        return;

    IDataObjectEx* pDataObjectEx = nullptr;
    pDataObject->QueryInterface(IID_IDataObjectEx, (void**)&pDataObjectEx);
    if (pDataObjectEx)
    {
        if (pDataObjectEx->GetDragFeedback() == this)
            pDataObjectEx->SetDragFeedback(nullptr);

        pDataObjectEx->Release();
    }
}
void  CDragFeedback::Bind2DropSource(IDropSource* pDropSource)
{
    if (!pDropSource)
        return;

    IDropSourceEx* pDropSourceEx = nullptr;
    pDropSource->QueryInterface(IID_IDropSourceEx, (void**)&pDropSourceEx);
    if (pDropSourceEx)
    {
        pDropSourceEx->SetDragFeedback(this);
        pDropSourceEx->Release();
    }
}
void  CDragFeedback::Unbind2DropSource(IDropSource* pDropSource)
{
    if (!pDropSource)
        return;

    IDropSourceEx* pDropSourceEx = nullptr;
    pDropSource->QueryInterface(IID_IDropSourceEx, (void**)&pDropSourceEx);
    if (pDropSourceEx)
    {
        if (pDropSourceEx->GetDragFeedback() == this)
            pDropSourceEx->SetDragFeedback(nullptr);

        pDropSourceEx->Release();
    }
}

long UIDoDragDrop(
		IDataObject* pDataObject, 
		IDropSource* pDropSource, 
		unsigned int dwOKEffect,         
		unsigned int* pdwEffect,        
		HBITMAP hbmpDragImage,
		Point ptOffset)
{
    CDragFeedback  feedback;
    feedback.Create(hbmpDragImage, ptOffset);
    feedback.Bind2DataObject(pDataObject);
    feedback.Bind2DropSource(pDropSource);
    
    long lRet = DoDragDrop(
			pDataObject, pDropSource, dwOKEffect, pdwEffect);

    feedback.Unbind2DataObject(pDataObject);
    feedback.Unbind2DropSource(pDropSource);
    feedback.Destroy();
    return lRet;
}
}