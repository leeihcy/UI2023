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

     // �����н��㣬����������Ӧmousewheel��Ϣ���й���
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
// 1. ָ����viewSize��ֱ�Ӵ���viewSize����������Ϊrange
// 2. û��ָ��viewSize����ʹ��DesiredSize��Ϊ������range
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

// ֱ����ק����
void  ScrollPanel::OnLButtonDown(unsigned int nFlags, POINT point)
{
		// �����������ֱ���϶�����
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