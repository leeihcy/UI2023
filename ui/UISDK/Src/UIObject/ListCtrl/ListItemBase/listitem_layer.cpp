#include "stdafx.h"
#include "listitem_layer.h"
#include "Src\Base\Object\object_layer.h"
#include "..\listctrlbase.h"
#include "listitembase.h"
#include "..\..\Window\windowbase.h"

using namespace UI;

ListItemLayer::ListItemLayer(ListItemBase& o) : m_item(o)
{
	m_pLayer = nullptr;
}

ListItemLayer::~ListItemLayer()
{
	DestroyLayer();
}


void UI::ListItemLayer::CreateLayer()
{
	if (m_pLayer)
		return;

	ListCtrlBase* listctrl = m_item.GetListCtrlBase();
	if (!listctrl)
		return;

	WindowRender* pWndRender = nullptr;

	WindowBase* pWindow = listctrl->GetWindowObject();
	if (pWindow)
		pWndRender = pWindow->GetWindowRender();
	if (!pWndRender)
		return;

	m_pLayer = pWndRender->CreateLayer(static_cast<IListItemLayerContent*>(this));

	// 将layer插入到树中
	CRect rcParent;
	m_item.GetParentRect(&rcParent);
	if (!IsRectEmpty(&rcParent))
		OnSize(rcParent.Width(), rcParent.Height());

	// m_obj.OnLayerCreate();
	m_pLayer->Invalidate(nullptr);
}

void  ListItemLayer::TryDestroyLayer()
{
	if (!m_pLayer)
		return;

	m_pLayer->TryDestroy();
}

void  ListItemLayer::DestroyLayer()
{
	if (!m_pLayer)
		return;

	Layer*  p = m_pLayer;
	m_pLayer = nullptr;

	if (p)
		p->Destroy();
}

void  ListItemLayer::OnSize(uint nWidth, uint nHeight)
{
	if (m_pLayer)
	{
		m_pLayer->OnSize(nWidth, nHeight);
	}
}


void  ListItemLayer::Draw(UI::IRenderTarget* pRenderTarget)
{
	m_item.Draw2Layer(pRenderTarget);
}

UI::ListItemBase& UI::ListItemLayer::GetListItem()
{
	return m_item;
}

void UI::ListItemLayer::OnLayerDestory()
{
	if (m_pLayer)  
	{
		m_pLayer = nullptr;
		
		// m_obj.OnLayerDestory();
		m_item.Invalidate();
	}
}

void  ListItemLayer::GetWindowRect(RECT* prcOut)
{
	ListCtrlBase* listctrl = m_item.GetListCtrlBase();
	if (!listctrl)
		return;

	RECT rc = { 0 };
	m_item.GetParentOrFloatRect(&rc);
	listctrl->ItemRect2WindowRect(&rc, prcOut);
}

void  ListItemLayer::GetParentWindowRect(RECT* prcOut)
{
	ListCtrlBase* listctrl = m_item.GetListCtrlBase();
	if (listctrl)
		listctrl->GetWindowRect(prcOut);
}

bool  ListItemLayer::IsChildOf(ILayerContent* pParentLayer)
{
	if (!pParentLayer)
		return false;

	ListCtrlBase* listctrl = m_item.GetListCtrlBase();
	if (!listctrl)
		return false;

	if (pParentLayer->Type() == LayerContentTypeObject)
	{
		Object& parentObject =
			static_cast<IObjectLayerContent*>(pParentLayer)->GetObject();

		if (listctrl == static_cast<ListCtrlBase*>(&parentObject))
			return true;

		return parentObject.IsMyChild(listctrl, true);
	}
	else if (pParentLayer->Type() == LayerContentTypeListItem)
	{
		return false;
	}
	else
	{
		UIASSERT(0);
	}

	return false;
}

bool  ListItemLayer::IsVisible()
{
	bool b = m_item.IsVisible();
	if (!b)
		return false;

	ListCtrlBase* listctrl = m_item.GetListCtrlBase();
	if (listctrl && !listctrl->IsItemVisibleInScreen(&m_item))
		return false;

	return true;
}


Layer*  ListItemLayer::GetParentLayer()
{
	ListCtrlBase* listctrl = m_item.GetListCtrlBase();
	if (!listctrl)
		return nullptr;

	return listctrl->GetLayer();
}

// 判断思路：这个对象的下一个层，只会在这个对象的下一个Object中出现。
Layer*  ListItemLayer::GetNextLayer()
{
	ListCtrlBase* listctrl = m_item.GetListCtrlBase();
	if (!listctrl)
		return nullptr;

	Layer* pParentLayer = GetParentLayer();

	ListItemBase* pNextTreeItem = m_item.GetNextTreeItem();
	while (pNextTreeItem)
	{
		Layer* pThisLayer = pNextTreeItem->GetSelfLayer();

		// 找回到父layer了，说明自己就是最后一个，没有next
		if (pThisLayer && pThisLayer == pParentLayer)
			return nullptr;

		// 在后面的结点中，找到了一个同级层，插在它的前面 
		if (pThisLayer && pThisLayer->GetParent() == pParentLayer)
			return pThisLayer;

		// 否则继续往下一个树结点遍历
		pNextTreeItem = pNextTreeItem->GetNextTreeItem();
	}

	return nullptr;
}

