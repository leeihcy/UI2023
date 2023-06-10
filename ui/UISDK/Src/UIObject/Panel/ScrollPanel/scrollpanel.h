#pragma once
#include "Inc\Interface\iscrollpanel.h"
#include "..\..\ScrollBar\scrollbarmanager.h"
#include "..\panel.h"

namespace UI
{

class ScrollPanel : public Panel
{
public:
	ScrollPanel(IScrollPanel* p);
	~ScrollPanel();

	UI_BEGIN_MSG_MAP()
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        UICHAIN_MSG_MAP_MEMBER(m_mgrScrollBar)
        UIMSG_SERIALIZE(OnSerialize)
        UIMSG_FINALCONSTRUCT(FinalConstruct)
	UI_END_MSG_MAP_CHAIN_PARENT(Panel)

    IScrollPanel*  GetIScrollPanel() { return m_pIScrollPanel; }

protected:
	virtual void  virtualOnSize(UINT nType, UINT nWidth, UINT nHeight);

    HRESULT  FinalConstruct(ISkinRes* p);
	void  OnSerialize(SERIALIZEDATA* pData);
    BOOL  OnMouseWheel(UINT nFlags, short zDelta, POINT pt);
	void  OnLButtonDown(UINT nFlags, POINT point);

protected:
    IScrollPanel*      m_pIScrollPanel;
	ScrollBarManager   m_mgrScrollBar;

    SIZE    m_sizeView;  // 视图大小（包含滚动区域），如果未指定，则获取所有子控件的位置进行计算
};
}