#include "stdafx.h"
#include "aerowrap.h"
#include "src/UIObject\Window\customwindow.h"
#include "include/interface/iattributemap.h"

namespace ui
{
AeroWindowWrap::AeroWindowWrap()
{
    m_pDWM = DwmHelper::GetInstance();
    m_pAeroDisableMode = nullptr;
    m_pIAeroWindowWrap = nullptr;
    m_eAeroDisableMode = WINDOW_TRANSPARENT_TYPE_LAYERED;
    m_eMode = AERO_MODE_BLUR;
    ::SetRect(&blur.m_regionBlur, -1, -1, -1, -1);
    blur.m_hrgnBlurRgn = nullptr;
    blur.m_bAutoClipChildCtrl = false;
}
AeroWindowWrap::~AeroWindowWrap()
{
    m_pDWM = nullptr;
    SAFE_DELETE(m_pIAeroWindowWrap);
    SAFE_RELEASE(m_pAeroDisableMode);
    SAFE_DELETE_GDIOBJECT(blur.m_hrgnBlurRgn);
}

void  AeroWindowWrap::Init(ICustomWindow* pWnd)
{
    __super::Init(pWnd);

    IMapAttribute* pMapAttrib = m_pWindow->m_pIMapAttributeRemain;
    if (pMapAttrib)
    {
        const TCHAR* szText = pMapAttrib->GetAttr(
			XML_WINDOW_TRANSPARENT_TYPE_AERO_DISABLE, true);
        if (szText)
            m_eAeroDisableMode = GetTransparentModeTypeFromAttr(szText);

        szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_AERO_MODE, false);
        if (szText)
        {
            if (0 == _tcscmp(szText, XML_WINDOW_TRANSPARENT_AERO_MODE_BLUR))
            {
                m_eMode = AERO_MODE_BLUR;
            }
            else if (0 == _tcscmp(szText, XML_WINDOW_TRANSPARENT_AERO_MODE_TRANS))
            {
                m_eMode = AERO_MODE_TRANSPARENT;
                UIASSERT(GetWindowLongPtr(GetHWND(), GWL_STYLE)&WS_CAPTION);
            }
            else
            {
                m_eMode = AERO_MODE_BLUR;
            }
        }

        if (AERO_MODE_TRANSPARENT == m_eMode)
        {
            szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_AERO_TRANS_MARGINS, false);
            if (szText)
            {
				// 注： 如果这个值过小，会导致窗口四个圆角变形！建议至少为5? 7?
                RECT rc;
                if (util::TranslateRECT(szText, &rc, XML_SEPARATOR))
                {
                    trans.m_margins.cxLeftWidth = rc.left;
                    trans.m_margins.cxRightWidth = rc.right;
                    trans.m_margins.cyTopHeight = rc.top;
                    trans.m_margins.cyBottomHeight = rc.bottom;
#ifdef _DEBUG
					UIASSERT(rc.left == -1 || rc.left >= 5);
					UIASSERT(rc.right == -1 || rc.right >= 5);
					UIASSERT(rc.top == -1 || rc.top >= 5);
					UIASSERT(rc.bottom == -1 || rc.bottom >= 5);
#endif
                }
            }
        }
        else
        {
            szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_AERO_BLUR_REGION, false);
            if (szText)
            {
                util::TranslateRECT(szText, &blur.m_regionBlur, XML_SEPARATOR);
            }
            pMapAttrib->GetAttr_bool(XML_WINDOW_TRANSPARENT_AERO_BLUR_AUTOCLIPCHILDCTRL, false, &blur.m_bAutoClipChildCtrl);
        }
    }
}

long  AeroWindowWrap::_OnDwmCompositionChanged(unsigned int uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    Enable(true);
    m_pWindow->m_bNeedToSetWindowRgn = true;
    m_pWindow->Invalidate();

    bHandled = FALSE;
    return 0;
}

IAeroWindowWrap*  AeroWindowWrap::GetIAeroWndTransMode()
{
    if (!m_pIAeroWindowWrap)
        m_pIAeroWindowWrap = new IAeroWindowWrap(this);

    return m_pIAeroWindowWrap;
}

AERO_MODE  AeroWindowWrap::GetAeroMode()
{
    return m_eMode;
}   
void  AeroWindowWrap::GetBlurRegion(CRegion4* pregion)
{
    if (pregion && m_eMode == AERO_MODE_BLUR)
    {
        pregion->CopyRect(&blur.m_regionBlur);
    }
}

void  AeroWindowWrap::SetTransparentMargins(RECT* prcMargin)
{
	if (m_eMode != AERO_MODE_TRANSPARENT)
		return;

	trans.m_margins.cxLeftWidth = prcMargin->left;
	trans.m_margins.cxRightWidth = prcMargin->right;
	trans.m_margins.cyTopHeight = prcMargin->top;
	trans.m_margins.cyBottomHeight = prcMargin->bottom;
}

void  AeroWindowWrap::Enable(bool b)
{
	HWND hWnd = GetHWND();
	UIASSERT(hWnd);

    if (m_pDWM->IsEnable())
    {
        if (b)
        {
            if (m_pAeroDisableMode)
            {
                m_pAeroDisableMode->Enable(false);
                SAFE_RELEASE(m_pAeroDisableMode);
            }

			if (m_eMode == AERO_MODE_TRANSPARENT)
			{
				// 注：使用自已的图片阴影+aero透明会导致一个解决不了的问题：窗口改变大小时出现黑边问题，如拖拽和最大化时
				//     为了规避这个问题，只能采用aero自带的阴影，因此得添加一个WS_THICKFRAME属性以开启阴影，再加上WS_NCCALCSIZE/WS_NCACTIVATE消息
				//     屏蔽边框的显示
				LONG_PTR lStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
				SetWindowLongPtr(hWnd, GWL_STYLE, lStyle|WS_THICKFRAME);
				SetWindowPos(hWnd, 0,0,0,0,0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_FRAMECHANGED);
			}
            return;
            // 由外部调用UpdateRgn即可
        }
        else
        {
            if (m_eMode == AERO_MODE_BLUR)
            {
                DwmHelper* pDwm = DwmHelper::GetInstance();
                if (pDwm->pDwmEnableBlurBehindWindow)
                {
                    DWM_BLURBEHIND blurbehind = {0};
                    blurbehind.dwFlags = DWM_BB_ENABLE;
                    blurbehind.fEnable = FALSE;
                    pDwm->pDwmEnableBlurBehindWindow(hWnd, &blurbehind);
                }

				DwmHelper::GetInstance()->EnableWindows10Blur(hWnd, false);
            }
            else
            {
				if (!DwmHelper::GetInstance()->EnableWindows10Trans(hWnd, false))
				{
					if (m_pDWM->pDwmExtendFrameIntoClientArea)
					{
						MARGINS m = { 0 };
						m_pDWM->pDwmExtendFrameIntoClientArea(hWnd, &m);
					}
				}

				if (m_eMode == AERO_MODE_TRANSPARENT)
				{
					LONG_PTR lStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
					if (lStyle & WS_THICKFRAME)
					{
						SetWindowLongPtr(hWnd, GWL_STYLE, lStyle & ~WS_THICKFRAME);
						SetWindowPos(hWnd, 0,0,0,0,0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_FRAMECHANGED);
					}
				}
            }
        }
    }
    
    do 
    {
        if (m_pAeroDisableMode)
        {
            m_pAeroDisableMode->Enable(true);
            break;
        }

        m_pAeroDisableMode = CreateTransparentModeByType(m_eAeroDisableMode);
        if (!m_pAeroDisableMode)
            break;

        m_pAeroDisableMode->Init(m_pWindow->m_pICustomWindow);
        m_pAeroDisableMode->Enable(true);

    } while (0);
}


struct EnumChildWindowAeroRgnData
{
    int nCount;
    vector<CRect>  m_vec;
    HWND  m_hParent;
};

BOOL CALLBACK EnumChildWindowAeroRgn(HWND hWnd, LPARAM lParam)
{
    EnumChildWindowAeroRgnData*  pData = (EnumChildWindowAeroRgnData*)lParam;
    if (IsWindowVisible(hWnd))
    {
        if (GetParent(hWnd) == pData->m_hParent)
        {
            pData->nCount ++;
            CRect rc;
            GetWindowRect(hWnd, &rc);
            ::MapWindowPoints(nullptr, pData->m_hParent, (LPPOINT)&rc, 2);

            pData->m_vec.push_back(rc);
        }
    }
    return TRUE;
}

void  AeroWindowWrap::UpdateRgn()
{
	HWND hWnd = GetHWND();
	UIASSERT(hWnd);

    if (m_pAeroDisableMode)
    {
        m_pAeroDisableMode->UpdateRgn();
        return;
    }

    DwmHelper*  pDwm = DwmHelper::GetInstance();
	if (!pDwm->IsEnable())
		return;

	if (m_eMode == AERO_MODE_BLUR)
	{
		if (pDwm->pDwmEnableBlurBehindWindow)
		{
			DWM_BLURBEHIND blurbehind = { 0 };
			blurbehind.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION | DWM_BB_TRANSITIONONMAXIMIZED;
			blurbehind.fEnable = TRUE;
			blurbehind.fTransitionOnMaximized = TRUE;  // 这个字段到底是做什么的？

			if (blur.m_hrgnBlurRgn)
			{
				blurbehind.hRgnBlur = blur.m_hrgnBlurRgn;
				pDwm->pDwmEnableBlurBehindWindow(hWnd, &blurbehind);
			}
			else if (blur.m_bAutoClipChildCtrl)
			{
				CRect  rcClient;
				::GetClientRect(hWnd, &rcClient);
				rcClient.DeflateRect(blur.m_regionBlur.left,
					blur.m_regionBlur.top,
					blur.m_regionBlur.right,
					blur.m_regionBlur.bottom);

				HRGN hRgn = CreateRectRgnIndirect(&rcClient);

				// 抠掉子窗口
				EnumChildWindowAeroRgnData  data;
				data.m_hParent = hWnd;
				data.nCount = 0;
				EnumChildWindows(hWnd, EnumChildWindowAeroRgn, (LPARAM)&data);
				if (data.nCount > 0)
				{
					for (int i = 0; i < data.nCount; i++)
					{
						HRGN hRgnChild = CreateRectRgnIndirect(&data.m_vec[i]);
						::CombineRgn(hRgn, hRgn, hRgnChild, RGN_DIFF);
						DeleteObject(hRgnChild);
					}
				}

				blurbehind.hRgnBlur = hRgn;
				pDwm->pDwmEnableBlurBehindWindow(hWnd, &blurbehind);
				DeleteObject(hRgn);
			}
			else
			{
				CRect  rcClient;
				::GetClientRect(hWnd, &rcClient);

				// TBD: blurbehind.hRgnBlur的意义和MSDN上说的结果相反。
				//      blurbehind.hRgnBlur试验出来的结果是不需要blur的区域，而
				//      不是需要blur的区域。因此要将整个窗口都设置为blur，现在的做法
				//      是创建一个特殊的rgn(-1,-1,0,0)
				HRGN hRgn = nullptr;
				if (blur.m_regionBlur.left == -1 &&
					blur.m_regionBlur.top == -1 &&
					blur.m_regionBlur.right == -1 &&
					blur.m_regionBlur.bottom == -1
					)
				{
					if (util::IsWin8OrLater())
						hRgn = CreateRectRgn(-1, -1, 0, 0);
					else
						hRgn = CreateRectRgnIndirect(&rcClient);
				}
				else
				{
					if (util::IsWin8OrLater())
					{
// 						HRGN hRgnFull = CreateRectRgnIndirect(&rcClient);
// 						rcClient.DeflateRect(blur.m_regionBlur.left,
// 							blur.m_regionBlur.top,
// 							blur.m_regionBlur.right,
// 							blur.m_regionBlur.bottom);
// 
// 						HRGN hRgnExclude = CreateRectRgnIndirect(&rcClient);
// 						hRgn = CreateRectRgn(0, 0, 1, 1);
// 						int nRet = ::CombineRgn(hRgn, hRgnFull, hRgnExclude, RGN_DIFF);
// 						DeleteObject(hRgnExclude);
// 						DeleteObject(hRgnFull);

						DwmHelper::GetInstance()->EnableWindows10Blur(hWnd, true);
					}
					else
					{
						CRect  rcClient;
						::GetClientRect(hWnd, &rcClient);
						rcClient.DeflateRect(blur.m_regionBlur.left,
							blur.m_regionBlur.top,
							blur.m_regionBlur.right,
							blur.m_regionBlur.bottom);
						hRgn = CreateRectRgnIndirect(&rcClient);

						blurbehind.hRgnBlur = hRgn;
						pDwm->pDwmEnableBlurBehindWindow(hWnd, &blurbehind);
						DeleteObject(hRgn);
					}
				}
			}
		}
	}
	else if (AERO_MODE_TRANSPARENT == m_eMode)
	{
		if (!DwmHelper::GetInstance()->EnableWindows10Trans(hWnd, true))
		{
			if (m_pDWM->pDwmExtendFrameIntoClientArea)
			{
				trans.m_margins.cxLeftWidth = 50;
				trans.m_margins.cxRightWidth = 50;
				trans.m_margins.cyTopHeight = 50;
				trans.m_margins.cyBottomHeight = 50;
				m_pDWM->pDwmExtendFrameIntoClientArea(hWnd, &trans.m_margins);

				// 禁用系统边框和阴影
				if (m_pDWM->pDwmSetWindowAttribute)
				{
					DWMNCRENDERINGPOLICY ncrp = DWMNCRP_DISABLED;
					m_pDWM->pDwmSetWindowAttribute(hWnd, DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));
				}
			}
		}
	}
}

WINDOW_TRANSPARENT_TYPE  AeroWindowWrap::GetType()
{
    if (m_pAeroDisableMode)
        return m_pAeroDisableMode->GetType();

    return WINDOW_TRANSPARENT_TYPE_AERO;
}

bool  AeroWindowWrap::RequireAlphaChannel()
{
    if (m_pAeroDisableMode)
        return m_pAeroDisableMode->RequireAlphaChannel();

    return true;
}
bool  AeroWindowWrap::Commit()
{
    if (m_pAeroDisableMode)
    {
        return m_pAeroDisableMode->Commit();
    }

    return false;
}

}