#include "stdafx.h"
#include "tooltipwindow.h"
#if 0
#include <Uxtheme.h>

#include "UISDK\Kernel\Src\Renderbase\renderbase\renderbase.h"
#include "UISDK\Kernel\Src\Base\Application\uiapplication.h"

TooltipWindow::TooltipWindow()
{
	m_nLimitWidth = -1;
}
TooltipWindow::~TooltipWindow()
{
	this->Destroy();
}

BOOL TooltipWindow::PreCreateWindow(CREATESTRUCT* pcs)
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);
    if (FALSE == GetCurMsg()->lRet)
        return FALSE;

	pcs->lpszClass = WND_POPUP_CONTROL_SHADOW_NAME;  // ����Ӱ
	pcs->dwExStyle |= WS_EX_TOPMOST|WS_EX_NOACTIVATE;
	return TRUE;
}

void  TooltipWindow::OnSerialize(SerializeParam* pData)
{
	this->EnableWindowLayered(true);  // ����__super::SetAttribute��ָ������Gdiplus������Դ

	CustomWindow::OnSerialize(pData);
	if (NULL == m_pBkgndRender)
	{
		m_pUIApplication->GetRenderBaseFactory().CreateRenderBase(
				RENDER_TYPE_THEME_TOOLTIP_WINDOW_BKGND,
				this->GetIObject(),
				&m_pBkgndRender);
	}

	CRegion4 b(1,1,1,1);
	this->SetBorderRegion(&b);
	CRegion4 r(4,3,4,3);
	this->SetPaddingRegion(&r);      // �������Ե�ļ��

	this->SetResizeCapability(WRSB_NONE);  // ��ֹ��ק

	::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOMOVE|SWP_FRAMECHANGED);
}

void TooltipWindow::OnPaint(IRenderTarget* pRenderTarget)
{
	CRect rc;
	this->GetClientRectWithZeroOffset(&rc);

    DRAWTEXTPARAM  param;
	param.color.ReplaceRGB(GetSysColor(COLOR_INFOTEXT));
	if (IsThemeActive())
	{
		param.color.ReplaceRGB(RGB(87,87,87));  // TODO: GetSysColor(COLOR_INFOTEXT)�ò���WIN7����������ʾ��������ɫ
	}
    param.szText = m_strText.c_str();
    param.prc = &rc;
	pRenderTarget->DrawString(m_pDefaultFont, &param);
}

bool TooltipWindow::Create()
{
// 	if (m_hWnd)
// 		return false;
// 
// 	CustomWindow::Create(_T(""),NULL);
// 
// 	this->SetWindowLayered(true);
// 
// 	ATTRMAP map;
// 	this->SetAttribute(map, false);  // ����һЩĬ������
// 
// 	CRegion4 b(1,1,1,1);
// 	this->SetBorderRegion(&b);
// 	CRegion4 r(4,3,4,3);
// 	this->SetPaddingRegion(&r);      // �������Ե�ļ��
// 
// 	this->SetResizeCapability(WRSB_NONE);  // ��ֹ��ק
// 
// 	::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOMOVE|SWP_FRAMECHANGED);
	return true;
}
bool TooltipWindow::Destroy() 
{
	if (NULL == m_hWnd)
		return false;

	::DestroyWindow(m_hWnd);
	return true;
}

 void TooltipWindow::OnFinalMessage()
 {
 	delete this;
 }

bool TooltipWindow::SetText(const String& strText)
{
	if (NULL == m_hWnd)
	{
		Create();
	}
	m_strText = strText;

	// ���㴰�ڵ��´�С
	SIZE s = {0,0};
	IRenderFont* pFont = this->GetRenderFont();
	if (pFont)
	{
		s= pFont->MeasureString(strText.c_str(), m_nLimitWidth);
	}

    REGION4 rcNonClient = {0};
    GetNonClientRegion(&rcNonClient);

	s.cx += rcNonClient.left + rcNonClient.right;
	s.cy += rcNonClient.top + rcNonClient.bottom;

	this->SetObjectPos(0,0, s.cx,s.cy, SWP_NOMOVE);
	return true;
}
bool TooltipWindow::SetTitle(const String& strText)
{
	if (NULL == m_hWnd)
	{
		Create();
	}
	return false;
}
bool TooltipWindow::Show()
{
	POINT pt;
	::GetCursorPos(&pt);
	this->SetObjectPos(pt.x, pt.y+22, 0,0, SWP_NOSIZE);

	//::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	return true;
}
bool TooltipWindow::Hide() 
{
	this->HideWindow();
	return true;
}
#endif