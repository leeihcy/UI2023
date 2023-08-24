#include "stdafx.h"
#include "scrollpanel.h"
#include "include/interface/ilayout.h"
#include "..\..\Window\windowbase.h"

namespace ui
{

ScrollPanel::ScrollPanel(IScrollPanel* p) : Panel(p)
{
    m_pIScrollPanel = p;
    m_sizeView.cx = m_sizeView.cy = NDEF;
}
ScrollPanel::~ScrollPanel()
{
}

HRESULT  ScrollPanel::FinalConstruct(IResource* p)
{
    DO_PARENT_PROCESS(IScrollPanel, IPanel);
    if (FAILED(GetCurMsg()->lRet))
        return (HRESULT)GetCurMsg()->lRet;

    m_mgrScrollBar.SetBindObject(m_pIScrollPanel);

     // 允许有焦点，这样才能响应mousewheel消息进行滚动
    OBJSTYLE s = {0};
    s.default_tabstop = 1;
    s.tabstop = 1;
	s.vscroll = 1;
	s.hscroll = 1;

	OBJSTYLE sRemove = {0};
	sRemove.default_reject_all_mouse_msg = sRemove.reject_all_mouse_msg = 1;
	sRemove.default_reject_self_mouse_msg = sRemove.reject_self_mouse_msg = 1;
	this->ModifyObjectStyle(&s, &sRemove);

    return 0;
}

void  ScrollPanel::OnSerialize(SerializeParam* pData)
{
	__super::OnSerialize(pData);

	{
		AttributeSerializer as(pData, TEXT("ScrollPanel"));
		as.AddSize(XML_SCROLLPANEL_VIEWSIZE, m_sizeView)->AsData();
	}

    m_mgrScrollBar.Serialize(pData);
}

//
// 1. 指定了viewSize，直接传递viewSize给滚动条作为range
// 2. 没有指定viewSize，则使用DesiredSize作为滚动条range
void  ScrollPanel::virtualOnSize(unsigned int nType, unsigned int cx, unsigned int cy)
{
    Panel::virtualOnSize(nType, cx, cy);

    SIZE sizeContent = {0, 0};
    if (m_sizeView.cx != NDEF && m_sizeView.cy != NDEF)
    {
        sizeContent.cx = m_sizeView.cx;
        sizeContent.cy = m_sizeView.cy;
    }
    else
    {
        ILayout* pLayout = m_pIScrollPanel->GetLayout();
        if (nullptr == pLayout)
            return;

        sizeContent = pLayout->Measure();
    }

    CRect rcClient;
    m_pIScrollPanel->GetClientRectInObject(&rcClient);
    SIZE sizePage = { rcClient.Width(), rcClient.Height() };
    m_mgrScrollBar.SetScrollPageAndRange(&sizePage, &sizeContent);
}


BOOL  ScrollPanel::OnMouseWheel(unsigned int nFlags, short zDelta, POINT pt)
{
    int nhScrollPos = 0, nvScrollPos = 0;
    int nhScrollPos2 = 0, nvScrollPos2 = 0;

    m_mgrScrollBar.GetScrollPos(&nhScrollPos, &nvScrollPos);
    m_mgrScrollBar.nvProcessMessage(GetCurMsg(), 0, false);
    m_mgrScrollBar.GetScrollPos(&nhScrollPos2, &nvScrollPos2);

    if (nvScrollPos != nvScrollPos2 || nhScrollPos != nhScrollPos2)
    {
        this->Invalidate();
    }

    return 0;
}

// 直接拖拽窗口
void  ScrollPanel::OnLButtonDown(unsigned int nFlags, POINT point)
{
		// 这种情况下面直接拖动窗口
	WindowBase* pWindowBase = GetWindowObject();
	if (pWindowBase)
	{
		pWindowBase->SetPressObject(nullptr);
		ReleaseCapture();

		::SendMessage(pWindowBase->GetHWND(), WM_NCLBUTTONDOWN, HTCAPTION, GetMessagePos());
		return;
	}

	SetMsgHandled(FALSE);
}
}