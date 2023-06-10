#pragma once
#include "Inc\Interface\ipanel.h"
#include "Src\UIObject\Panel\panel.h"

namespace UI
{

class ListCtrlBase;
class ListItemBase;
interface IListItemBase;

class ListItemRootPanel;
interface __declspec(uuid("6EB028AB-A7B1-4EB3-9192-19B7F47AB415"))
IListItemRootPanel : public IPanel
{
    IListItemBase*  GetListItem();

    UI_DECLARE_INTERFACE(ListItemRootPanel);
};


// 内部子控件父Panel. 父对象为ListCtrl,但listctrl不知道自己有这个child object
// 该Panel的区域由ListItemBase::SetParentRect设置
class ListItemRootPanel : public Panel
{
public:
    ListItemRootPanel(IListItemRootPanel* p);

	UI_BEGIN_MSG_MAP()
        UIMSG_QUERYINTERFACE(ListItemRootPanel)
        UIMSG_ERASEBKGND(OnEraseBkgnd)
    UI_END_MSG_MAP_CHAIN_PARENT(Panel)

    IListItemRootPanel*  GetIListItemRootPanel() { return m_pIListItemRootPanel; }

protected:
    void  OnEraseBkgnd(IRenderTarget* pRenderTarget);

public:
    void  SetListCtrlItem(ListCtrlBase* p1, ListItemBase* p2);
    ListItemBase*  GetListItem() { return m_pListItem; }

protected:
    IListItemRootPanel*  m_pIListItemRootPanel;

    ListCtrlBase*   m_pListCtrl;
    ListItemBase*   m_pListItem;
};

}


