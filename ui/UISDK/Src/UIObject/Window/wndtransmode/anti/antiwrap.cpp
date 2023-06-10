#include "stdafx.h"
#include "antiwrap.h"
#include "Src/UIObject/Window/customwindow.h"
#include "Inc/Interface/imapattr.h"

namespace UI
{

AntiWindowWrap::AntiWindowWrap()
{
    m_bEnable = false;
}
AntiWindowWrap::~AntiWindowWrap()
{

}

void  AntiWindowWrap::Init(ICustomWindow* pWnd)
{
    __super::Init(pWnd);

    IMapAttribute* pMapAttrib = nullptr;
    m_pWindow->GetMapAttribute(&pMapAttrib);
    if (pMapAttrib)
    {
        const TCHAR* szText = pMapAttrib->GetAttr(
			XML_WINDOW_TRANSPARENT_ANTI_9REGION,
			false);

        if (szText)
		{
            Util::TranslateImage9Region(szText, &m_9region, XML_SEPARATOR);
		}

        SAFE_RELEASE(pMapAttrib);
    }
}

void  AntiWindowWrap::UpdateRgn()
{
	if (false == m_bEnable)  // 取消当前异形
	{
		SetWindowRgn(GetHWND(), nullptr, FALSE); // 先将m_bEnable置为false，避免再进到_OnNcCalcSize中时仍然设置了缩进
		return;
	}

    if (m_9region.IsAll_0())
        return ;

	HWND hWnd = GetHWND();
	UIASSERT(hWnd);

    CRect rc;
    ::GetWindowRect(hWnd, &rc);
    int w = rc.Width();
    int h = rc.Height();

    int rl = m_9region.left;
    int rt = m_9region.top;
    int rr = m_9region.right;
    int rb = m_9region.bottom;

    RECT rect1, rect2, rect3;

    vector<RECT>  vRectRgnData;
    switch (m_9region.topleft)
    {
    case 0:
        break;

    case 1:
        {
            ::SetRect(&rect1, 0, 0, 1, 1);
            ::OffsetRect(&rect1, rl, rt);
            vRectRgnData.push_back(rect1);
        }
        break;

    case 2:  // 2,1模式
        {
            ::SetRect(&rect1, 0, 0, 2, 1);
            ::SetRect(&rect2, 0, 1, 1, 2);

            ::OffsetRect(&rect1, rl, rt);
            vRectRgnData.push_back(rect1);
            ::OffsetRect(&rect2, rl, rt);
            vRectRgnData.push_back(rect2);
        }
        break;

    case 3:  // 4,2,1,1模式
    default:
        {
            ::SetRect(&rect1, 0, 0, 4, 1);
            ::SetRect(&rect2, 0, 1, 2, 2);
            ::SetRect(&rect3, 0, 2, 1, 4);

            ::OffsetRect(&rect1, rl, rt);
            vRectRgnData.push_back(rect1);
            ::OffsetRect(&rect2, rl, rt);
            vRectRgnData.push_back(rect2);
            ::OffsetRect(&rect3, rl, rt);
            vRectRgnData.push_back(rect3);
        }
        break;
    }

    switch (m_9region.topright)
    {
    case 0:
        break;

    case 1:
        {
            ::SetRect(&rect1, w-1, 0, w, 1); 
            ::OffsetRect(&rect1, -rr, rt);
            vRectRgnData.push_back(rect1);
        }
        break;

    case 2:
        {
            ::SetRect(&rect1, w-2, 0, w, 1);
            ::SetRect(&rect2, w-1, 1, w, 2);

            OffsetRect(&rect1, -rr, rt);
            vRectRgnData.push_back(rect1);
            OffsetRect(&rect2, -rr, rt);
            vRectRgnData.push_back(rect2);
        }
        break;

    case 3:
    default:
        {
            ::SetRect(&rect1, w-4, 0, w, 1);
            ::SetRect(&rect2, w-2, 1, w, 2);
            ::SetRect(&rect3, w-1, 2, w, 4);

            ::OffsetRect(&rect1, -rr, rt);
            vRectRgnData.push_back(rect1);
            ::OffsetRect(&rect2, -rr, rt);
            vRectRgnData.push_back(rect2);
            ::OffsetRect(&rect3, -rr, rt);
            vRectRgnData.push_back(rect3);
        }
        break;
    }

    switch (m_9region.bottomleft)
    {
    case 0:
        break;

    case 1:
        {
            ::SetRect(&rect1, 0, h-1, 1, h); //  {0, 0};
            OffsetRect(&rect1, rl, -rb);
            vRectRgnData.push_back(rect1);
        }
        break;

    case 2:
        {
            ::SetRect(&rect1, 0, h-2, 1, h-1);
            ::SetRect(&rect2, 0, h-1, 2, h);

            OffsetRect(&rect1, rl, -rb);
            vRectRgnData.push_back(rect1);
            OffsetRect(&rect2, rl, -rb);
            vRectRgnData.push_back(rect2);
        }
        break;

    case 3:
    default:
        {
            ::SetRect(&rect1, 0, h-1, 4, h);
            ::SetRect(&rect2, 0, h-2, 2, h-1);
            ::SetRect(&rect3, 0, h-4, 1, h-2);

            ::OffsetRect(&rect1, rl, -rb);
            vRectRgnData.push_back(rect1);
            ::OffsetRect(&rect2, rl, -rb);
            vRectRgnData.push_back(rect2);
            ::OffsetRect(&rect3, rl, -rb);
            vRectRgnData.push_back(rect3);
        }
        break;
    }

    switch (m_9region.bottomright)
    {
    case 0:
        break;

    case 1:
        {
            ::SetRect(&rect1, w-1, h-1, w, h);
            OffsetRect(&rect1, -rr, -rb);
            vRectRgnData.push_back(rect1);
        }
        break;

    case 2:
        {
            ::SetRect(&rect1, w-1, h-2, w, h-1);
            ::SetRect(&rect2, w-2, h-1, w, h);

            OffsetRect(&rect1, -rr, -rb);
            vRectRgnData.push_back(rect1);
            OffsetRect(&rect2, -rr, -rb);
            vRectRgnData.push_back(rect2);
        }
        break;

    case 3:
    default:
        {
            ::SetRect(&rect1, w-4, h-1, w, h);
            ::SetRect(&rect2, w-2, h-2, w, h-1);
            ::SetRect(&rect3, w-1, h-4, w, h-2);

            ::OffsetRect(&rect1, -rr, -rb);
            vRectRgnData.push_back(rect1);
            ::OffsetRect(&rect2, -rr, -rb);
            vRectRgnData.push_back(rect2);
            ::OffsetRect(&rect3, -rr, -rb);
            vRectRgnData.push_back(rect3);
        }
        break;
    }

    HRGN hRgn2Delete =  CreateRgnByVectorRect(vRectRgnData);

    CRect rectRgn( 0, 0, w, h);
    rectRgn.DeflateRect(rl, rt, rr, rb);

    HRGN  hRgn = CreateRectRgnIndirect(&rectRgn);
    CombineRgn(hRgn, hRgn, hRgn2Delete, RGN_DIFF);
    ::SetWindowRgn(hWnd, hRgn, TRUE);
    // DeleteObject(hRgn);  // TODO: MSDN上表示不需要销毁该对象
    DeleteObject(hRgn2Delete);
}

void  AntiWindowWrap::Enable(bool b)
{
    m_bEnable = b;
	// 不在这里立即设置，延迟到下一次updatergn中
	// 即使设置了不刷新，还是会在界面上刷出系统标题栏出来，造成闪烁。
	// 	if (false == b)  // 取消当前异形
	// 	{
	// 		SetWindowRgn(GetHWND(), nullptr, FALSE); // 先将m_bEnable置为false，避免再进到_OnNcCalcSize中时仍然设置了缩进
	// 	}
}


LRESULT  AntiWindowWrap::_OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return 0;
}
}