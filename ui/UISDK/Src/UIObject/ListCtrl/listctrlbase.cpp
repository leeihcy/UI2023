#include "stdafx.h"
#include "listctrlbase.h"
#include <algorithm>
#include "include/interface/iwindow.h"
#include "include/interface/ilistitembase.h"
#include "src/UIObject\ListCtrl\ListItemRootPanel\listitemrootpanel.h"
#include "include/interface/irenderlayer.h"
#include "src/UIObject\ListCtrl\ListItemBase\listitembase.h"
#include "src/layout/canvaslayout.h"
#include "src/UIObject\ListCtrl\MouseKeyboard\popuplistctrlmkmgr.h"
#include "src/UIObject\ListCtrl\MouseKeyboard\multisellistctrlmkmgr.h"
#include "src/UIObject\ListCtrl\MouseKeyboard\menumkmgr.h"
#include "src/UIObject\ListCtrl\MouseKeyboard\listctrl_inner_drag.h"
#include "src/application/uiapplication.h"
#include "src/Util\Gesture\gesturehelper.h"
#include "src/Util\DPI\dpihelper.h"
#include "src/resource/res_bundle.h"
namespace ui
{

//////////////////////////////////////////////////////////////////////////

ListItemShareData::ListItemShareData(IListItemShareData* p) : Message(p)
{
    m_pIListItemShareData = p;
    m_pListCtrlBase = nullptr;
}
ListItemShareData::~ListItemShareData()
{

}
void  ListItemShareData::SetListCtrlBase(ListCtrlBase* p)
{
    m_pListCtrlBase = p;
}
IListCtrlBase*  ListItemShareData::GetListCtrlBase()
{
    if (m_pListCtrlBase)
        return m_pListCtrlBase->GetIListCtrlBase();
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
    
ListCtrlBase::ListCtrlBase(IListCtrlBase* p) : Control(p)
{
    m_pIListCtrlBase = p;

	m_pFirstItem         = nullptr;
	m_pLastItem          = nullptr;
	m_pFirstDrawItem     = nullptr;
	m_pFirstSelectedItem = nullptr;
	m_pLastDrawItem      = nullptr;
	m_pCompareProc       = nullptr;
    m_pFocusRender       = nullptr;

	m_nItemCount         = 0;
	m_nItemHeight        = 20;
	m_nVertSpacing       = 0;
	m_nHorzSpacing       = 0;
    m_nChildNodeIndent   = 20;
    SetRectEmpty(&m_rItemContentPadding);
	memset(&m_listctrlStyle, 0, sizeof(m_listctrlStyle));

	m_bNeedCalcFirstLastDrawItem = false;
    m_bNeedLayoutItems = false;
    m_bNeedSortItems = false;
    m_bNeedUpdateItemIndex = true;

    // m_bRedrawInvalidItems = false;
    m_bPaintingCtrlRef = 0;

    m_pMKMgr = nullptr;
	m_pEditingItem = nullptr;
	m_pInnerDragMgr = nullptr;
	m_pReusable = nullptr;

    SetClipClient(true);
}

ListCtrlBase::~ListCtrlBase()
{
    SAFE_DELETE(m_pMKMgr);
	SAFE_DELETE(m_pInnerDragMgr);
    SAFE_RELEASE(m_pFocusRender);
}

HRESULT  ListCtrlBase::FinalConstruct(IResource* p)
{
	DO_PARENT_PROCESS(IListCtrlBase, IControl);
    if (FAILED(GetCurMsg()->lRet))
        return (HRESULT) GetCurMsg()->lRet;

    m_MgrLayout.SetListCtrlBase(this);
    m_MgrFloatItem.SetListCtrlBase(this);

	if (nullptr == m_MgrLayout.GetLayout())
    {
		SetLayout(CreateListCtrlLayout(LISTCTRL_ITEM_LAYOUT_TYPE_1, m_pIListCtrlBase));
    }
    if (nullptr == m_pMKMgr)
    {
        m_pMKMgr = new SingleSelListCtrlMKMgr;
        m_pMKMgr->SetListCtrlBase(p->GetImpl()->GetUIApplication(), this);
    }

    m_mgrScrollBar.SetBindObject(static_cast<IObject*>(m_pIListCtrlBase));

	OBJSTYLE s = {0};
	s.vscroll = 1;
	s.hscroll = 1;
	this->ModifyObjectStyle(&s, 0);

	return 0;
}

void  ListCtrlBase::FinalRelease()
{
    LISTCTRLSTYLE s = {0};
    s.destroying = 1;
    ModifyListCtrlStyle(&s, 0);

    m_mgrScrollBar.SetHScrollBar(nullptr); // 滚动条被销毁了（_RemoveAllItem中会调用滚动函数）
    m_mgrScrollBar.SetVScrollBar(nullptr);  

    this->_RemoveAllItem();   // 因为_RemoveAllItem会调用虚函数，因此这个操作没有放在析构函数中执行

    // 销毁共享数据
    map<int, IListItemShareData*>::iterator iter = m_mapItemTypeShareData.begin();
    for (; iter != m_mapItemTypeShareData.end(); iter++)
    {
        iter->second->Release();
    }
    m_mapItemTypeShareData.clear();
    m_mapItem.clear();  

	EnableReusable(false);
    DO_PARENT_PROCESS(IListCtrlBase, IControl);
}

IListCtrlBase*  ListCtrlBase::GetIListCtrlBase()
{
    return m_pIListCtrlBase; 
}

void  ListCtrlBase::OnSerialize(SerializeParam* pData)
{
	DO_PARENT_PROCESS(IListCtrlBase, IControl);

	{
		AttributeSerializer  s(pData, TEXT("ListCtrl"));
		s.AddLong(XML_LISTCTRL_ITEM_HEIGHT, m_nItemHeight)
			->SetDpiScaleType(LONGATTR_DPI_SCALE_ENABLE)
            ->SetDefault(20);
		s.AddRect(XML_LISTCTRL_ITEM_PADDING, m_rItemContentPadding);
	}

	m_mgrScrollBar.Serialize(pData);
	m_mgrScrollBar.SetVScrollLine(m_nItemHeight);
	m_mgrScrollBar.SetHScrollLine(3);

	// text render
// 	if (nullptr == m_pIListCtrlBase->GetTextRender())
// 	{
//         ITextRenderBase* pTextRender = nullptr;
//         pMapAttrib->GetAttr_TextRenderBase(nullptr, XML_TEXTRENDER_TYPE, true, pUIApp, m_pIListCtrlBase, &pTextRender);
//         if (pTextRender)
//         {
//             m_pIListCtrlBase->SetTextRender(pTextRender);
//             SAFE_RELEASE(pTextRender);
//         }
//     }
//     if (nullptr == m_pIListCtrlBase->GetTextRender())
//     {
//         ITextRenderBase* pTextRender = nullptr;
//         pUIApp->CreateTextRenderBase(TEXTRENDER_TYPE_SIMPLE, m_pIListCtrlBase, &pTextRender);
//         if (pTextRender)
//         {
//             SerializeParam data = {0};
//             data.pUIApplication = GetIUIApplication();
//             data.pMapAttrib = pMapAttrib;
//             data.szPrefix = nullptr;
//             data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
//             pTextRender->Serialize(&data);
// 
//             m_pIListCtrlBase->SetTextRender(pTextRender);
//             pTextRender->Release();
//         }
// 	}

    {
        map<int, IListItemShareData*>::iterator iter = m_mapItemTypeShareData.begin();
        for (; iter != m_mapItemTypeShareData.end(); iter++)
        {
// 		    SerializeParam data = {0};
//             data.pUIApplication = GetIUIApplication();
// 		    data.pMapAttrib = pData->pMapAttrib;
// 		    data.nFlags = SERIALIZEFLAG_LOAD;
// 		    if (data.IsReload())
// 			    data.nFlags |= SERIALIZEFLAG_RELOAD;
		    UISendMessage(iter->second, UI_MSG_SERIALIZE, (WPARAM)pData/*data*/);
            //UISendMessage(iter->second, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)bReload);
        }
    }
}


IScrollBarManager*  ListCtrlBase::GetIScrollBarMgr()
{
    return m_mgrScrollBar.GetIScrollBarMgr();
}

void ListCtrlBase::SetLayout(IListCtrlLayout* pLayout)
{
    if (pLayout)
        pLayout->SetIListCtrlBase(m_pIListCtrlBase);
    
    m_MgrLayout.SetLayout(pLayout);
}

// 将布局类型设置为ListCtrlItemLayout1
void  ListCtrlBase::SetLayoutFixedHeight()
{
    this->SetLayout(new ListCtrlItemSimpleLayout());
}
// 可变高度
void  ListCtrlBase::SetLayoutVariableHeight()
{
    this->SetLayout(new ListCtrlItemVariableHeightLayout());
}

IListCtrlItemFixHeightFlowLayout*  ListCtrlBase::SetLayoutFixedHeightFlow()
{
	IListCtrlItemFixHeightFlowLayout* p = 
		new ListCtrlItemFixHeightFlowLayout();

	this->SetLayout(p);
	return p;
}
IListCtrlLayout*  ListCtrlBase::GetLayout()
{
    return m_MgrLayout.GetLayout();
}

void ListCtrlBase::RemoveItem(int nIndex)
{
	ListItemBase* pItem = this->GetItemByPos(nIndex, true);
	if (nullptr == pItem)
		return ;

	this->RemoveItem(pItem);
}

void  ListCtrlBase::DelayRemoveItem(ListItemBase* pItem)
{
    IApplication* pUIApplication = m_pIListCtrlBase->GetUIApplication();
    if (nullptr == pUIApplication)
        return;

	{
		LISTCTRLSTYLE s = {0};
		s.destroying = 1;
		if (TestListCtrlStyle(&s))
			return;
	}
	{
		LISTITEMSTYLE s = {0};
		s.bDelayRemoving = 1;
		if (pItem->TestStyle(s))
			return;

		// 加上标志，防止DelayRemoveItem重入，防止再调用RemoveItem
		pItem->ModifyStyle(&s, nullptr);
	}
	
	// 从树中移除，防止再调用RemoveAll/RemoveAllChild等
	_RemoveItemFromTree(pItem);

    UIMSG  msg;
    msg.message = UI_MSG_NOTIFY;
    msg.nCode = UI_LCN_INNER_DELAY_REMOVE_ITEM;
    msg.wParam = (WPARAM)pItem;
    msg.lParam = (LPARAM)0;
    msg.pMsgFrom = m_pIListCtrlBase;
    msg.pMsgTo = m_pIListCtrlBase;
    UIPostMessage(pUIApplication, &msg);
}

long  ListCtrlBase::OnDelayRemoveItem(WPARAM w, LPARAM l)
{
	ListItemBase* pListItem = (ListItemBase*)w;

	// 移除DelayRemoving样式，RemoveItem中会判断如果有这个
	// 样式将不处理。
	LISTITEMSTYLE s = {0};
	s.bDelayRemoving = 1;
	pListItem->ModifyStyle(0, &s);

    RemoveItem(pListItem);
    return 0;
}


// 在删除一个ITEM时，除了要通知自己被REMOVE，还要通知外面自己的
// 子结点也被REMOVE了，否则外部得到的通知不全，会导致野指针崩溃。
void  ListCtrlBase::_SendItemRemoveNotify(ListItemBase* pItem)
{
	if (!pItem)
		return;

	ListItemBase* pChildItem = pItem->GetChildItem();
	while (pChildItem)
	{
		_SendItemRemoveNotify(pChildItem);
		pChildItem = pChildItem->GetNextItem();
	}

	LONGLONG lId = pItem->GetId();
	UIMSG  msg;
	msg.pMsgTo = m_pIListCtrlBase;
	msg.message = UI_MSG_NOTIFY;
	msg.nCode  = UI_LCN_ITEMREMOVE;
	msg.wParam = (WPARAM)&lId;
	msg.lParam = (LPARAM)pItem->GetIListItemBase();
	UISendMessage(&msg);
}

void ListCtrlBase::RemoveItem(ListItemBase* pItem)
{
	if (nullptr == pItem)
		return;

	// 正在延时删除中
	LISTITEMSTYLE s = {0};
	s.bDelayRemoving = 1;
	if (pItem->TestStyle(s))
		return;

	ListItemBase* pNextItem = pItem->GetNextItem();

	bool bRet = _RemoveItem(pItem, true);
	if (false == bRet)
		return;

	this->UpdateItemIndex(pNextItem);
	
	SetNeedLayoutItems();
    Invalidate();
}

void  ListCtrlBase::RemoveAllChildItems(ListItemBase* pParent)
{
    if (nullptr == pParent)
        return;

    bool bRet = _RemoveAllChildItems(pParent);
    if (false == bRet)
        return;

	SetNeedLayoutItems();
    Invalidate();
}

bool  ListCtrlBase::_RemoveAllChildItems(ListItemBase* pParent)
{
    if (nullptr == pParent)
        return false;

    ListItemBase*  pItem = pParent->GetChildItem();
    if (nullptr == pItem)
        return false;

    ListItemBase*  pFocusItem = GetFocusItem();
    if (pParent->IsMyChildItem(pFocusItem, true))
    {
        SetFocusItem(pParent);
    }

    bool bSelChanged = false;
    while (pItem)
    {
        m_nItemCount--;

        if (m_pMKMgr)
            m_pMKMgr->OnRemoveItem(pItem, &bSelChanged);
        m_MgrFloatItem.OnRemoveItem(pItem);

		_SendItemRemoveNotify(pItem);

        ListItemBase* pNext = pItem->GetNextItem();
        pItem->GetIListItemBase()->Release();;
        pItem = pNext;
    }

    pParent->SetChildItem(nullptr);
    pParent->SetLastChildItem(nullptr);
    this->SetCalcFirstLastDrawItemFlag();

    // 等所有状态数据更新后，再通知。要不然外部在这个通知中可能触发新的刷新操作
    if (bSelChanged)
        FireSelectItemChanged(nullptr);

    return true;
}


//
// item析构了。
// 用于解析多级复杂树控件，在删除任意一级父结点时，需要把所有子结点都做一次清理工作。
//
void  ListCtrlBase::ItemDestructed(ListItemBase* pItem)
{
    if (!pItem)
        return;

    LISTCTRLSTYLE s = {0};
    s.destroying = 1;
    if (TestListCtrlStyle(&s))
        return;

    UINT_PTR lId = pItem->GetId();
    if (lId != 0)
    {
        _mapItemIter iter = m_mapItem.find(lId);
        if (iter != m_mapItem.end())
            m_mapItem.erase(iter);
    }
}


// 未刷新，未更新Item Rect, scroll bar
// bool bReleaseItem是否销毁item。
bool ListCtrlBase::_RemoveItem(ListItemBase* pItem, bool bReleaseItem)
{
	if (nullptr == pItem)
		return false;
	
    {
        UIMSG  msg;
        msg.pMsgTo = m_pIListCtrlBase;
        msg.message = UI_MSG_NOTIFY;
        msg.wParam = (WPARAM)pItem->GetIListItemBase();
        msg.nCode = UI_LCN_PRE_ITEMREMOVE;
        UISendMessage(&msg);
    }

    bool  bSelChanged = false;
    if (m_pMKMgr)
        m_pMKMgr->OnRemoveItem(pItem, &bSelChanged);
    m_MgrFloatItem.OnRemoveItem(pItem);

    _RemoveItemFromTree(pItem);
	_SendItemRemoveNotify(pItem);

	if (bReleaseItem)
	{
		pItem->GetIListItemBase()->Release();;
	}

    // 等所有状态数据更新后，再通知。要不然外部在这个通知中可能触发新的刷新操作
    if (bSelChanged)
        FireSelectItemChanged(nullptr);

	return true;
}
void ListCtrlBase::RemoveAllItem()
{
    bool bHaveSelection = m_pFirstSelectedItem?true:false;
	if (false == _RemoveAllItem())
		return;

    if (bHaveSelection)
        FireSelectItemChanged(nullptr);

    m_bNeedLayoutItems = false;
    m_pIListCtrlBase->Invalidate();
}

bool ListCtrlBase::_RemoveAllItem()
{
	if (nullptr == m_pFirstItem)
		return false;

    {
        UIMSG  msg;
        msg.pMsgTo = m_pIListCtrlBase;
        msg.message = UI_MSG_NOTIFY;
        msg.nCode = UI_LCN_PRE_ALLITEMREMOVE;
        UISendMessage(&msg);
    }

	ListItemBase* p = m_pFirstItem;
	while (p)
	{
		ListItemBase* pNext = p->GetNextItem();  // Save
		p->GetIListItemBase()->Release();
		p = pNext;
	}

	m_pFirstItem = nullptr;
	m_pLastItem = nullptr;
	m_pFirstDrawItem = nullptr;
	m_pFirstSelectedItem = nullptr;
	if (m_pEditingItem)
	{
		m_pEditingItem = nullptr;
	}
	m_nItemCount = 0;
    m_mapItem.clear();
    if (m_pMKMgr)
        m_pMKMgr->OnRemoveAll();
    m_MgrFloatItem.OnRemoveAllItem();

	m_mgrScrollBar.SetScrollRange(0,0);

    Invalidate();

    {
        UIMSG  msg;
        msg.pMsgTo = m_pIListCtrlBase;
        msg.message = UI_MSG_NOTIFY;
        msg.nCode = UI_LCN_ALLITEMREMOVE;
        UISendMessage(&msg);
    }
	return true;
}
void ListCtrlBase::SetSortCompareProc(ListItemCompareProc p)
{
	m_pCompareProc = p;
}

void  ListCtrlBase::Sort()
{
    if (nullptr == m_pCompareProc)
        return;

    m_bNeedSortItems = false;
    SortChildren(nullptr);
}

void  ListCtrlBase::SortChildren(ListItemBase* pParent)
{
    if (nullptr == m_pCompareProc)
        return;

    if (!m_listctrlStyle.sort_ascend && !m_listctrlStyle.sort_descend)
        return;

    if (!pParent)
    {
        sort_by_first_item(m_pFirstItem, GetRootItemCount(), 
			m_listctrlStyle.sort_child);
    }
    else
    {
        sort_by_first_item(pParent->GetChildItem(), pParent->GetChildCount(),
			m_listctrlStyle.sort_child);
    }

    this->SetCalcFirstLastDrawItemFlag();
	SetNeedLayoutItems();
}

// nCount为pFirstItem的邻居数量 
void  ListCtrlBase::sort_by_first_item(ListItemBase* pFirstItem, int nNeighbourCount, bool bSortChildren)
{
    if (!pFirstItem)
        return;

    if (nNeighbourCount <= 1)
    {
        // 直接排子节点
        if (pFirstItem->GetChildItem() && bSortChildren)
        {
            sort_by_first_item(pFirstItem->GetChildItem(), pFirstItem->GetChildCount(), bSortChildren);
        }
        return;
    }

    IListItemBase**  pArray = new IListItemBase*[nNeighbourCount];
    pArray[0] = pFirstItem->GetIListItemBase();

    for (int i = 1; i < nNeighbourCount; i++)
    {
        pArray[i] = pArray[i-1]->GetNextItem();
    }

    std::sort(pArray, (pArray+nNeighbourCount), m_pCompareProc);

    // 根据升降序重新整理列表
    if (m_listctrlStyle.sort_ascend)
    {
        ListItemBase*  pFirstItem = pArray[0]->GetImpl();
        ListItemBase*  pLastItem = pArray[nNeighbourCount-1]->GetImpl();

        ListItemBase*  pParentItem = pFirstItem->GetParentItem();
        if (pParentItem)
        {
            pParentItem->SetChildItem(pFirstItem);
            pParentItem->SetLastChildItem(pLastItem);            
        }
        else
        {
            m_pFirstItem = pFirstItem;
            m_pLastItem = pLastItem;
        }

        pFirstItem->SetPrevItem(nullptr);
        pFirstItem->SetNextItem(pArray[1]->GetImpl());

        pLastItem->SetNextItem(nullptr);
        pLastItem->SetPrevItem(pArray[nNeighbourCount-2]->GetImpl());

        for (int i = 1; i < nNeighbourCount-1; i++)
        {
            pArray[i]->SetNextItem(pArray[i+1]);
            pArray[i]->SetPrevItem(pArray[i-1]);
        }
    }
    else
    {
        ListItemBase*  pLastItem = pArray[0]->GetImpl();
        ListItemBase*  pFirstItem = pArray[nNeighbourCount-1]->GetImpl();

        ListItemBase*  pParentItem = pFirstItem->GetParentItem();
        if (pParentItem)
        {
            pParentItem->SetChildItem(pFirstItem);
            pParentItem->SetLastChildItem(pLastItem);            
        }
        else
        {
            m_pFirstItem = pFirstItem;
            m_pLastItem = pLastItem;
        }

        pFirstItem->SetPrevItem(nullptr);
        pFirstItem->SetNextItem(pArray[nNeighbourCount-2]->GetImpl());

        pLastItem->SetNextItem(nullptr);
        pLastItem->SetPrevItem(pArray[1]->GetImpl());

        for (int i = 1; i < nNeighbourCount-1; i++)
        {
            pArray[i]->SetPrevItem(pArray[i+1]);
            pArray[i]->SetNextItem(pArray[i-1]);
        }
    }

    SAFE_ARRAY_DELETE(pArray);

    if (bSortChildren)
    {
        ListItemBase*  pItem = pFirstItem;
        while (pItem)
        {
            if (pItem->GetChildItem())
                sort_by_first_item(pItem->GetChildItem(), pItem->GetChildCount(), bSortChildren);

            pItem = pItem->GetNextItem();
        }
    }
}

// 交换两个item的位置
void  ListCtrlBase::SwapItemPos(ListItemBase*  p1, ListItemBase* p2)
{
    if (nullptr == p1 || nullptr == p2 || p1 == p2)
        return;

    if (p1->GetNextItem() == p2)
    {
        ListItemBase* p1Prev = p1->GetPrevItem();
        ListItemBase* p2Next = p2->GetNextItem();

        if (p1Prev)
            p1Prev->SetNextItem(p2);
        p2->SetPrevItem(p1Prev);

        if (p2Next)
            p2Next->SetPrevItem(p1);
        p1->SetNextItem(p2Next);

        p2->SetNextItem(p1);
        p1->SetPrevItem(p2);
    }
    else if (p1->GetPrevItem() == p2)
    {
        ListItemBase* p1Next = p1->GetNextItem();
        ListItemBase* p2Prev = p2->GetPrevItem();

        if (p2Prev)
            p2Prev->SetNextItem(p1);
        p1->SetPrevItem(p2Prev);

        if (p1Next)
            p1Next->SetPrevItem(p2);
        p2->SetNextItem(p1Next);

        p1->SetNextItem(p2);
        p2->SetPrevItem(p1);
    }
    else
    {
        ListItemBase* p1Prev = p1->GetPrevItem();
        ListItemBase* p1Next = p1->GetNextItem();

        ListItemBase* p2Prev = p2->GetPrevItem();
        ListItemBase* p2Next = p2->GetNextItem();

        if (p2Next)
            p2Next->SetPrevItem(p1);
        p1->SetNextItem(p2Next);

        if (p2Prev)
            p2Prev->SetNextItem(p1);
        p1->SetPrevItem(p2Prev);

        if (p1Next)
            p1Next->SetPrevItem(p2);
        p2->SetNextItem(p1Next);

        if (p1Prev)
            p1Prev->SetNextItem(p2);
        p2->SetPrevItem(p1Prev);       
    }

    // 重置起始项
    if (nullptr == p1->GetPrevItem())
        m_pFirstItem = p1;
    if (nullptr == p2->GetPrevItem())
        m_pFirstItem = p2;

    if (nullptr == p1->GetNextItem())
        m_pLastItem = p1;
    if (nullptr == p2->GetNextItem())
        m_pLastItem = p2;

    // 更换索引行
    int nIndex1 = p1->GetLineIndex();
    int nIndex2 = p2->GetLineIndex();
    p1->SetLineIndex(nIndex2);
    p2->SetLineIndex(nIndex1);

    // 更新可见
    this->SetCalcFirstLastDrawItemFlag();
	SetNeedLayoutItems();
    Invalidate();
}

int  ListCtrlBase::GetChildNodeIndent()
{
    return m_nChildNodeIndent;
}
void  ListCtrlBase::SetChildNodeIndent(int n)
{
    m_nChildNodeIndent = n;
}

void ListCtrlBase::SetItemHeight(int nHeight, bool bUpdate)
{
	if (m_nItemHeight == nHeight)
		return;

	m_nItemHeight = nHeight;
//	this->MeasureAllItem();

    if (bUpdate)
    	this->LayoutItem(m_pFirstItem, true);
}

ListItemBase*  ListCtrlBase::GetItemByWindowPoint(POINT pt)
{
    if (!m_pMKMgr)
        return nullptr;

    return m_pMKMgr->GetItemByPos(pt);
}
ListItemBase*  ListCtrlBase::GetItemUnderCursor()
{
    POINT pt = {0};
    GetCursorPos(&pt);
    MapWindowPoints(nullptr, m_pIListCtrlBase->GetHWND(), &pt, 1);
    return GetItemByWindowPoint(pt);
}
ListItemBase* ListCtrlBase::GetItemByPos(unsigned int nIndex, bool bVisibleOnly)
{
	if (nIndex < 0)
		return nullptr;

    if (bVisibleOnly)
    {
        ListItemBase* pItem = FindVisibleItemFrom(nullptr);

        unsigned int i = 0;
        while (pItem)
        {
            if (i == nIndex)
                return pItem;

            i++;
            pItem = pItem->GetNextVisibleItem();
        }
    }
    else
    {
        if (nIndex >= m_nItemCount)
            return nullptr;

	    ListItemBase* pItem = m_pFirstItem;

	    unsigned int i = 0;
	    while (pItem)
	    {
		    if (i == nIndex)
			    return pItem;

		    i++;
		    pItem = pItem->GetNextItem();
	    }
    }
    return nullptr;
}

int  ListCtrlBase::GetItemPos(ListItemBase* pFindItem, bool bVisibleOnly)
{
    if (!pFindItem)
        return -1;

    if (pFindItem->GetListCtrlBase() != this)
        return -1;

    if (bVisibleOnly)
    {
        ListItemBase* pItem = FindVisibleItemFrom(nullptr);

        unsigned int i = 0;
        while (pItem)
        {
            if (pFindItem == pItem)
                return i;

            i++;
            pItem = pItem->GetNextVisibleItem();
        }
    }
    else
    {
        ListItemBase* pItem = m_pFirstItem;

        unsigned int i = 0;
        while (pItem)
        {
            if (pFindItem == pItem)
                return i;

            i++;
            pItem = pItem->GetNextItem();
        }
    }
    return 0;
}

ListItemBase*  ListCtrlBase::GetItemById(long lId)
{
    if (0 == lId)
    {
        ListItemBase* pItem = m_pFirstItem;

        while (pItem)
        {
            if (pItem->GetId() == lId)
                return pItem;

            pItem = pItem->GetNextTreeItem();
        }
        return nullptr;
    }
    else
    {
        _mapItemIter iter = m_mapItem.find(lId);
        if (iter == m_mapItem.end())
            return nullptr;

        return iter->second->GetImpl();
    }
}
void  ListCtrlBase::ItemIdChanged(
		IListItemBase* pItem, UINT_PTR lOldId, UINT_PTR lNewId)
{
    if (!pItem)
        return;

    if (lOldId == lNewId)
        return;

    if (0 != lOldId)
    {
        _mapItemIter iter = m_mapItem.find(lOldId);
        if (iter != m_mapItem.end())
            m_mapItem.erase(iter);
    }
    if (0 != lNewId)
    {
        m_mapItem[lNewId] = pItem;
    }
}
ListItemBase* ListCtrlBase::FindItemByText(const wchar_t*  szText, ListItemBase* pStart)
{
    if (nullptr == szText)
        return nullptr;

    ListItemBase* p = pStart;
    if (nullptr == pStart)
        p = m_pFirstItem;

    if (nullptr == p)
        return nullptr;

    while (p)
    {
        if (0 == _tcscmp(p->GetText(), szText))
        {
            return p;
        }
        p = p->GetNextTreeItem();
    }

    return nullptr;
}

ListItemBase*  ListCtrlBase::EnumItemByProc(ListItemEnumProc pProc, ListItemBase* pEnumFrom, WPARAM w, LPARAM l)
{
    if (!pProc)
        return nullptr;

    ListItemBase* p = pEnumFrom;
    if (nullptr == pEnumFrom)
        p = m_pFirstItem;

    if (nullptr == p)
        return nullptr;

    while (p)
    {
        if (!pProc(p->GetIListItemBase(), w, l))
        {
            return p;
        }
        p = p->GetNextTreeItem();
    }

    return nullptr;
}

// 仅在pParent下查找，如果pParent为nullptr,则仅在最顶层查找
ListItemBase*  ListCtrlBase::FindChildItemByText(const wchar_t*  szText, ListItemBase* pParent, ListItemBase* pStart)
{
    if (nullptr == szText)
        return nullptr;

    if (!pStart)
    {
        if (pParent)
            pStart = pParent->GetChildItem();
        else
            pStart = m_pFirstItem;
    }

    if (pStart)
    {
        ListItemBase* p = pStart;
        while (p)
        {
            if (0 == _tcscmp(p->GetText(), szText))
            {
                return p;
            }
            p = p->GetNextItem();
        }
    }
    return nullptr;
}

//
//	在末尾添加一项，根据排序结果，最后调用InsertItem
//
bool ListCtrlBase::AddItem(ListItemBase* pItem)
{
	ListItemBase* pInsertAfter = m_pLastItem;

	bool bAscendSort = m_listctrlStyle.sort_ascend;
	bool bDescendSort = m_listctrlStyle.sort_descend;

	if ((bAscendSort||bDescendSort) && m_pCompareProc)
	{
		// 排序决定位置(由于采用了链接的数据结构，不能采用二分查找的方式...)
		ListItemBase* pEnumItem = m_pFirstItem;
		while (nullptr != pEnumItem)
		{
			int nResult = m_pCompareProc(pEnumItem->GetIListItemBase(), 
				pItem->GetIListItemBase());
			if (bAscendSort)
			{
				// 查找第一个大于自己的对象
				if (nResult<0)
				{
					pInsertAfter = pEnumItem->GetPrevItem();
					break;
				}
			}
			else if(bDescendSort)
			{
				// 查找第一个小于自己的对象 
				if(nResult>0)
				{
					pInsertAfter = pEnumItem->GetPrevItem();
					break;
				}
			}

			pEnumItem = pEnumItem->GetNextItem();
		}
	}

	// 插入
	return this->InsertItem(pItem, pInsertAfter);
}

// 更新每一个ITEM的索引计数
void  ListCtrlBase::UpdateItemIndex(ListItemBase* pStart)
{
#if 0  // 无子对象版本
	if (nullptr == pStart)
		pStart = m_pFirstItem;

	ListItemBase* p = pStart;
	while (p != nullptr)
	{
		if (nullptr == p->GetPrevItem())   // 第一个
			p->SetLineIndex(0);
		else
			p->SetLineIndex(p->GetPrevItem()->GetLineIndex()+1);

		p = p->GetNextItem();
	}

#else // 树结构版本

    int nTreeIndex = -1;
    int nLineIndex = -1;
    ListItemBase* pItem = pStart;
    if (nullptr == pItem)
        pItem = m_pFirstItem;

    if (pItem)
    {
        ListItemBase*  pPrev = pItem->GetPrevTreeItem();
        if (pPrev)
            nTreeIndex = pPrev->GetTreeIndex();

        pPrev = pItem->GetPrevVisibleItem();
        if (pPrev)
            nLineIndex = pPrev->GetLineIndex();
    }

    while (pItem)
    {
        pItem->SetTreeIndex(++nTreeIndex);

        if (pItem->IsVisible())
            pItem->SetLineIndex(++nLineIndex);
        else
            pItem->SetLineIndex(-1);

        if (nullptr == pItem->GetPrevItem())
            pItem->SetNeighbourIndex(0);
        else
            pItem->SetNeighbourIndex((pItem->GetPrevItem())->GetNeighbourIndex()+1);

        pItem = pItem->GetNextTreeItem();
    }

#endif

	m_bNeedUpdateItemIndex = false;
}


void ListCtrlBase::LayoutItem(ListItemBase* pStart, bool bRedraw)
{
	SetCalcFirstLastDrawItemFlag();
    m_bNeedLayoutItems = false;

	if (m_pLayoutParam && false/*m_pLayoutParam->IsSizedByContent()*/)
	{
		ILayout* layout = __super::GetLayout();
		if (layout)
		{
			// REMARK: 有可能当前正在绘制中，触发了UpdateIfNeed，导致控件位置变化。
			// 但此时rendertarget中脏区域还是老的控件范围，为了刷新控件新的位置，重新
			// 触发一次刷新。
			// 新位置可能更小，因此要先记下老位置进行刷新

			Invalidate();

			layout->Arrange(m_pIObject); 

			// REMARK: 有可能当前正在绘制中，触发了UpdateIfNeed，导致控件位置变化。
			// 但此时rendertarget中脏区域还是老的控件范围，为了刷新控件新的位置，重新
			// 触发一次刷新。
			Invalidate();  
			return;
		}
	}
#if 0
    bool bWidthNotConfiged = false;
    bool bHeightNotConfiged = false;
    if (!m_pLayoutParam)
    {
        bWidthNotConfiged = true;
        bHeightNotConfiged = true;
    }
    else if (m_pLayoutParam && m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
    {
        CanvasLayoutParam* pParam = static_cast<CanvasLayoutParam*>(m_pLayoutParam);
        bWidthNotConfiged = ((pParam->GetConfigLeft() == NDEF || pParam->GetConfigRight() == NDEF) && pParam->GetConfigWidth()==AUTO);
        bHeightNotConfiged = ((pParam->GetConfigTop() == NDEF || pParam->GetConfigBottom() == NDEF) && pParam->GetConfigHeight() == AUTO);
    }

	if (bWidthNotConfiged || bHeightNotConfiged)
	{
		m_pIListCtrlBase->UpdateLayout(bRedraw);
	}
	else

#endif
	{
		SIZE sizeContent = {0,0};
        IListItemBase*  p = nullptr;
        if (pStart)
            p = pStart->GetIListItemBase();

		arrange_item(p?p->GetImpl():nullptr, &sizeContent);
		
		m_mgrScrollBar.SetScrollRange(sizeContent.cx, sizeContent.cy);

        // 更新hoveritem，排序、arrage后该item可能不再是位于鼠标下面
        if (GetHoverItem())
        {
            POINT pt = {0, 0};
            ::GetCursorPos(&pt);
            ::MapWindowPoints(nullptr, m_pIListCtrlBase->GetHWND(), &pt, 1);
            UISendMessage(m_pIListCtrlBase,
                WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
        }

		if (bRedraw)
			Invalidate();
	}
}

void ListCtrlBase::OnSize(unsigned int nType, int cx, int cy)
{
    SetMsgHandled(FALSE);

	this->SetCalcFirstLastDrawItemFlag();

	SIZE sizeContent = {0,0};
	arrange_item(nullptr, &sizeContent);

	// 更新滚动条的属性
	CRect rcClient;
	m_pIListCtrlBase->GetClientRectInObject(&rcClient);

    SIZE sizePage = { rcClient.Width(), rcClient.Height() };
    m_mgrScrollBar.SetScrollPageAndRange(&sizePage, &sizeContent);
}

void  ListCtrlBase::arrange_item(ListItemBase* pFrom, __out SIZE* pContent)
{
	m_MgrLayout.Arrange(nullptr, pContent);

	// TODO:
// 	if (m_pEditingItem)
// 	{
// 		CRect  rcOleEditingItem(0,0,0,0);
// 		m_pEditingItem->GetParentRect(&rcOleEditingItem);
// 
// 		m_MgrLayout.Arrange(p, &sizeContent);
// 		long lVisiblePos = 0;
// 		IsItemVisibleInScreenEx(m_pEditingItem, lVisiblePos);
// 		if (lVisiblePos != LISTITEM_VISIBLE)
// 		{
// 			// 不是完全可见，直接停止 
// 			this->DiscardEdit();
// 		}
// 		else
// 		{
// 			// 更新编辑框位置
// 			long lRet = UISendMessage(m_pEditingItem->GetIListItemBase(), 
// 				UI_MSG_NOTIFY, w, l, UI_WEN_UPDATEEDITPOS);
// 		}
// 	}
// 	else
// 	{
// 		m_MgrLayout.Arrange(p, &sizeContent);
// 	}
}

void ListCtrlBase::update_mouse_mgr_type()
{
    SAFE_DELETE(m_pMKMgr);

    if (m_listctrlStyle.multiple_sel)
    {
        m_pMKMgr = new MultiSelListCtrlMKMgr;
    }
    else if (m_listctrlStyle.popuplistbox)
    {
        m_pMKMgr = new PopupListCtrlMKMgr;
    }
    else if (m_listctrlStyle.menu)
    {
        m_pMKMgr = new MenuMKMgr;
    }
    else
    {
        m_pMKMgr = new SingleSelListCtrlMKMgr;
    }
    m_pMKMgr->SetListCtrlBase(m_pIListCtrlBase->GetUIApplication()->GetImpl(), this);
}

#pragma  region  // selection

bool ListCtrlBase::IsSelected(ListItemBase* pItem)
{
	if (nullptr == pItem)
		return false;

// 	if (m_pFirstSelectedItem == pItem || nullptr != pItem->GetPrevSelection())
// 		return true;

    return pItem->IsSelected();
}

// 设置一个选择项(只选中一个)
void ListCtrlBase::SelectItem(ListItemBase* pItem, bool bNotify, bool bMakeVisible)
{
	if (nullptr == pItem)  
		return;

    if (!pItem->IsSelectable())
        return;

    // 定位到该ITEM，确保完全可见
	if (m_pFirstSelectedItem == pItem && nullptr == m_pFirstSelectedItem->GetNextSelection())
	{
        if (bMakeVisible)
        {
            bool bNeedUpdate = false;
		    this->MakeItemVisible(m_pFirstSelectedItem, &bNeedUpdate);
            if (bNeedUpdate)
			    this->Invalidate();
        }

        if (GetFocusItem() != pItem)
            SetFocusItem(pItem);

		return;
	}

	ListItemBase* pOldSelectoinItem = m_pFirstSelectedItem;
	ClearSelectItem(false);
	m_pFirstSelectedItem = pItem;
    if (pItem)
        pItem->SetSelected(true);

    if (bMakeVisible)
    {
        bool bNeedUpdate = false;
        this->MakeItemVisible(m_pFirstSelectedItem, &bNeedUpdate);
        if (bNeedUpdate)
            this->Invalidate();
    }

    this->SetFocusItem(pItem);

	if (m_pFirstSelectedItem != pOldSelectoinItem)
	{
		this->InvalidateItem(m_pFirstSelectedItem);
		this->InvalidateItem(pOldSelectoinItem);

        if (bNotify)
        {
            FireSelectItemChanged(pOldSelectoinItem);
        }
	}
}

// 对于多选样式，不再针对每一个ITEM发送一次选中消息，而是只发消息，不发状态，
// 由外部自己去判断所需要的状态
void  ListCtrlBase::FireSelectItemChanged(ListItemBase* pOldSelectoinItem)
{
    // 通知ctrl
	UIMSG  msg;
	msg.message = UI_MSG_NOTIFY;
	msg.nCode = UI_LCN_SELCHANGED_SELF;

	if (!m_listctrlStyle.multiple_sel)
	{
        msg.wParam = (WPARAM)(pOldSelectoinItem?pOldSelectoinItem->GetIListItemBase():nullptr);
        msg.lParam = (LPARAM)(m_pFirstSelectedItem?m_pFirstSelectedItem->GetIListItemBase():nullptr);
	}
	msg.pMsgFrom = m_pIListCtrlBase;

    // 先交给子类处理
    msg.pMsgTo = m_pIListCtrlBase;
    UISendMessage(&msg, 0, 0);

    // 再通知外部处理 
//  msg.pMsgTo = nullptr;
//  msg.bHandled = FALSE;
// 	msg.nCode = UI_LCN_SELCHANGED;
// 	m_pIListCtrlBase->DoNotify(&msg);
	select_changed.emit(m_pIListCtrlBase);
}

ListItemBase* ListCtrlBase::GetLastSelectItem()
{
	ListItemBase* pItem = m_pFirstSelectedItem;
	while (pItem)
	{
		ListItemBase* pNext = pItem->GetNextSelection();
		if (nullptr == pNext)
			break;
		
		pItem = pNext;
	}

	return pItem;
}

// 如果当前没有选择项，只设置为选择项。如果已经有选择项，则增加为下一个选择项
void ListCtrlBase::AddSelectItem(ListItemBase* pItem, bool bNotify)
{
	if (nullptr == pItem)
		return;

    if (!pItem->IsSelectable())
        return;

	if (m_listctrlStyle.multiple_sel)
	{
		if (pItem->IsSelected())
			return;

		ListItemBase* pLastSelItem = this->GetLastSelectItem();
        if (pLastSelItem)
        {
            pLastSelItem->SetNextSelection(pItem);
            pItem->SetPrevSelection(pLastSelItem);
        }
        else
        {
            m_pFirstSelectedItem = pItem;
        }

        pItem->SetSelected(true);

        if (nullptr == GetFocusItem())
            this->SetFocusItem(pItem);

        this->InvalidateItem(pItem);

        if (bNotify)
        {
            this->FireSelectItemChanged(nullptr);
        }
	}
	else   // 单选
	{
		if (m_pFirstSelectedItem == pItem)
			return;

		SelectItem(pItem, false, bNotify);
	}	
}
void ListCtrlBase::RemoveSelectItem(ListItemBase* pItem, bool bNotify)
{
	if (nullptr == pItem || nullptr == m_pFirstSelectedItem)
		return;

	if (!pItem->IsSelected())
		return;

	ListItemBase* pOldSelection = m_pFirstSelectedItem;
	if (pItem->GetPrevSelection())
		pItem->GetPrevSelection()->SetNextSelection(pItem->GetNextSelection());
	if (pItem->GetNextSelection())
		pItem->GetNextSelection()->SetPrevSelection(pItem->GetPrevSelection());

    if (m_pFirstSelectedItem == pItem)
        m_pFirstSelectedItem = pItem->GetNextSelection();

	pItem->SetPrevSelection(nullptr);
	pItem->SetNextSelection(nullptr);

    pItem->SetSelected(false);
	if (bNotify)
		this->FireSelectItemChanged(pOldSelection);
}
void ListCtrlBase::ClearSelectItem(bool bNotify)
{
	if (nullptr == m_pFirstSelectedItem)
		return;

	ListItemBase* pOldSelection = m_pFirstSelectedItem;
	
	ListItemBase* pItem = m_pFirstSelectedItem;
	while (pItem)
	{
		this->InvalidateItem(pItem);
        pItem->SetSelected(false);

		ListItemBase* pNextItem = pItem->GetNextSelection();
		if (nullptr == pNextItem)
			break;

		pNextItem->SetPrevSelection(nullptr);
		pItem->SetNextSelection(nullptr);
		pItem = pNextItem;
	}
	m_pFirstSelectedItem = nullptr;

	if (bNotify)
	{
		this->FireSelectItemChanged(pOldSelection);
	}
}
unsigned int  ListCtrlBase::GetSelectedItemCount()
{
    unsigned int  nCount = 0;
    ListItemBase* p = m_pFirstSelectedItem;
    while (p)
    {
        nCount ++;
        p = p->GetNextSelection();
    }
    return nCount;
}

// 是否选取了多个（大于1个）
bool  ListCtrlBase::IsSelectMulti()
{
    if (!m_pFirstSelectedItem)
        return false;
    if (m_pFirstSelectedItem->GetNextSelection())
        return true;

    return false;
}

void  ListCtrlBase::SelectAll(bool bUpdate)
{
    ClearSelectItem(false);
    ListItemBase*  p = FindSelectableItemFrom(nullptr);
    m_pFirstSelectedItem = p;

    ListItemBase*  pNext = nullptr;
    while (p)
    {
        p->SetSelected(true);

        pNext = p->GetNextSelectableItem();
        p->SetNextSelection(pNext);
        if (pNext)
            pNext->SetPrevSelection(p);

        p = pNext;
    }

    this->FireSelectItemChanged(nullptr);

    Invalidate();
}
#pragma endregion

ListItemBase* ListCtrlBase::GetHoverItem()
{
    if (nullptr == m_pMKMgr)
        return nullptr;

    return m_pMKMgr->GetHoverItem();
}
ListItemBase* ListCtrlBase::GetPressItem()
{
    if (nullptr == m_pMKMgr)
        return nullptr;

    return m_pMKMgr->GetPressItem();
}

ListItemBase* ListCtrlBase::GetFocusItem()
{
    if (nullptr == m_pMKMgr)
        return nullptr;

    return m_pMKMgr->GetFocusItem();
}
void  ListCtrlBase::SetFocusItem(ListItemBase* pItem)
{
    m_pMKMgr->SetFocusItem(pItem);
    return;
// 
// 	UIASSERT(0 && _T("focus已放到mkmgr中实现 "));
// 	if (m_pFocusItem == pItem)
// 		return;
// 
// 	if (m_pFocusItem)
// 		this->InvalidateItem(m_pFocusItem);
// 
//     if (m_pFocusItem)
//         m_pFocusItem->SetFocus(false);
// 
// 	m_pFocusItem = pItem;
// 
//     if (m_pFocusItem)
//         m_pFocusItem->SetFocus(true);
// 
// 	if (m_pFocusItem)
// 		this->InvalidateItem(m_pFocusItem);
}

Object*  ListCtrlBase::GetHoverObject()
{
    return m_pMKMgr->GetHoverObject();
}
Object*  ListCtrlBase::GetPressObject()
{
    return m_pMKMgr->GetPressObject();
}
void  ListCtrlBase::SetFocusObject(Object* pObj)
{
    m_pMKMgr->SetFocusObject(pObj);
}
Object*  ListCtrlBase::GetFocusObject()
{
    return m_pMKMgr->GetFocusObject();
}
SIZE ListCtrlBase::GetAdaptWidthHeight(int nWidth, int nHeight)
{
	SIZE s = {nWidth,nHeight};

	if (m_lMinWidth != NDEF && nWidth < m_lMinWidth)
		s.cx = m_lMinWidth;
	if (m_nMaxWidth != NDEF && nWidth > m_nMaxWidth)
		s.cx = m_nMaxWidth;

	if (m_lMinHeight != NDEF && nHeight < m_lMinHeight)
		s.cy = m_lMinHeight;
	if (m_nMaxHeight != NDEF && nHeight > m_nMaxHeight)
		s.cy = m_nMaxHeight;

	return s;
}

bool ListCtrlBase::Scroll2Y(int nY, bool bUpdate)
{
	return this->SetScrollPos(-1, nY, bUpdate);
}
bool ListCtrlBase::Scroll2X(int nX, bool bUpdate)
{
	return this->SetScrollPos(nX, -1, bUpdate);
}
bool  ListCtrlBase::ScrollY(int nY, bool bUpdate)
{
    return this->SetScrollPos(-1, m_mgrScrollBar.GetVScrollPos() + nY, bUpdate);
}
bool  ListCtrlBase::ScrollX(int nX, bool bUpdate)
{
    return this->SetScrollPos(m_mgrScrollBar.GetHScrollPos() + nX, -1, bUpdate);
}
bool ListCtrlBase::SetScrollPos(int nX, int nY, bool bUpdate)
{
	bool bScrollChanged = false;
	if (-1 != nX)
	{
		if (m_mgrScrollBar.SetHScrollPos(nX))
			bScrollChanged = true;
	}
	if (-1 != nY)
	{
		if (m_mgrScrollBar.SetVScrollPos(nY))
			bScrollChanged = true;
	}

	if (bScrollChanged)
	{
		SetCalcFirstLastDrawItemFlag();
		if (bUpdate)
			m_pIListCtrlBase->Invalidate();
	}

	return bScrollChanged;
}

void  ListCtrlBase::GetScrollPos(int* pnX, int* pnY)
{
	m_mgrScrollBar.GetScrollPos(pnX, pnY);
}

bool ListCtrlBase::InsertItem(ListItemBase* pItem, unsigned int nPos)
{
	ListItemBase* pInsertAfter = nullptr;
	if (nPos >= m_nItemCount)
		pInsertAfter = m_pLastItem;
	else
		pInsertAfter = this->GetItemByPos(nPos, true);

	return this->InsertItem(pItem, pInsertAfter);
}
//
// 在pInsertAfter前面插入pItem，如果pInsertAfter为nullptr表示插入最前面
//
bool ListCtrlBase::InsertItem(ListItemBase*  pItem, ListItemBase* pInsertAfter)
{
	if (nullptr == pItem)
		return false;
	
	if (false == this->_InsertItem(pItem, pInsertAfter))
		return false;

    pItem->SetIListCtrlBase(m_pIListCtrlBase);
    UISendMessage(pItem->GetIListItemBase(), UI_MSG_INITIALIZE);


	SetNeedLayoutItems();
    Invalidate();

    UIMSG  msg;
    msg.pMsgTo = m_pIListCtrlBase;
    msg.message = UI_MSG_NOTIFY;
    msg.nCode = UI_LCN_ITEMADD;
    msg.wParam = (WPARAM)pItem->GetIListItemBase();
    UISendMessage(&msg);

	return true;
}


// 由InsertItem调用。仅做数据插入的工作，不处理SIZE
bool ListCtrlBase::_InsertItem(ListItemBase* pItem, ListItemBase* pInsertAfter)
{
	if (nullptr == pItem)
		return false;

    // TODO: 参数处理 IListItemBase。这里的参数是ListItemBase，不用处理。
//     if (pInsertAfter == UITVI_FIRST || pInsertAfter == UITVI_ROOT)
//     {
//         pInsertAfter = nullptr;
//     }
//     else if (pInsertAfter == UITVI_LAST)
//     {
//         pInsertAfter = m_pLastItem;
//     }

	if (nullptr == pInsertAfter)// 插在首位
	{
		if (nullptr == m_pFirstItem)
		{
			m_pFirstItem = m_pLastItem = pItem;
		}
		else
		{
			m_pFirstItem->SetPrevItem(pItem);
			pItem->SetNextItem(m_pFirstItem);
			m_pFirstItem = pItem;
		}
	}
	else
	{
		if (nullptr == pInsertAfter->GetNextItem())
		{
			pInsertAfter->SetNextItem(pItem);
			pItem->SetPrevItem(pInsertAfter);
			m_pLastItem = pItem;
		}
		else
		{
			pInsertAfter->GetNextItem()->SetPrevItem(pItem);
			pItem->SetNextItem(pInsertAfter->GetNextItem());
			pInsertAfter->SetNextItem(pItem);
			pItem->SetPrevItem(pInsertAfter);
		}
	}
    
    pItem->SetIListCtrlBase(m_pIListCtrlBase);

	SetCalcFirstLastDrawItemFlag();

    // 如果是批量插入，将子结点的id也缓存起来
    ListItemBase* pEnd = pItem->GetNextItem();
    ListItemBase* p = pItem;
    do 
    {
        if (p->GetId() != 0)
        {
            m_mapItem[p->GetId()] = p->GetIListItemBase();
        }
        p = p->GetNextTreeItem();

        m_nItemCount++; 
    } 
    while (p != pEnd);

	return true;
}

void  ListCtrlBase::GetDesiredSize(SIZE* pSize) 
{
    m_MgrLayout.Measure(pSize);
}


#if 0 // -- 架构改造  -- 废弃
// 获取所有item中，最大的宽度值(如果pDesiredHeight不为空，则同时返回总共需要的高度)
int ListCtrlBase::GetMaxDesiredWidth(int* pDesiredHeight)
{
	if (pDesiredHeight)
		*pDesiredHeight = 0;

	int nRet = 0;
	ListItemBase* p = m_pFirstItem;
	while (p != nullptr)
	{
		SIZE s = GetItemDesiredSize(p);
		if (s.cx > nRet)
			nRet = s.cx;

		if (pDesiredHeight)
		{
			*pDesiredHeight += s.cy;
			if (p != m_pFirstItem)
			{
				*pDesiredHeight += m_nVertSpacing;
			}
		}
		p = p->GetNextVisibleItem();
	}

	return nRet;
}
#endif

void  ListCtrlBase::SetCalcFirstLastDrawItemFlag()
{
    m_bNeedCalcFirstLastDrawItem = true; 
    m_pFirstDrawItem = m_pLastDrawItem = nullptr;

    if (m_pMKMgr)
        m_pMKMgr->SetMouseNotReady();
}

// 计算当前的第一个可视ITEM
void  ListCtrlBase::CalcFirstLastDrawItem()
{
	m_pFirstDrawItem = nullptr;
	m_pLastDrawItem = nullptr;

	CRect  rcClient;
	m_pIListCtrlBase->GetObjectClientRect(&rcClient);

	int xOffset = 0, yOffset = 0;
    m_mgrScrollBar.GetScrollPos(&xOffset, &yOffset);

    LISTITEMSTYLE sAnimatingTest = {0};
    sAnimatingTest.bMoveAnimating = 1;

    CRect  rcParent;

    ListItemBase* pItem = FindVisibleItemFrom(m_pFirstItem);
    while (pItem)
    {
        bool bAnimating = pItem->TestStyle(sAnimatingTest);

        // 只要是在做动画，就认为这个item是可见的。
        if (bAnimating)
        {
            if (nullptr == m_pFirstDrawItem)
                m_pFirstDrawItem = pItem;
        }
        else
        {
            pItem->GetParentRect(&rcParent);
            if (rcParent.bottom - yOffset <= rcClient.top)  // top cover unvisible item
            {
                pItem = pItem->GetNextVisibleItem();
                continue;
            }
            if (rcParent.right - xOffset <= rcClient.left)
            {
                pItem = pItem->GetNextVisibleItem();
                continue;
            }

            if (nullptr == m_pFirstDrawItem)
                m_pFirstDrawItem = pItem;

            if (rcParent.top - yOffset >= rcClient.bottom)  // last visible item
                break;
            if (rcParent.left - xOffset >= rcClient.right)
                break;
        }

        m_pLastDrawItem = pItem;
        pItem = pItem->GetNextVisibleItem();
    }

    if (m_listctrlStyle.float_group_head)
        UISendMessage(m_pIListCtrlBase, UI_LCN_FIRSTLASTDRAWITEM_UPDATED);
}

ListItemBase*  ListCtrlBase::GetFirstDrawItem()
{
	return m_pFirstDrawItem;   // 有可能这个时候m_nNeedCalcFirstLastVisibleItem为true，但仍然返回m_pFirstVisibleItem，因为可能还没有调用UpdateRectItem
}
ListItemBase*  ListCtrlBase::GetLastDrawItem()
{
	return m_pLastDrawItem;
}

ListItemBase*  ListCtrlBase::GetFirstItem()
{ 
    if (m_bNeedSortItems)
    {
        Sort();
    }
    return m_pFirstItem;
}
ListItemBase*  ListCtrlBase::GetLastItem()
{ 
    if (m_bNeedSortItems)
    {
        Sort();
    }
    return m_pLastItem; 
}


ListItemBase*  ListCtrlBase::GetEditingItem()
{
	return m_pEditingItem;
}
void  ListCtrlBase::SetEditingItem(ListItemBase* p)
{
	m_pEditingItem = p;
}

ListItemBase*  ListCtrlBase::FindVisibleItemFrom(ListItemBase* pFindFrom)
{
    if (m_bNeedSortItems)
    {
        Sort();
    }

    if (nullptr == pFindFrom)
        pFindFrom = m_pFirstItem;

    while (pFindFrom)
    {
        if (pFindFrom->IsVisible())
            return pFindFrom;

        pFindFrom = pFindFrom->GetNextVisibleItem();
    }

    return nullptr;
}
ListItemBase*  ListCtrlBase::FindSelectableItemFrom(ListItemBase* pFindFrom)
{
    if (m_bNeedSortItems)
    {
        Sort();
    }

    if (nullptr == pFindFrom)
        pFindFrom = m_pFirstItem;

    while (pFindFrom)
    {
        if (pFindFrom->IsSelectable())
            return pFindFrom;

        pFindFrom = pFindFrom->GetNextSelectableItem();
    }

    return nullptr;
}

ListItemBase*  ListCtrlBase::FindFocusableItemFrom(ListItemBase* pFindFrom)
{
    if (m_bNeedSortItems)
    {
        Sort();
    }

    if (nullptr == pFindFrom)
        pFindFrom = m_pFirstItem;

    while (pFindFrom)
    {
        if (pFindFrom->CanFocus())
            return pFindFrom;

        pFindFrom = pFindFrom->GetNextFocusableItem();
    }

    return nullptr;
}

bool  ListCtrlBase::IsItemRectVisibleInScreen(LPCRECT prc)
{
    CRect rcClient;
    m_pIListCtrlBase->GetClientRectInObject(&rcClient);

    int xOffset = 0, yOffset = 0;
    m_mgrScrollBar.GetScrollPos(&xOffset, &yOffset);

    if (prc->bottom - yOffset <= 0) 
        return false;

    if (prc->top - yOffset >= rcClient.Height())  
        return false;

    if (prc->left - xOffset >= rcClient.Width())
        return false;

    if (prc->right - xOffset <= 0)
        return false;

    return true;
}

bool ListCtrlBase::IsItemVisibleInScreen(ListItemBase* pItem)
{
    if (nullptr == pItem)
        return false;

    if (false == pItem->IsVisible())
        return false;

    UpdateListIfNeed();

    CRect rcParent;
    pItem->GetParentRect(&rcParent);
    return IsItemRectVisibleInScreen(&rcParent);
}

bool ListCtrlBase::IsItemVisibleInScreenEx(
    ListItemBase* pItem,
    /*LISTITEM_VISIBLE_POS_TYPE*/long& ePos)
{
    ePos = LISTITEM_UNVISIBLE_ERROR;

	if (nullptr == pItem)
		return false;

	if (false == pItem->IsVisible())
		return false;

    ePos = LISTITEM_VISIBLE;

    UpdateListIfNeed();

	CRect rcClient, rcItemParent;
	m_pIListCtrlBase->GetClientRectInObject(&rcClient);
	pItem->GetParentRect(&rcItemParent);

	int xOffset = 0, yOffset = 0;
	m_mgrScrollBar.GetScrollPos(&xOffset, &yOffset);

	int yTop = rcItemParent.top - yOffset;
	int yBottom = rcItemParent.bottom - yOffset;
    int xLeft = rcItemParent.left - xOffset;
    int xRight = rcItemParent.right - xOffset;

	if (yBottom <= 0) 
	{
		ePos = LISTITEM_UNVISIBLE_TOP;
		return false;
	}
	else if (yTop >= rcClient.Height())
	{
		ePos = LISTITEM_UNVISIBLE_BOTTOM;
		return false;
	}
    else if (xRight <= 0)
    {
        ePos = LISTITEM_UNVISIBLE_LEFT;
        return false;
    }
    else if (xLeft >= rcClient.Width())
    {
        ePos = LISTITEM_UNVISIBLE_RIGHT;
        return false;
    }

    if (yTop < 0)
    {
        ePos |= LISTITEM_VISIBLE_COVERTOP;
    }
    if (yBottom > rcClient.Height())
    {
        ePos |= LISTITEM_VISIBLE_COVERBOTTOM;
    }
    if (xLeft < 0)
    {
        ePos |= LISTITEM_VISIBLE_COVERLEFT;
    }
    if (xRight > rcClient.Width())
    {
        ePos |= LISTITEM_VISIBLE_COVERRIGHT;
    }

	return true;
}

// 设置滚动条的位置，确保pItem可见
// 返回控件是否滚动了（是否需要刷新）
void  ListCtrlBase::MakeItemVisible(ListItemBase* pItem, bool* pbNeedUpdate)
{
	bool  bNeedUpdateObject = false;

	if (nullptr == pItem)
		return ;

    if (pItem->GetParentItem())
    {
        ExpandItem(pItem->GetParentItem(), false);
    }

	/*LISTITEM_VISIBLE_POS_TYPE*/long ePosType = LISTITEM_VISIBLE;
	this->IsItemVisibleInScreenEx(pItem, ePosType);
    if (ePosType == LISTITEM_VISIBLE ||
        ePosType == LISTITEM_UNVISIBLE_ERROR)
        return;

    CRect rc;
    CRect rcClient;
    pItem->GetParentRect(&rc);
    m_pIListCtrlBase->GetClientRectInObject(&rcClient);

	if (LISTITEM_UNVISIBLE_TOP & ePosType || LISTITEM_VISIBLE_COVERTOP & ePosType)
	{
		bNeedUpdateObject = true;
		m_mgrScrollBar.SetVScrollPos(rc.top);
	}
	else if (LISTITEM_UNVISIBLE_BOTTOM & ePosType || LISTITEM_VISIBLE_COVERBOTTOM & ePosType)
	{
		bNeedUpdateObject = true;
		m_mgrScrollBar.SetVScrollPos(rc.bottom - rcClient.Height());
	}

    if (ePosType & LISTITEM_UNVISIBLE_LEFT || ePosType & LISTITEM_VISIBLE_COVERLEFT)
    {
        // 考虑一行显示不下一列的情况
        int nLimitScrollPos = rc.right - rcClient.Width();
        int nDesiredScrollPos = rc.left;
        if (nDesiredScrollPos > nLimitScrollPos)
        {
            bNeedUpdateObject = true;
            m_mgrScrollBar.SetHScrollPos(nDesiredScrollPos);
        }
    }
    else if (ePosType & LISTITEM_UNVISIBLE_RIGHT || ePosType & LISTITEM_VISIBLE_COVERRIGHT)
    {
        // 考虑一行显示不下一列的情况
        int nLimitScrollPos = rc.left;
        int nDesiredScrollPos = rc.right - rcClient.Width();
        if (nDesiredScrollPos < nLimitScrollPos)
        {
            bNeedUpdateObject = true;
            m_mgrScrollBar.SetHScrollPos(nDesiredScrollPos);
        }
    }

    if (bNeedUpdateObject)
        this->SetCalcFirstLastDrawItemFlag();

    if (pbNeedUpdate)
        *pbNeedUpdate = bNeedUpdateObject;
}


void  ListCtrlBase::ObjectRect2WindowRect(RECT* prcObj, RECT* prcWindow)
{
    if (!prcObj || !prcWindow)
        return;

    RECT  rcWnd = *prcObj;

    RECT rcCtrl;
    GetClientRectInWindow(&rcCtrl);
    OffsetRect(&rcWnd, rcCtrl.left, rcCtrl.top);

    CopyRect(prcWindow, &rcWnd);
}

void  ListCtrlBase::ObjectRect2ItemRect(RECT* prcObject, RECT* prcItem)
{
	if (!prcItem || !prcObject)
		return;

	RECT  rcItem = *prcObject;

	int xOffset = 0, yOffset = 0;
	m_mgrScrollBar.GetScrollPos(&xOffset, &yOffset);
	OffsetRect(&rcItem, xOffset, yOffset);

	RECT rcClient;
	m_pIListCtrlBase->GetClientRectInObject(&rcClient);
	OffsetRect(&rcItem, -rcClient.left, -rcClient.top);

	CopyRect(prcItem, &rcItem);
}

void  ListCtrlBase::ItemRect2ObjectRect(RECT* prcItem, RECT* prcObject)
{
    if (!prcItem || !prcObject)
        return;

    RECT  rcObj = *prcItem;

    int xOffset = 0, yOffset = 0;
    m_mgrScrollBar.GetScrollPos(&xOffset, &yOffset);
    OffsetRect(&rcObj, -xOffset, -yOffset);

    RECT rcClient;
    m_pIListCtrlBase->GetClientRectInObject(&rcClient);
    OffsetRect(&rcObj, rcClient.left, rcClient.top);

    CopyRect(prcObject, &rcObj);
}

void ListCtrlBase::ItemRect2WindowRect(LPCRECT prc, LPRECT prcRet)
{
	if (nullptr == prc || nullptr == prcRet)
		return;

	CRect rcItem(*prc);

	int xOffset = 0, yOffset = 0;
	m_mgrScrollBar.GetScrollPos(&xOffset, &yOffset);
	rcItem.OffsetRect(-xOffset, -yOffset);

	CRect rcWindow;
	GetClientRectInWindow(&rcWindow);
	rcItem.OffsetRect(rcWindow.left, rcWindow.top);
	CopyRect(prcRet, &rcItem);

	// 调用GetVisibleRectInWindow可获取到剪裁区
	// IntersectRect(prcRet, &rcItem, &rcWindow);
}

void ListCtrlBase::WindowPoint2ItemPoint(ListItemBase* pItem, const POINT* ptWnd, POINT* ptItem)
{
    if (nullptr == pItem || nullptr == ptWnd || nullptr == ptItem)
        return;

    if (pItem->IsFloat())
    {
        this->WindowPoint2ObjectPoint(ptWnd, ptItem, true);

        RECT  rc;
        pItem->GetFloatRect(&rc);
        ptItem->x -= rc.left;
        ptItem->y -= rc.top;
    }
    else
    {
        CRect rcItem;
        pItem->GetParentRect(&rcItem);

        WindowPoint2ObjectClientPoint(ptWnd, ptItem, true);
        ptItem->x -= rcItem.left;
        ptItem->y -= rcItem.top;
    }
}

void ListCtrlBase::OnKeyDown(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags)
{
    // 向外部发出notify，如果外部处理，则不再交由mousekeymgr处理
// 	UIMSG  msg;
// 	msg.message = UI_MSG_NOTIFY;
// 	msg.nCode = UI_NM_KEYDOWN;
// 	msg.wParam = nChar;
// 	msg.pMsgFrom = m_pIListCtrlBase;
// 
//     long lRet = m_pIListCtrlBase->DoNotify(&msg);
// 	if (0 == lRet)


    bool bHandled = false;
	keydown.emit(m_pIListCtrlBase, nChar, bHandled);
    if (!bHandled)
	{
		SetMsgHandled(FALSE);
	}
}

void  ListCtrlBase::OnLButtonDown(unsigned int nFlags, POINT point)
{
    if (m_listctrlStyle.dragwnd_if_clickblank &&
        !GetHoverItem() && !GetPressItem())
    {
        // 这种情况下面直接拖动窗口
        IWindowBase* pWindowBase = m_pIListCtrlBase->GetWindowObject();
        if (pWindowBase)
        {
            pWindowBase->SetPressObject(nullptr);
            ReleaseCapture();

            ::SendMessage(pWindowBase->GetHWND(), WM_NCLBUTTONDOWN, HTCAPTION, GetMessagePos());
            return;
        }
    }

    SetMsgHandled(FALSE);
}

void ListCtrlBase::OnStateChanged(unsigned int nMask)
{
    SetMsgHandled(FALSE);
	if (nMask & OSB_FOCUS)
	{
		m_pIListCtrlBase->Invalidate();
	}
}

void ListCtrlBase::OnVScroll(int nSBCode, int nPos, IMessage* pMsgFrom)
{
	SetMsgHandled(FALSE);
	SetCalcFirstLastDrawItemFlag();
}
void ListCtrlBase::OnHScroll(int nSBCode, int nPos, IMessage* pMsgFrom)
{
    SetMsgHandled(FALSE);
    SetCalcFirstLastDrawItemFlag();
}
BOOL  ListCtrlBase::OnMouseWheel(unsigned int nFlags, short zDelta, POINT pt)
{
    BOOL bHandled = TRUE;
    BOOL bNeedRefresh = FALSE;
    m_mgrScrollBar.DoMouseWheel(nFlags, zDelta, pt, bHandled, bNeedRefresh);

    if (bNeedRefresh)
    {
        SetCalcFirstLastDrawItemFlag();

        Invalidate();
    }

    return bHandled;
}

long  ListCtrlBase::OnInertiaVScroll(unsigned int uMsg, WPARAM wParam, LPARAM lParam)
{
    SetCalcFirstLastDrawItemFlag();
    this->Invalidate();
    return 0;
}

// 外部调用
void  ListCtrlBase::SetNeedLayoutItems()
{
	m_bNeedLayoutItems = true;
    Invalidate();

	if (IsRectEmpty(&m_rcParent))
	{
		// 如果控件大小为空，调用Invalidate也不会触发onpaint，也就不会
		// 触发layoutitem
		// HACK:
		SetRect(&m_rcParent, 0, 0, 1, 1);
	}
}

void  ListCtrlBase::SetNeedSortItems()
{
    m_bNeedSortItems = true;
    m_pIListCtrlBase->Invalidate();
}

void  ListCtrlBase::UpdateListIfNeed()
{
    if (m_bNeedSortItems)
    {
        Sort();
    }
    if (m_bNeedLayoutItems)
    {
        LayoutItem(nullptr, false);
    }
    if (m_bNeedUpdateItemIndex)
    {
        UpdateItemIndex(nullptr);
    }
    if (m_bNeedCalcFirstLastDrawItem)
    {
        m_bNeedCalcFirstLastDrawItem = false;
        this->CalcFirstLastDrawItem();
    }
}

void ListCtrlBase::OnPaint(IRenderTarget* pRenderTarget)
{ 
    m_bPaintingCtrlRef ++;
    UpdateListIfNeed();

	
	ListItemBase* pItem = m_pFirstDrawItem;
	while (pItem)
	{
		if (pItem->IsFloat() || pItem->GetSelfLayer())
		{
			pItem = pItem->GetNextVisibleItem();
			continue;
		}
		
        RECT rcParent;
        pItem->GetParentRect(&rcParent);
        if (pRenderTarget->IsRelativeRectInClip(&rcParent))
        {
            if (HANDLED != this->OnDrawItem(pRenderTarget, pItem)) // 绘制背景
            {
                pItem->Draw(pRenderTarget);                        // 子对象绘制
                pItem->DrawItemInnerControl(pRenderTarget);        // 绘制内部控件
            }
        }

		if (pItem == m_pLastDrawItem)
			break;

		pItem = pItem->GetNextVisibleItem();
	}

    m_bPaintingCtrlRef--;
}

void  ListCtrlBase::OnPostPaint(IRenderTarget* pRenderTarget)
{
    m_MgrFloatItem.DoPaint(pRenderTarget);
}

HANDLED_VALUE  ListCtrlBase::OnDrawItem(IRenderTarget* pRenderTarget, ListItemBase* p)
{
// 	ControlStyle s = {0};
// 	s.ownerdraw = 1;
//     if (m_pIListCtrlBase->TestControlStyle(&s))
//     {
//         OWNERDRAWSTRUCT s;
//         s.pItemDraw = p->GetIListItemBase();
//         s.pObjDraw = m_pIListCtrlBase;
//         s.pRenderTarget = pRenderTarget;
// 
//         UIMSG  msg;
//         msg.message = UI_MSG_NOTIFY;
//         msg.nCode = UI_WM_OWNERDRAW;
//         msg.wParam = (WPARAM)&s;
//         msg.pMsgFrom = m_pIListCtrlBase;
// 
//         if (HANDLED == m_pIListCtrlBase->DoNotify(&msg))
//             return HANDLED;
//     }
// 
//     LISTITEMSTYLE s2 = {0};
//     s2.bOwnerDraw = 1;
//     if (p->TestStyle(s2))
//     {
//         OWNERDRAWSTRUCT s;
//         s.pItemDraw = p->GetIListItemBase();
//         s.pObjDraw = m_pIListCtrlBase;
//         s.pRenderTarget = pRenderTarget;
// 
//         UIMSG  msg;
//         msg.message = UI_WM_OWNERDRAW;
//         msg.wParam = (WPARAM)&s;
//         msg.pMsgFrom = msg.pMsgTo = m_pIListCtrlBase;
//         if (HANDLED == UISendMessage(&msg))
//             return HANDLED;
//     }

    return NOT_HANDLED;
}

void ListCtrlBase::SetHoverItem(ListItemBase* pItem)
{
	if (m_pMKMgr)
        m_pMKMgr->SetHoverItem(pItem);
}
void ListCtrlBase::SetPressItem(ListItemBase* pItem)
{
	if (m_pMKMgr)
        m_pMKMgr->SetPressItem(pItem);
}


// void ListCtrlBase::OnFontModifyed(IRenderFont* pFont)
// {
// 	if (pFont == GetRenderFont() && nullptr != pFont)
// 	{
// 		ListItemBase* pItem = m_pFirstItem;
// 		while (pItem)
// 		{
// 			this->MeasureItem(pItem);
// 			pItem = pItem->GetNextItem();
// 		}
// 	}
// 
// 	this->LayoutItem();
// }

void  ListCtrlBase::InvalidateItem(ListItemBase* pItem)
{
	if (nullptr == pItem)
		return;

    UIASSERT(pItem->GetListCtrlBase() == this);

    if (!pItem->IsVisible())
        return;

	if (pItem->GetSelfLayer())
	{
		pItem->GetSelfLayer()->Invalidate(nullptr);
		return;
	}

    CRect rcItem;
    if (pItem->IsFloat())
    {
        pItem->GetFloatRect(&rcItem);
    }
    else
    {
        pItem->GetParentRect(&rcItem);
        ItemRect2ObjectRect(&rcItem, &rcItem);
    }
    if (rcItem.IsRectEmpty())
        return;
    
    CRect rcListCtrl;
    this->GetClientRectInObject(&rcListCtrl);

    if (IntersectRect(&rcItem, &rcItem, &rcListCtrl))
    {
    	this->Invalidate(&rcItem);
    }
}

// 内部控件在刷新时，与RedrawItem有些不同：
// 1. 此时已有获取到pRenderTarget了，不需要调用begindrawpart. <<-- 但由于还得做一些绘制背景的操作
// 2. 不需要commit到窗口上面
// 3. 不需要再绘制innerctrl，只需要listitem内容
void ListCtrlBase::RedrawItemByInnerCtrl(IRenderTarget* pRenderTarget, ListItemBase* pItem)
{
	if (!IsItemVisibleInScreen(pItem))
		return;

    if (HANDLED == this->OnDrawItem(pRenderTarget, pItem))
        return;

    pItem->Draw(pRenderTarget); 
}



//////////////////////////////////////////////////////////////////////////

const wchar_t*  ListCtrlBase::GetItemText(ListItemBase* pItem)
{
	if (nullptr == pItem)
		return nullptr;
	
	return pItem->GetText();
}

int  ListCtrlBase::GetVisibleItemCount()
{
    int nRet = 0;
    ListItemBase*  pItem = FindVisibleItemFrom(nullptr);
    while (pItem)
    {
        nRet ++;
        pItem = pItem->GetNextVisibleItem();
    }
    return nRet;
}
int   ListCtrlBase::GetRootItemCount()
{
    int i = 0;

    ListItemBase* p = m_pFirstItem;
    while (p)
    {
        i++;
        p = p->GetNextItem();
    }

    return i;
}

// item share data op
IListItemShareData*  ListCtrlBase::GetItemTypeShareData(int lType)
{
    map<int, IListItemShareData*>::iterator iter = m_mapItemTypeShareData.find(lType);
    if (iter != m_mapItemTypeShareData.end())
        return iter->second;

    return nullptr;
}
void  ListCtrlBase::SetItemTypeShareData(int lType, IListItemShareData* pData)
{
    m_mapItemTypeShareData[lType] = pData;
    
    // 设置属性
    if (pData)
    {
        pData->GetImpl()->SetListCtrlBase(this);
        if (this->m_pIMapAttributeRemain)
        {
			SerializeParam data = {0};
            data.pUIApplication = GetIUIApplication();
			data.pMapAttrib = m_pIMapAttributeRemain;
			data.nFlags = SERIALIZEFLAG_LOAD;
			UISendMessage(pData, UI_MSG_SERIALIZE, (WPARAM)&data);
        }
    }
}
void  ListCtrlBase::RemoveItemTypeShareData(int lType)
{
    map<int, IListItemShareData*>::iterator iter = m_mapItemTypeShareData.find(lType);
    if (iter != m_mapItemTypeShareData.end())
    {
        m_mapItemTypeShareData.erase(iter);
    }
}

// pt为窗口坐标
// ListItemBase* ListCtrlBase::HitTest(POINT ptWindow)
// {
//     return GetItemByWindowPoint(ptWindow);
//     
//     CRect rcClient;
//     m_pIListCtrlBase->GetClientRectInObject(&rcClient);
// 
//     // 1. 转换为内部坐标
// 
//     POINT pt;
//     m_pIListCtrlBase->WindowPoint2ObjectPoint(&ptWindow, &pt, true);
//     if (FALSE == rcClient.PtInRect(pt))
//         return nullptr;
// 
//     m_pIListCtrlBase->ObjectPoint2ObjectClientPoint(&pt, &pt);
// 
//     // 2. 判断
// 
//     ListItemBase* p = m_pFirstDrawItem;
//     while (p)
//     {
//         CRect rcParent;
//         p->GetParentRect(&rcParent);
//         if (PtInRect(&rcParent, pt))
//         {
//             if (ptItem)
//             {
//                 ptItem->x = pt.x - rcParent.left;
//                 ptItem->y = pt.y - rcParent.top;
//             }
//             return p;
//         }
// 
//         if (p == m_pLastDrawItem)
//             break;
// 
//         p = p->GetNextVisibleItem();
//     }
// 
//     return nullptr;
// }

// pt为控件内部坐标
long  ListCtrlBase::OnHitTest(unsigned int uMsg, WPARAM wParam, LPARAM lParam)
{
    POINT pt = {(int)wParam, (int)lParam};

    // 1. 检查float item 
    ListItemBase* pItem = m_MgrFloatItem.HitTest(pt);
    if (pItem)
        return (long)pItem->GetIListItemBase();

    // 2. 检查可视item
	UpdateListIfNeed();
//     if (!m_pFirstDrawItem || m_bNeedCalcFirstLastDrawItem)
//     {
//         CalcFirstLastDrawItem();
//         m_bNeedCalcFirstLastDrawItem= false;
//     }
    ListItemBase* p = m_pFirstDrawItem;
    while (p)
    {
        // TODO: 是否有必要过滤float item 

        CRect rcParent;
        p->GetParentRect(&rcParent);

        if (PtInRect(&rcParent, pt))
        {
            return (long)p->GetIListItemBase();
        }
        if (p == m_pLastDrawItem)
            break;

        p = p->GetNextVisibleItem();
    }

    return 0;
}
void  ListCtrlBase::SetFocusRender(IRenderBase* p)
{
    SAFE_RELEASE(m_pFocusRender);
    m_pFocusRender = p;

    if (m_pFocusRender)
        m_pFocusRender->AddRef();
}


bool  ListCtrlBase::InsertItem(
		ListItemBase* pNewItem, 
        IListItemBase* pParent, 
        IListItemBase* pAfter)
{
    if (false == _InsertItemToTree(pNewItem, pParent, pAfter))
        return false;

    pNewItem->SetIListCtrlBase(m_pIListCtrlBase);
    UISendMessage(pNewItem->GetIListItemBase(), UI_MSG_INITIALIZE);

	SetNeedLayoutItems();
    Invalidate();

    UIMSG  msg;
    msg.pMsgTo = m_pIListCtrlBase;
    msg.message = UI_MSG_NOTIFY;
    msg.nCode = UI_LCN_ITEMADD;
    msg.wParam = (WPARAM)pNewItem->GetIListItemBase();
    msg.lParam = 0;
    UISendMessage(&msg);

    return true;
}

bool ListCtrlBase::_InsertRoot(ListItemBase* pNewItem)
{
    if (nullptr == m_pFirstItem)
    {
        m_pFirstItem = pNewItem;
        m_pLastItem = pNewItem;
    }
    else
    {
        pNewItem->SetNextItem(m_pFirstItem);
        m_pFirstItem->SetPrevItem(pNewItem);
        m_pFirstItem = pNewItem;
    }
    return true;
}

bool ListCtrlBase::_InsertAfter(
		ListItemBase* pNewItem, ListItemBase* pInsertAfter)
{
    if (nullptr == pInsertAfter)
        return false;

    ListItemBase* pOldNext = pInsertAfter->GetNextItem();
    pInsertAfter->SetNextItem(pNewItem);
    pNewItem->SetPrevItem(pInsertAfter);
    pNewItem->SetParentItem(pInsertAfter->GetParentItem());

    if (nullptr != pOldNext)
    {
        pOldNext->SetPrevItem(pNewItem);
        pNewItem->SetNextItem(pOldNext);
    }
    if (m_pLastItem == pInsertAfter)
    {
        m_pLastItem = pNewItem;
    }

    return true;
}

bool ListCtrlBase::_InsertBefore(
		ListItemBase* pNewItem, ListItemBase* pInsertBefore)
{
    if (nullptr == pInsertBefore)
        return false;

    ListItemBase* pOldPrev = pInsertBefore->GetPrevItem();
    pInsertBefore->SetPrevItem(pNewItem);
    pNewItem->SetNextItem(pInsertBefore);
    pNewItem->SetParentItem(pInsertBefore->GetParentItem());

    if (nullptr != pOldPrev)
    {
        pOldPrev->SetNextItem(pNewItem);
        pNewItem->SetPrevItem(pOldPrev);
    }
    if (m_pLastItem == pInsertBefore)
    {
        m_pLastItem = pNewItem;
    }
    if (m_pFirstItem == pInsertBefore)
    {
        m_pFirstItem = pNewItem;
    }

    return true;
}

bool ListCtrlBase::_InsertFirstChild(ListItemBase* pNewItem, ListItemBase* pParent)
{
    if (nullptr == pParent)
        return false;

	pParent->AddChildFront(pNewItem);
    return true;
}

bool ListCtrlBase::_InsertLastChild(ListItemBase* pNewItem, ListItemBase* pParent)
{
    if (nullptr == pParent)
        return false;

    if (nullptr == pParent->GetChildItem())
        return this->_InsertFirstChild(pNewItem, pParent);

    pParent->AddChild(pNewItem);
    return true;
}


void ListCtrlBase::ToggleItemExpand(ListItemBase* pItem, bool bUpdate)
{
    if (nullptr == pItem)
        return;

    if (pItem->IsExpand())
        CollapseItem(pItem, bUpdate);
    else
        ExpandItem(pItem, bUpdate);
}

void ListCtrlBase::CollapseItem(ListItemBase* pItem, bool bUpdate)
{
    if (nullptr == pItem)
        return ;

    if (pItem->IsCollapsed())
        return;

    pItem->SetExpand(false);

    if (nullptr == pItem->GetChildItem())
        return;

    // 判断当前选择项是否被隐藏，（如果自己不可选【一些特殊场景，只允许子结点被选中】，保持不变）
    if (pItem->IsSelectable() && m_pFirstSelectedItem)
    {
        if (pItem->IsMyChildItem(GetFirstSelectItem(), true))
        {
            SelectItem(pItem, false);  // 将选择项转移到父结点
        }
    }
    ListItemBase* pFocusItem = GetFocusItem();
    if (pFocusItem)
    {
        if (pItem->IsMyChildItem(pFocusItem, true))
        {
            SetFocusItem(pItem);
        }
    }

    this->SetCalcFirstLastDrawItemFlag();
    this->UpdateItemIndex(nullptr);
    this->LayoutItem(m_pFirstItem, bUpdate);

    this->Invalidate();  // PS: 有些对象经折叠后被隐藏了，是不能再调用RedrawItem的，因此也必须全部刷新
}

void ListCtrlBase::ExpandItem(ListItemBase* pItem, bool bUpdate)
{
    if (nullptr == pItem)
        return ;

    bool bDoExpandAction = false;
    ListItemBase* pParent = pItem;
    while (pParent)
    {
        if (pParent->IsExpand())
        {
            pParent = pParent->GetParentItem();    
            continue;
        }
         
        bDoExpandAction = true;
        pParent->SetExpand(true);
        pParent = pParent->GetParentItem();    
    }
    
    if (bDoExpandAction)
    {
        this->SetCalcFirstLastDrawItemFlag();
        this->UpdateItemIndex(nullptr);
        this->LayoutItem(m_pFirstItem, bUpdate);

        this->Invalidate(); 
    }
}

void  ListCtrlBase::CollapseAll(bool bUpdate)
{
    ListItemBase*  pItem = m_pFirstItem;
    while (pItem)
    {
        pItem->SetExpand(false, true);
        pItem = pItem->GetNextTreeItem();
    }

    if (bUpdate)
    {
        this->LayoutItem(nullptr, true);
    }
}
void  ListCtrlBase::ExpandAll(bool bUpdate)
{
    ListItemBase*  pItem = m_pFirstItem;
    while (pItem)
    {
        pItem->SetExpand(true, true);
        pItem = pItem->GetNextTreeItem();
    }

    if (bUpdate)
    {
        this->LayoutItem(nullptr, true);
    }
}

IMKMgr* ListCtrlBase::virtualGetIMKMgr()
{
    if (nullptr == m_pMKMgr)
        return nullptr;

    return static_cast<IMKMgr*>(m_pMKMgr);
}

BOOL  ListCtrlBase::ProcessItemMKMessage(UIMSG* pMsg, ListItemBase* pItem)
{
    if (nullptr == m_pMKMgr)
        return FALSE;

    return m_pMKMgr->ProcessItemMessage(pMsg, pItem);
}


// 该函数通常是用于将一个item从一个列表移到另一个列表当中去
void  ListCtrlBase::RemoveItemFromTree(ListItemBase* pItem)
{
	if (nullptr == pItem)
		return;

	ListItemBase* pNextItem = pItem->GetNextItem();
	bool bRet = _RemoveItem(pItem, false);
	if (false == bRet)
		return;

	this->UpdateItemIndex(pNextItem);

	SetNeedLayoutItems();
	Invalidate();
}

// 将p从本树中脱离，但不负责其它逻辑，如focus/selection/map处理。
void  ListCtrlBase::_RemoveItemFromTree(ListItemBase*  pItem)
{
    if (!pItem)
        return;

    // 从列表中删除
    // 注：必须先从列表中删除，再清除selection，
    //     否则fire selection changed响应中还能拿到该ITEM，导致崩溃
    if (pItem->GetPrevItem())
    {
        pItem->GetPrevItem()->SetNextItem(pItem->GetNextItem());
    }
    if (pItem->GetNextItem())
    {
        pItem->GetNextItem()->SetPrevItem(pItem->GetPrevItem());
    }

    // 自己做为父结点的第一个子结点
    ListItemBase* pParent = pItem->GetParentItem();
    if (pParent)
    {
        if (pParent->GetChildItem() == pItem)
            pParent->SetChildItem(pItem->GetNextItem());

        if (pParent->GetLastChildItem() == pItem)
            pParent->SetLastChildItem(pItem->GetPrevItem());
    }

    if (m_pFirstItem == pItem)
    {
        m_pFirstItem = pItem->GetNextItem();
    }
    if (m_pLastItem == pItem)
    {
        m_pLastItem = pItem->GetPrevItem();
    }
	if (m_pEditingItem == pItem)
	{
		// TODO cancel edit.
		m_pEditingItem = nullptr;
	}
    this->SetCalcFirstLastDrawItemFlag();
    if (pItem == m_pFirstDrawItem || pItem == m_pLastDrawItem)
    {
        m_pFirstDrawItem = nullptr;
        m_pLastDrawItem = nullptr;
    }

    pItem->SetNextItem(nullptr);
    pItem->SetPrevItem(nullptr);
    pItem->SetParentItem(nullptr);

    ListItemBase* p = pItem;
    do 
    {
        m_nItemCount--;
    }
    while (p = p->GetNextTreeItem());
}

bool  ListCtrlBase::is_my_item(IListItemBase* pItem)
{
    ListItemBase* pValidItem = get_valid_list_item(pItem);
    if (pValidItem == nullptr) 
        return true;

    if (pValidItem->GetListCtrlBase() != this)
        return false;

    return true;
}

ListItemBase*  ListCtrlBase::get_valid_list_item(IListItemBase* pItem)
{
    if (pItem == nullptr || 
        pItem == UITVI_ROOT ||
        pItem == UITVI_LAST || 
        pItem == UITVI_FIRST)
        return nullptr;

    return pItem->GetImpl();
}

bool  ListCtrlBase::MoveItem(ListItemBase* pItem, IListItemBase* pNewParent, IListItemBase* pInsertAfter)
{
    if (!pItem)
        return false;

    // 异常
    // 1. item不是listctrl的列表项
    if (pItem->GetListCtrlBase() != this)
        return false;

    if (!is_my_item(pNewParent) || !is_my_item(pInsertAfter))
        return false;

    // 2. parent 是自己或者自己了子孙结点
    if (pItem->GetIListItemBase() == pNewParent || pItem->GetIListItemBase() == pInsertAfter)
        return false;

    ListItemBase* pValidNewParent = get_valid_list_item(pNewParent);
    if (pValidNewParent && pItem->IsMyChildItem(pValidNewParent, true))
        return false;

    ListItemBase* pValidInsertAfter = get_valid_list_item(pInsertAfter);
    if (pValidInsertAfter && pItem->IsMyChildItem(pValidInsertAfter, true))
        return false;

    // 与当前位置一致，不需要移动
    IListItemBase* pOldParent = nullptr;
    IListItemBase* pOldPrev = nullptr;
    if (pItem->GetParentItem())
        pOldParent = pItem->GetParentItem()->GetIListItemBase();
    if (pItem->GetPrevItem())
        pOldPrev = pItem->GetPrevItem()->GetIListItemBase();

    if (pOldParent == pNewParent && pOldPrev == pInsertAfter)
        return false;

    // 其它状态影响
    if (pItem == m_pFirstDrawItem || pItem == m_pLastDrawItem)
    {
        SetCalcFirstLastDrawItemFlag();
    }

    _RemoveItemFromTree(pItem);
    _InsertItemToTree(pItem, pNewParent, pInsertAfter);

	SetNeedLayoutItems();
    Invalidate();

    return true;
}

bool  ListCtrlBase::_InsertItemToTree(ListItemBase*  pNewItem, IListItemBase* pParent, IListItemBase* pAfter)
{
    if (nullptr == pParent)
        pParent = UITVI_ROOT;
    if (nullptr == pAfter)
        pAfter = UITVI_LAST;
    if (UITVI_ROOT == pAfter)
    {
        pParent = UITVI_ROOT;
        pAfter = UITVI_FIRST;
    }

    // 现在pParent可取值: ROOT | xxx
    //      hAfter可取值 : FIRST | LAST | xxx
    bool bRet = false;
    do
    {
        if (UITVI_ROOT == pParent)
        {
            if (UITVI_FIRST == pAfter)
            {
                bRet = _InsertRoot(pNewItem);
            }
            else if (UITVI_LAST == pAfter)
            {
                ListItemBase* pLastRoot = GetLastItem();
                if (nullptr == pLastRoot)
                    bRet = _InsertRoot(pNewItem);
                else
                    bRet = _InsertAfter(pNewItem, pLastRoot);
            }
            else
            {
                if (pAfter->GetParentItem() != nullptr)  // 不是根结点
                    break;

                bRet = _InsertAfter(pNewItem, pAfter?pAfter->GetImpl():nullptr);
            }
        }
        else
        {
            if (UITVI_FIRST == pAfter)
            {
                bRet = _InsertFirstChild(pNewItem, pParent?pParent->GetImpl():nullptr);
            }
            else if (UITVI_LAST == pAfter)
            {
                bRet = _InsertLastChild(pNewItem, pParent?pParent->GetImpl():nullptr);
            }
            else
            {
                if (pAfter->GetParentItem() != pParent)  // 不是对应子结点
                    break;

                bRet = _InsertAfter(pNewItem, pAfter?pAfter->GetImpl():nullptr);
            }
        }
    }
    while(0);

    if (bRet)
    {
        m_nItemCount++;
        if (pNewItem->GetId() != 0)
        {
            m_mapItem[pNewItem->GetId()] = pNewItem->GetIListItemBase();
        }
        SetCalcFirstLastDrawItemFlag();
    }
    return bRet;
}

void  ListCtrlBase::ModifyListCtrlStyle(LISTCTRLSTYLE* add, LISTCTRLSTYLE* remove)
{
    LISTCTRLSTYLE  oldStyle = m_listctrlStyle;

#define  MODIFY(x) \
    if (add && add->x) \
        m_listctrlStyle.x = 1; \
    if (remove && remove->x) \
        m_listctrlStyle.x = 0;

    MODIFY(sort_ascend);
    MODIFY(sort_descend);
    MODIFY(sort_child);
    MODIFY(multiple_sel);
    MODIFY(popuplistbox);
    MODIFY(menu);
    MODIFY(destroying);
    MODIFY(float_group_head);
    MODIFY(dragwnd_if_clickblank);
    MODIFY(changeselection_onlbuttonup);

    if (oldStyle.menu != m_listctrlStyle.menu ||
        oldStyle.multiple_sel != m_listctrlStyle.multiple_sel ||
        oldStyle.popuplistbox != m_listctrlStyle.popuplistbox)
    {
        update_mouse_mgr_type();
    }

	if ((add && add->multiple_sel) || (remove && remove->multiple_sel))
	{
		update_mouse_mgr_type();
	}
}
bool  ListCtrlBase::TestListCtrlStyle(LISTCTRLSTYLE* test)
{
	if (!test)
		return false;

#define  TEST(x) \
    if (test->x && !m_listctrlStyle.x) \
        return false;

    TEST(sort_ascend);
    TEST(sort_descend);
    TEST(sort_child);
    TEST(multiple_sel);
    TEST(popuplistbox);
    TEST(menu);
    TEST(destroying);
    TEST(float_group_head);
    TEST(dragwnd_if_clickblank);
    TEST(changeselection_onlbuttonup);

	return true;
}


long  ListCtrlBase::OnGesturePan(unsigned int, WPARAM wParam, LPARAM lParam)
{
	long lRet = HANDLED;
	
	static int  bounce_edge_power = 0;
	//UI::GESTUREINFO* pgi = (UI::GESTUREINFO*)lParam;

    //::OutputDebugStringA("ScrollBarManager::OnGesturePan");
    int xOffset = GET_X_LPARAM(wParam);
    int yOffset = GET_Y_LPARAM(wParam);

	bounce_edge_power += yOffset;

    bool bNeedUpdate = false;
    if (xOffset)
    {
        if (m_mgrScrollBar.SetHScrollPos(
				m_mgrScrollBar.GetHScrollPos()-xOffset))
		{
            bNeedUpdate = true;
		}
    }
    if (yOffset)
    {
		int nMaxPos = m_mgrScrollBar.GetVScrollMaxPos();
		int nCurPos = m_mgrScrollBar.GetVScrollPos();
		int nTry = nCurPos - yOffset;

		if (nTry < 0 || nTry > nMaxPos)
		{
			lRet = GESTURE_RETURN_NEED_BOUNCE_EDGE;
		}

		if (m_mgrScrollBar.SetVScrollPos(nTry))   
		{
            bNeedUpdate = true;
		}
    }

    if (bNeedUpdate)
    {
		if (GetUIApplication())
			GetUIApplication()->HideToolTip();

		this->SetCalcFirstLastDrawItemFlag();
        this->Invalidate();
    }
    return lRet;
}

void  ListCtrlBase::EnableInnerDrag(bool b)
{
	if (b)
	{
		if (!m_pInnerDragMgr)
		{
			m_pInnerDragMgr = new ListCtrlInnerDragMgr;
			m_pInnerDragMgr->SetListCtrlBase(this);
		}
	}
	else
	{
		SAFE_DELETE(m_pInnerDragMgr);
	}
}

void  ListCtrlBase::EnableReusable(IListCtrlReusableDelegate* p)
{
	if (p && !m_pReusable)
	{
		m_pReusable = new ListCtrlReusable;
		m_pReusable->Init(p);
	}
	else if (!p && m_pReusable)
	{
		m_pReusable->Release();
		SAFE_DELETE(m_pReusable);
	}
	return;
}

}