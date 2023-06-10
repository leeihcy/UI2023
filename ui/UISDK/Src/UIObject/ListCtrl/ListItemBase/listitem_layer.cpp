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

	// ��layer���뵽����
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

// �ж�˼·������������һ���㣬ֻ��������������һ��Object�г��֡�
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

		// �һص���layer�ˣ�˵���Լ��������һ����û��next
		if (pThisLayer && pThisLayer == pParentLayer)
			return nullptr;

		// �ں���Ľ���У��ҵ���һ��ͬ���㣬��������ǰ�� 
		if (pThisLayer && pThisLayer->GetParent() == pParentLayer)
			return pThisLayer;

		// �����������һ����������
		pNextTreeItem = pNextTreeItem->GetNextTreeItem();
	}

	return nullptr;
}

