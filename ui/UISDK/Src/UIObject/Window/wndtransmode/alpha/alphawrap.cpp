#include "stdafx.h"
#include "alphawrap.h"
#include "Src/UIObject/Window/customwindow.h"
#include "Inc/Interface/imapattr.h"

namespace ui
{

AlphaMaskWindowWrap::AlphaMaskWindowWrap()
{
    m_nAlphaMask = 1;  // 当alpha值为0时，将抠除
}   
AlphaMaskWindowWrap::~AlphaMaskWindowWrap()
{

}

void  AlphaMaskWindowWrap::Init(ICustomWindow* pWnd)
{
    __super::Init(pWnd);

    IMapAttribute* pMapAttrib = nullptr;
    m_pWindow->GetMapAttribute(&pMapAttrib);
    if (pMapAttrib)
    {
        const TCHAR* szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_MASKALPHA_9REGION, false);
        util::TranslateImage9Region(szText, &m_9region, XML_SEPARATOR);

        // 透明度
        szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_MASKALPHA_VALUE, true);
        if (szText)
        {
            m_nAlphaMask = (byte)_ttoi(szText);
        }

        SAFE_RELEASE(pMapAttrib);
    }
}


void  AlphaMaskWindowWrap::Enable(bool b)
{
	if (false == b)  // 取消当前异形
	{
		SetWindowRgn(GetHWND(), nullptr, FALSE);
	}
}

void  AlphaMaskWindowWrap::UpdateRgn()
{
	if (m_9region.IsAll_1())
		return;

	HWND hWnd = GetHWND();
	UIASSERT(hWnd);

	byte* pScan0 = nullptr;
	int  nPitch = 0;
	if (!GetWindowBitmapInfo(&pScan0, nPitch))
		return;

	vector<POINT> vecPt;
	RECT rc = {0};

	CRect rcWnd;
	m_pWindow->GetParentRect(&rcWnd);

	if (m_9region.IsAll_0())
	{
		// 整个窗口过滤
		GetRgnData(pScan0, nPitch, &rcWnd, vecPt);
	}
	else
	{
		int nTop = abs(m_9region.top);
		int nBottom = abs(m_9region.bottom);

		if (nTop > 0)
		{
			if (m_9region.topleft)
			{
				SetRect(&rc, 0, 0, m_9region.topleft, nTop);
				GetRgnData(pScan0, nPitch, &rc, vecPt);
			}

			if (m_9region.topright)
			{
				SetRect(&rc, rcWnd.right-m_9region.topright, 0, rcWnd.right, nTop);
				GetRgnData(pScan0, nPitch, &rc, vecPt);
			}

			if (m_9region.top > 0)
			{
				SetRect(&rc, m_9region.topleft, 0, rcWnd.right-m_9region.topright, nTop);
				GetRgnData(pScan0, nPitch, &rc, vecPt);
			}
		}

		if (m_9region.left > 0)
		{
			SetRect(&rc, 0, nTop, m_9region.left, rcWnd.bottom-nBottom);
			GetRgnData(pScan0, nPitch, &rc, vecPt);
		}
		if (m_9region.right > 0)
		{
			SetRect(&rc, rcWnd.right-m_9region.right, nTop, rcWnd.right, rcWnd.bottom-nBottom);
			GetRgnData(pScan0, nPitch, &rc, vecPt);
		}

		if (nBottom > 0)
		{
			if (m_9region.bottomleft)
			{
				SetRect(&rc, 0, rcWnd.bottom-nBottom, m_9region.bottomleft, rcWnd.bottom);
				GetRgnData(pScan0, nPitch, &rc, vecPt);
			}
			if (m_9region.bottomright)
			{
				SetRect(&rc, rcWnd.right-m_9region.bottomright, rcWnd.bottom-nBottom, rcWnd.right, rcWnd.bottom);
				GetRgnData(pScan0, nPitch, &rc, vecPt);
			}

			if (m_9region.bottom > 0)
			{
				SetRect(&rc, m_9region.bottomleft, rcWnd.bottom-nBottom, rcWnd.right-m_9region.bottomright, rcWnd.bottom);
				GetRgnData(pScan0, nPitch, &rc, vecPt);
			}
		}
	}

	HRGN hRgnExclude = CreateRgnByVectorPoint(vecPt);
	if (!hRgnExclude)
		return;

	HRGN hRgnWindow = ::CreateRectRgn(0,0,m_pWindow->GetWidth(), m_pWindow->GetHeight());
	::CombineRgn(hRgnWindow, hRgnWindow, hRgnExclude,  RGN_DIFF);

	// 如果这里的参数不去刷新的话，就会在屏幕上残留下这些被裁掉的region区域。
	// MSDN: Typically, you set bRedraw to TRUE if the window is visible. 
	SetWindowRgn(hWnd, hRgnWindow, ::IsWindowVisible(hWnd));
	SAFE_DELETE_GDIOBJECT(hRgnExclude);
	// SAFE_DELETE_GDIOBJECT(hRgnWindow); // TODO: MSDN上表示不需要销毁该对象
}

void  AlphaMaskWindowWrap::GetRgnData(BYTE* pScan0, int nPitch, RECT* prc, vector<POINT>& vecPt)
{
    if(!pScan0 || !prc)
        return;

    if (::IsRectEmpty(prc))
        return;

    if (m_nAlphaMask == 0 || m_nAlphaMask == 255)
        return;

    BYTE* p = pScan0;
    POINT pt = {0, 0};

    for (LONG i = prc->top; i < prc->bottom; i++ )   
    {
        p = pScan0 + nPitch*i + prc->left*4;
        for (LONG j = prc->left; j < prc->right; j++)      
        {
            if (p[3] < m_nAlphaMask)
            {
                pt.x = j;
                pt.y = i;
                vecPt.push_back(pt);
            }
            p += 4;
        }
    }
}

}