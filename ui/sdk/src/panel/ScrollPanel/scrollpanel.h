#pragma once
#include "include/interface/iscrollpanel.h"
#include "..\..\ScrollBar\scrollbarmanager.h"
#include "..\panel.h"

namespace ui
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
	virtual void  virtualOnSize(unsigned int nType, unsigned int nWidth, unsigned int nHeight);

    HRESULT  FinalConstruct(IResource* p);
	void  OnSerialize(SerializeParam* pData);
    BOOL  OnMouseWheel(unsigned int nFlags, short zDelta, POINT pt);
	void  OnLButtonDown(unsigned int nFlags, POINT point);

protected:
    IScrollPanel*      m_pIScrollPanel;
	ScrollBarManager   m_mgrScrollBar;

    SIZE    m_sizeView;  // 视图大小（包含滚动区域），如果未指定，则获取所有子控件的位置进行计算
};
}