#include "stdafx.h"
#include "listctrlbase.h"
#include "src/resource/res_bundle.h"
#include "CustomItem\custom_listitem.h"
#include "src/UIObject\Panel\panel.h"
#include "ListItemRootPanel\listitemrootpanel.h"

CustomListItem*  ListCtrlBase::InsertCustomItem(
                    const wchar_t*  szLayoutName,
                    long lId,
                    IListItemBase* pParent, 
                    IListItemBase* pInsertAfter)
{
    if (!szLayoutName)
        return nullptr;

    UIElementProxy element = m_resource_bundle->
            GetLayoutManager().FindListItemElement(szLayoutName);
    if (!element)
        return nullptr;

    ICustomListItem* pItem = ICustomListItem::
            CreateInstance(m_resource_bundle->GetIResource());

    CustomListItem* pImpl = pItem->GetImpl();
    if (!this->InsertItem(pImpl, pParent, pInsertAfter))
    {
        SAFE_RELEASE(pItem);
        return nullptr;
    }

    pImpl->SetId(lId);
    pImpl->CreateRootPanel();

	Panel* pRootPanel = pImpl->GetRootPanel();

    // 解析子控件
    pRootPanel->LoadAttributeFromXml(element.get(), false);

    // 解析子控件
    m_resource_bundle->GetLayoutManager().ParseChildElement(
        element.get(), pRootPanel, NOTIFY_TARGET_NULL);
 
	
	// 发送初始化通知
	Object* p = nullptr;
	while (p = pRootPanel->EnumAllChildObject(p))
	{
		p->RouteMessage(UI_MSG_INITIALIZE);
		Object::ForwardInitializeMessageToDecendant(p);
		p->RouteMessage(UI_MSG_INITIALIZE2);
	}
    return pImpl;
}

CustomListItem*  ListCtrlBase::GetCustomListItem(Object* pChildControl)
{
	if (!pChildControl)
		return nullptr;

	Object* pParent = nullptr;
	while (pParent = pChildControl->GetParentObject())
	{
		if (pParent == static_cast<Object*>(this))
		{
			ListItemBase* pItem = 
				static_cast<ListItemRootPanel*>(pChildControl)->GetListItem();

			return static_cast<CustomListItem*>(pItem);
		}
		pChildControl = pParent;
	}

	return nullptr;
}