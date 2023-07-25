#include "stdafx.h"
#include "listitemrootpanel.h"
#include "src/UIObject\ListCtrl\listctrlbase.h"
#include "src/UIObject\ListCtrl\ListItemBase\listitembase.h"

namespace ui
{

UI_IMPLEMENT_INTERFACE(ListItemRootPanel, Panel);

IListItemBase*  IListItemRootPanel::GetListItem()
{
    ListItemBase* p = __pImpl->GetListItem();
	if (p)
		return p->GetIListItemBase();

	return nullptr;
}

ListItemRootPanel::ListItemRootPanel(IListItemRootPanel* p):Panel(p)
{
    m_pIListItemRootPanel = p;

    m_pListCtrl = nullptr;
    m_pListItem = nullptr;

	OBJSTYLE s = { 0 };
	s.transparent = 1;
	m_pIListItemRootPanel->ModifyObjectStyle(nullptr, &s);  // 保证可见，这样子对象在单独刷新时，就不用继续往上遍历了
}


void  ListItemRootPanel::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    if (nullptr == pRenderTarget)
        return;

    if (nullptr == m_pListCtrl || nullptr == m_pListItem)
        return;

    // 将DC偏移量移到listctrl左上角
    POINT ptSave = {0, 0};
    pRenderTarget->GetOrigin((int*)&ptSave.x, (int*)&ptSave.y);

    CRect  rcWindow;
    this->GetWindowRect(&rcWindow);
    
    POINT pt = { GetParentRectL(), GetParentRectT() };
    pRenderTarget->OffsetOrigin(-pt.x, -pt.y);

    m_pListCtrl->RedrawItemByInnerCtrl(pRenderTarget, m_pListItem);

    pRenderTarget->OffsetOrigin(pt.x, pt.y);

    __super::OnEraseBkgnd(pRenderTarget);
}

void  ListItemRootPanel::SetListCtrlItem(ListCtrlBase* p1, ListItemBase* p2)
{
    m_pListCtrl = p1;
    m_pListItem = p2;
}


}