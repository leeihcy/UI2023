#include "stdafx.h"
#include "listitembase.h"
#include "..\ListItemRootPanel\listitemrootpanel.h"
#include "..\listctrlbase.h"
#include "..\float\float_item_mgr.h"
#include "include/interface/ilayout.h"
#include "include/interface/iimagerender.h"
#include "src/Base\Object\object.h"
#include "include/interface/iuires.h"
#include "..\animate_callback\listctrl_animate.h"

using namespace ui;

ListItemBase::ListItemBase(IListItemBase* p) : Message(p), m_itemLayer(*this)
{
    m_pIListItemBase = p;
    m_nConfigWidth = m_nConfigHeight = NDEF;

    m_nLineIndex = 0;
    m_nTreeIndex = 0;
    m_nNeighbourIndex = 0;
  
    m_pPrev = m_pNext = m_pPrevSelection = m_pNextSelection = nullptr;
    m_pParent = m_pChild = m_pLastChild = nullptr;

    m_rcParent.SetRectEmpty();

    m_lId = 0;
    m_lItemType = 0;
    m_pData = nullptr;
    m_pCtrlData = nullptr;
    m_nItemState = 0;
    m_nItemFlag = 0;
    m_pIconRender = nullptr;
    m_pListCtrlBase = nullptr;
    m_nFlagsDelayOp = 0;
    m_lSort = 0;
    memset(&m_itemStyle, 0, sizeof(m_itemStyle));

    m_pPanelRoot = nullptr;
}
ListItemBase::~ListItemBase()
{
	m_itemLayer.DestroyLayer();

    if (IsEditing())
    {
        //m_pListCtrlBase->DiscardEdit();
		UIMSG msg;
		msg.message = UI_MSG_LISTCTRL_DISCARDEDIT;
		msg.pMsgFrom = msg.pMsgTo = GetIListCtrlBase();
		UISendMessage(&msg);
    }
    m_pListCtrlBase->ItemDestructed(this);

    ListItemBase* pItem = m_pChild;
    while (pItem)
    {
        ListItemBase* pNext = pItem->GetNextItem();
        pItem->GetIListItemBase()->Release();

        pItem = pNext;
    }
    m_pChild = nullptr;
    m_pLastChild = nullptr;
    m_pParent = nullptr;

    m_pPrev = m_pNext = m_pPrevSelection = m_pNextSelection = nullptr;
    m_pData = nullptr;
    m_pListCtrlBase = nullptr;
    SAFE_RELEASE(m_pIconRender);
	SAFE_RELEASE(m_pPanelRoot);
}

void  ListItemBase::SetIListCtrlBase(IListCtrlBase* p)
{
    if (p)
        m_pListCtrlBase = p->GetImpl();
    else
        m_pListCtrlBase = nullptr;

    m_ToolTipMgr.SetListItem(this);
}
IListCtrlBase*  ListItemBase::GetIListCtrlBase() 
{
    if (m_pListCtrlBase)
        return m_pListCtrlBase->GetIListCtrlBase();
    return nullptr;
}
ListCtrlBase*  ListItemBase::GetListCtrlBase()
{
    return m_pListCtrlBase;
}
void  ListItemBase::SetId(UINT_PTR n)
{
    if (n == m_lId)
        return;

    if (m_pListCtrlBase)
    {
        m_pListCtrlBase->ItemIdChanged(
            m_pIListItemBase, m_lId, n);
    }

    m_lId = n; 
}

void  ListItemBase::AddChildFront(ListItemBase* p)
{
	if (!p)
		return;

	ListItemBase* pOldChild = this->GetChildItem();
	this->SetChildItem(p);
	p->SetParentItem(this);

	if (pOldChild)
	{
		pOldChild->SetPrevItem(p);
		p->SetNextItem(pOldChild);
	}
    else
    {
        this->SetLastChildItem(p);
    }

}
void  ListItemBase::AddChild(ListItemBase* p)
{
	if (!p)
		return;

	ListItemBase* pOldLast = this->GetLastChildItem();
	p->SetParentItem(this);

	if (pOldLast)
	{
		pOldLast->SetNextItem(p);
		p->SetPrevItem(pOldLast);
	}
	else
	{
		this->SetChildItem(p);
	}
    this->SetLastChildItem(p);
}

// ���Լ��������Լ����ӽ�㣩�����ṹ���Ƴ�
// void ListItemBase::RemoveMeInTheTree()
// {
//     if (m_pPrev)
//     {
//         m_pPrev->m_pNext = this->m_pNext;
//     }
//     else
//     {
//         if (m_pParent) 
//         {
//             m_pParent->m_pChild = this->m_pNext;
//         }
//     }
// 
//     if (m_pNext)
//     {
//         m_pNext->m_pPrev = this->m_pPrev;
//     }
//     else
//     {
//         m_pParent->m_pLastChild = this->m_pPrev;
//     }
// 
//     m_pParent = m_pNext = m_pPrev = nullptr;
// }

void  ListItemBase::SetText(const wchar_t* szText)
{
    if (nullptr == szText)
    {
        m_strText.clear();
        return;
    }
    m_strText = szText;
}
const wchar_t*  ListItemBase::GetToolTip()                
{
    return m_ToolTipMgr.GetToolTip();
}
void  ListItemBase::SetToolTip(const wchar_t* szText)  
{
    m_ToolTipMgr.SetToolTip(szText);
}
void  ListItemBase::ShowSingleToolTip()
{
    m_ToolTipMgr.ShowSingleToolTip();
}
unsigned int  ListItemBase::GetItemState() 
{ 
    return m_nItemState; 
}
unsigned int  ListItemBase::GetItemDelayOp() 
{ 
    return m_nFlagsDelayOp; 
}
void  ListItemBase::AddItemDelayOp(int n)
{ 
    m_nFlagsDelayOp |= n;
}
void  ListItemBase::RemoveDelayOp(int n)
{ 
    m_nFlagsDelayOp &= ~n; 
}
void  ListItemBase::ClearDelayOp()
{ 
    m_nFlagsDelayOp = 0; 
}
void  ListItemBase::SetLazyLoadData(bool b)
{
    if (b)
        m_nFlagsDelayOp |= DELAY_OP_LAZYLOADDATA;
    else
        m_nFlagsDelayOp &= ~DELAY_OP_LAZYLOADDATA;
}
long  ListItemBase::GetSort()
{
    return m_lSort;
}
void  ListItemBase::SetSort(long l)
{
    if (m_lSort == l)
        return;

    m_lSort = l;

    if (m_pListCtrlBase)
        m_pListCtrlBase->SetNeedSortItems();
}

bool  ListItemBase::IsMySelfVisible()
{
    if (m_nItemState & OSB_UNVISIBLE)
        return false;
    else
        return true;
}

bool  ListItemBase::IsVisible()
{
    if (false == IsMySelfVisible())
        return false;

    if (nullptr == m_pParent)
        return true;

    if (m_pParent->IsCollapsed())
        return false;

    return m_pParent->IsVisible();
}

bool  ListItemBase::IsDisable()
{
    if (m_nItemState & OSB_DISABLE)
        return true;
    else
        return false;
}
bool  ListItemBase::IsHover()
{
    if (m_nItemState & OSB_HOVER)
        return true;
    else
        return false;
}
bool  ListItemBase::IsPress()
{
    if (m_nItemState & OSB_PRESS)
        return true;
    else
        return false;
}
bool  ListItemBase::IsFocus()
{
    if (m_nItemState & OSB_FOCUS)
        return true;
    else
        return false;
}
bool  ListItemBase::IsCollapsed()
{
    if (m_nItemState & OSB_COLLAPSED)
        return true;
    else
        return false;
}
bool  ListItemBase::IsExpand()
{
    if (m_nItemState & OSB_COLLAPSED)
        return false;
    else
        return true;
}
bool  ListItemBase::IsSelected()
{
    if (m_nItemState & OSB_SELECTED)
        return true;
    else
        return false;
}
bool  ListItemBase::IsChecked()
{
    if (m_nItemState & OSB_CHECKED)
        return true;
    else
        return false;
}
bool  ListItemBase::IsRadioChecked()
{
    if (m_nItemState & OSB_RADIOCHECKED)
        return true;
    else
        return false;
}
bool  ListItemBase::IsEditing()
{
	if (m_nItemState & OSB_EDITING)
		return true;
	else
		return false;
}

void  ListItemBase::SetPress(bool b, bool bNotify)
{
    unsigned int nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_PRESS;
    else
        m_nItemState &= ~OSB_PRESS;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
        msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_PRESS;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}
void  ListItemBase::SetHover(bool b, bool bNotify)
{
    unsigned int nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_HOVER;
    else
        m_nItemState &= ~OSB_HOVER;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
		msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_HOVER;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}
void  ListItemBase::SetFocus(bool b, bool bNotify)
{
    unsigned int nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_FOCUS;
    else
        m_nItemState &= ~OSB_FOCUS;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
		msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_FOCUS;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}
void  ListItemBase::SetExpand(bool b, bool bNotify)
{
    unsigned int nOld = m_nItemState;
    if (b)
        m_nItemState &= ~OSB_COLLAPSED;
    else
        m_nItemState |= OSB_COLLAPSED;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
		msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_COLLAPSED;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }

//     if (m_bExpand == b)
//         return;
// 
//     m_bExpand = b;
//     if (ListItemBase)
//     {
//         m_pCheckBtnExpandCollapse->SetCanRedraw(false);
//         m_pCheckBtnExpandCollapse->SetCheck(m_bExpand?BST_UNCHECKED:BST_CHECKED);
//         m_pCheckBtnExpandCollapse->SetCanRedraw(true);
//     }
}
void  ListItemBase::SetVisible(bool b, bool bNotify)
{
    unsigned int nOld = m_nItemState;
    if (b)
        m_nItemState &= ~OSB_UNVISIBLE;
    else
        m_nItemState |= OSB_UNVISIBLE;

	if (!b && m_pListCtrlBase && m_pListCtrlBase->m_pMKMgr)
	{
		m_pListCtrlBase->m_pMKMgr->OnHideItem(this);
	}

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
		msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_UNVISIBLE;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}
void  ListItemBase::SetSelected(bool b, bool bNotify)
{
    unsigned int nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_SELECTED;
    else
        m_nItemState &= ~OSB_SELECTED;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
		msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_SELECTED;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}

void  ListItemBase::SetDisable(bool b, bool bNotify)
{
    unsigned int nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_DISABLE;
    else
        m_nItemState &= ~OSB_DISABLE;

	if (b && m_pListCtrlBase && m_pListCtrlBase->m_pMKMgr)
	{
		m_pListCtrlBase->m_pMKMgr->OnDisableItem(this);
	}

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
		msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_DISABLE;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}
void  ListItemBase::SetChecked(bool b, bool bNotify)
{
    unsigned int nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_CHECKED;
    else
        m_nItemState &= ~OSB_CHECKED;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
		msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_CHECKED;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}
void  ListItemBase::SetRadioChecked(bool b, bool bNotify)
{
    unsigned int nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_RADIOCHECKED;
    else
        m_nItemState &= ~OSB_RADIOCHECKED;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
		msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_RADIOCHECKED;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}

void  ListItemBase::SetSelectable(bool b)
{
    if (b)
    {
        m_itemStyle.bNotSelectable = false;
    }
    else
    {
        m_itemStyle.bNotSelectable = true;
    }
}
bool  ListItemBase::IsSelectable()
{
    return !m_itemStyle.bNotSelectable;
}

void  ListItemBase::SetFocusable(bool b)
{
    m_itemStyle.bNotFocusable = !b;
}
bool  ListItemBase::IsFocusable()
{   
    return !m_itemStyle.bNotFocusable;
}
bool  ListItemBase::CanFocus()
{
	return IsFocusable() && IsVisible() && !IsDisable();
}
bool  ListItemBase::IsDragDropHover()
{
    return m_nItemState&OSB_DRAGDROPHOVER ? true:false;
}
void  ListItemBase::SetDragDropHover(bool b, bool bNotify)
{
    unsigned int nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_DRAGDROPHOVER;
    else
        m_nItemState &= ~OSB_DRAGDROPHOVER;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
		msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_DRAGDROPHOVER;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}

bool  ListItemBase::IsDraging()
{
    return m_nItemState&OSB_DRAGING ? true:false;
}
void  ListItemBase::SetDraging(bool b, bool bNotify)
{
    unsigned int nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_DRAGING;
    else
        m_nItemState &= ~OSB_DRAGING;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
		msg.message = UI_MSG_STATECHANGED;
        msg.wParam = OSB_DRAGING;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}

void  ListItemBase::SetEditing(bool b, bool bNotify)
{
	unsigned int nOld = m_nItemState;
	if (b)
		m_nItemState |= OSB_EDITING;
	else
		m_nItemState &= ~OSB_EDITING;

	if (bNotify && m_nItemState != nOld)
	{
		UIMSG  msg;
		msg.message = UI_MSG_STATECHANGED;
		msg.wParam = OSB_DRAGING;
		msg.pMsgTo = m_pIListItemBase;
		UISendMessage(&msg);
	}
}



// ���ӽ������ʱ�����ñ����ķ�ʽЧ�ʺܵ�
ListItemBase*  ListItemBase::GetLastChildItem()
{
    return m_pLastChild;

//     ListItemBase* pChild = (ListItemBase*)m_pChild;
//     if (nullptr == pChild)
//         return nullptr;
// 
//     if (nullptr == pChild->GetNextItem())
//         return (ListItemBase*)pChild;
// 
//     while (pChild = pChild->GetNextItem())
//     {
//         if (nullptr == pChild->GetNextItem())
//             break;
//     }
//     return (ListItemBase*)pChild;
}

ListItemBase*  ListItemBase::GetAncestorItem()
{
    ListItemBase* p = this;
    while (p->m_pParent)
        p = p->m_pParent;

    return p;
}

// �ж�pChild�Ƿ����Լ����ӽ�����������
bool  ListItemBase::IsMyChildItem(ListItemBase* pChild, bool bTestdescendant)
{
    if (nullptr == pChild)
        return false;

    if (bTestdescendant)
    {
        ListItemBase* pParent = pChild->GetParentItem();
        while (pParent)
        {
            if (pParent == this)
                return true;

            pParent = pParent->GetParentItem();
        }
    }
    else
    {
        return pChild->GetParentItem() == this;
    }

    return false;
}

unsigned int  ListItemBase::GetChildCount()
{
    unsigned int n = 0;
    ListItemBase* p = m_pChild;
    while (p)
    {
        n++;
        p = p->m_pNext;
    }
    return n;
}

unsigned int  ListItemBase::GetDescendantCount()
{
    unsigned int n = 0;
    ListItemBase* p = m_pChild;
    while (p)
    {
        n++;

        if (p->m_pChild)
            n += p->GetDescendantCount();

        p = p->m_pNext;
    }
    return n;
}

// ��ȡ�������е�δ�����
unsigned int  ListItemBase::GetDescendantLeafCount()
{
    unsigned int n = 0;
    ListItemBase* p = m_pChild;
    while (p)
    {
        if (p->m_pChild)
            n += p->GetDescendantLeafCount();
        else
            n++;

        p = p->m_pNext;
    }
    return n;
}

// ��ť��->��->�ҵ�˳���ȡ������һ�����
ListItemBase*  ListItemBase::GetNextTreeItem()
{
    ListItemBase* p = this;

    // ���ӽ��
    if (p->GetChildItem()) 
    {
        return p->GetChildItem();
    }

    // û���ӽ�㣬��ȡ��һ�����
    if (p->GetNextItem())
    {
        return p->GetNextItem();
    }

    // Ҳû����һ���ӽ�㣬��ȡ���������һ�����
    ListItemBase* pParent = p->GetParentItem();
    while (pParent)
    {
        ListItemBase* pNext = (ListItemBase*)pParent->GetNextItem();
        if (pNext)
            return pNext;

        pParent = pParent->GetParentItem();
    }

    return nullptr;
}
// ��ť��->��->����˳���ȡ������һ�����
ListItemBase*  ListItemBase::GetPrevTreeItem()
{
    ListItemBase* p = (ListItemBase*)this;

    // ��ȡ���������һ���ӽ��
    if (p->GetPrevItem())
    {
        ListItemBase* pLeft = p->GetPrevItem();
        ListItemBase* pChild = nullptr;
        while (pChild = pLeft->GetLastChildItem())
        {
            pLeft = pChild;
        }
        return pLeft;
    }

    // �и����
    if (p->GetParentItem()) 
    {
        return p->GetParentItem();
    }

    return nullptr;
}

ListItemBase*  ListItemBase::GetNextVisibleItem()
{
    ListItemBase* pTreeItem = this;

    while (pTreeItem = pTreeItem->GetNextTreeItem())
    {
        if (pTreeItem->IsVisible())
            return pTreeItem;
    }

    return nullptr;
}

ListItemBase*  ListItemBase::GetPrevVisibleItem()
{
    ListItemBase* pTreeItem = this;

    while (pTreeItem = pTreeItem->GetPrevTreeItem())
    {
        if (pTreeItem->IsVisible())
            return pTreeItem;
    }

    return nullptr;
}

ListItemBase*  ListItemBase::GetNextSelectableItem()
{
    ListItemBase* pTreeItem = this;

    while (pTreeItem = pTreeItem->GetNextVisibleItem())
    {
        if (pTreeItem->IsSelectable())
            return pTreeItem;
    }

    return nullptr;
}
ListItemBase*  ListItemBase::GetPrevSelectableItem()
{
    ListItemBase* pTreeItem = this;

    while (pTreeItem = pTreeItem->GetPrevVisibleItem())
    {
        if (pTreeItem->IsSelectable())
            return pTreeItem;
    }

    return nullptr;
}


ListItemBase*  ListItemBase::GetNextFocusableItem()
{
    ListItemBase* pTreeItem = this;

    while (pTreeItem = pTreeItem->GetNextVisibleItem())
    {
        if (pTreeItem->CanFocus())
            return pTreeItem;
    }

    return nullptr;
}
ListItemBase*  ListItemBase::GetPrevFocusableItem()
{
    ListItemBase* pTreeItem = this;

    while (pTreeItem = pTreeItem->GetPrevVisibleItem())
    {
        if (pTreeItem->CanFocus())
            return pTreeItem;
    }

    return nullptr;
}

// int  ListItemBase::GetDepth()
// {
//     int  nDepth = 0;
//     
//     ListItemBase*  pParent = m_pParent;
//     while (pParent)
//     {
//         nDepth++;
//         pParent = pParent->GetParentItem();
//     }
//     return nDepth;
// }

IPanel*  ListItemBase::GetIRootPanel()
{
	return m_pPanelRoot;
}

Panel*  ListItemBase::GetRootPanel() 
{ 
	if (!m_pPanelRoot)
		return nullptr;

	return m_pPanelRoot->GetImpl();
}
// ����RootPanel��Ϊ�����ڲ��ؼ���׼��
bool ListItemBase::CreateRootPanel()
{
    if (m_pPanelRoot)
        return true;

    if (nullptr == m_pListCtrlBase)
    {
        UIASSERT(0);
        return false;
    }

    IListItemRootPanel* p = IListItemRootPanel::
            CreateInstance(m_pListCtrlBase->GetIResource());
    if (nullptr == p)
        return false;

	ListItemRootPanel* pImpl = p->GetImpl();
    m_pPanelRoot = static_cast<IPanel*>(p);

    pImpl->SetListCtrlItem(m_pListCtrlBase, this);
    pImpl->SetParentObjectDirect(m_pListCtrlBase);   // �����Ļ�������root panel��ɣ�������Ҫ����ĸ���ϵ
    m_pPanelRoot->SetLayoutType(LAYOUT_TYPE_CANVAS);
    
    OnSize();

    return true;
}

long  ListItemBase::OnDelayOp(unsigned int, WPARAM, LPARAM)
{
    if (m_nFlagsDelayOp & DELAY_OP_LAZYLOADDATA)
    {
        OnLazyLoadData();
        m_nFlagsDelayOp &= ~DELAY_OP_LAZYLOADDATA;
    }
    if (m_nFlagsDelayOp & DELAY_OP_SIZE_CHANGED)
    {
        OnSize();
        m_nFlagsDelayOp &= ~DELAY_OP_SIZE_CHANGED;
    }
    return 0;
}

void  ListItemBase::OnLazyLoadData()
{
    UIMSG  msg;
    msg.message = UI_MSG_NOTIFY;
    msg.nCode = UI_LCN_LAZYLOADDATA;
    msg.pMsgFrom = GetIListCtrlBase();
    msg.wParam = (WPARAM)m_pIListItemBase;
    GetIListCtrlBase()->DoNotify(&msg);
}

void  ListItemBase::Draw(IRenderTarget* pRenderTarget)
{
    if (m_nFlagsDelayOp)
    {
        UI::UISendMessage(m_pIListItemBase, UI_LISTITEM_MSG_DELAY_OP);
    }

    UISendMessage(m_pIListItemBase, WM_PAINT, (WPARAM)pRenderTarget);
}

void  ListItemBase::DrawItemInnerControl(IRenderTarget* pRenderTarget)
{
#if 0
    if (!m_pPanelRoot || !proc)
        return;

	if (!m_pPanelRoot->GetChildObject())
		return;


	RenderContext context(*proc);
	context.m_bUpdateClip = true;
	context.DrawListItem(m_pIListItemBase);

	m_pPanelRoot->GetImpl()->DrawChildObject(pRenderTarget, context);
	
	{ 
		bool bOld = proc->m_bUpdateClip;
		proc->m_bUpdateClip = true;
		proc->Update(pRenderTarget);  // ��ԭ��������
		proc->m_bUpdateClip = bOld;
	}
#endif

    if (!m_pPanelRoot)
        return;

    if (!m_pPanelRoot->GetChildObject())
        return;

	RECT rc = { 0 };
	GetParentOrFloatRect(&rc);
    pRenderTarget->PushRelativeClipRect(&rc);
    pRenderTarget->OffsetOrigin(rc.left, rc.top);

    m_pPanelRoot->GetImpl()->DrawToLayer__(pRenderTarget);

    pRenderTarget->OffsetOrigin(-rc.left, -rc.top);
    pRenderTarget->PopRelativeClipRect();
}

void  ListItemBase::DrawFloat(IRenderTarget* pRenderTarget)
{
    if (m_nFlagsDelayOp)
    {
        UI::UISendMessage(m_pIListItemBase, UI_LISTITEM_MSG_DELAY_OP);
    }

    UISendMessage(m_pIListItemBase, WM_PAINT, (WPARAM)pRenderTarget);

#if 0
    if (nullptr == m_pPanelRoot || !proc)
        return;
    if (!m_pPanelRoot->GetChildObject())
        return;

    RenderContext context(*proc);
    context.m_bUpdateClip = true;
    context.DrawListItem(m_pIListItemBase);

    m_pPanelRoot->GetImpl()->DrawChildObject(pRenderTarget, context);

    { 
        bool bOld = proc->m_bUpdateClip;
        proc->m_bUpdateClip = true;
        proc->Update(pRenderTarget);  // ��ԭ��������
        proc->m_bUpdateClip = bOld;
    }
#endif
}

// ��item�����Լ���layer���档
void  ListItemBase::Draw2Layer(IRenderTarget* pRenderTarget)
{
	RECT rc = { 0 };
	GetParentOrFloatRect(&rc);

	int xOffset = 0, yOffset = 0;
	xOffset = -rc.left;
	yOffset = -rc.top;

	pRenderTarget->OffsetOrigin(xOffset, yOffset);
	{
		this->Draw(pRenderTarget);
		this->DrawItemInnerControl(pRenderTarget);
	}
	pRenderTarget->OffsetOrigin(-xOffset, -yOffset);
}

void ListItemBase::SetParentRect(LPCRECT prc)
{
    UIASSERT(prc);

    if (IsFloat())
    {
        m_rcParent.CopyRect(prc); 
        return;
    }

    if (!ListCtrlAnimate::HandleItemRectChanged(
            *this, &m_rcParent, prc))
    {
        if (!m_rcParent.EqualRect(prc))
        {
            m_rcParent.CopyRect(prc); 
            m_nFlagsDelayOp |= DELAY_OP_SIZE_CHANGED;  // �ӳٴ��������Ч��
        }
    }
}

void  ListItemBase::SetParentRectLightly(LPCRECT prc)
{
    UIASSERT(prc);
    m_rcParent = *prc;
    m_nFlagsDelayOp |= DELAY_OP_SIZE_CHANGED;  // �ӳٴ��������Ч��
}

void  ListItemBase::OnSize()
{
	m_itemLayer.OnSize(m_rcParent.Width(), m_rcParent.Height());

    BOOL bHandled = FALSE;
    UISendMessage(m_pIListItemBase, WM_SIZE, (WPARAM)0,
        MAKELPARAM(m_rcParent.Width(), m_rcParent.Height()), 0, 0, 0, &bHandled);

    if (bHandled)
        return;

    if (m_pPanelRoot)
    {
        m_pPanelRoot->SetObjectPos(&m_rcParent, SWP_NO_REDRAW);
    }
}


// void  ListItemBase::SetAttribute(IAttributeMap* attribute_map, bool bReload)
// {
//     if (!bReload)
//     {
//         const wchar_t* szText = attribute_map->GetAttr(XML_TOOLTIP, true);
//         if (szText)
//         {
//             SetToolTip(szText);
//         }
//     }
// 
//     attribute_map->GetAttr_RenderBase(XML_ICON_RENDER_PREFIX, XML_RENDER_TYPE, true,
//         m_pListCtrlBase->GetIUIApplication(), m_pListCtrlBase->GetIListCtrlBase(), &m_pIconRender);
// }

void ListItemBase::SetIconRenderType(RENDER_TYPE eType)
{
    SAFE_RELEASE(m_pIconRender);

    // Ϊ�˱�֤�Լ����������ͷŵ�m_pIconRender�����Լ���ģ���д�����
    GetIListCtrlBase()->GetUIApplication()->CreateRenderBase(
        eType, GetIListCtrlBase(), &m_pIconRender);
}
void ListItemBase::SetIconRender(IRenderBase* p)
{
    SAFE_RELEASE(m_pIconRender);
    m_pIconRender = p;

    if (m_pIconRender)
        m_pIconRender->AddRef();
}

void  ListItemBase::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;

	if (m_nConfigWidth != NDEF && m_nConfigHeight != NDEF)
	{
		pSize->cx = m_nConfigWidth;
   	    pSize->cy = m_nConfigHeight;
		return;
	}
	
	ITextRenderBase* pTextRender = m_pListCtrlBase->GetTextRenderDefault();
	if (pTextRender)
	{
		*pSize = pTextRender->GetDesiredSize(GetText());
	}
	if (m_nConfigWidth != NDEF)
		pSize->cx = m_nConfigWidth;

	if (m_nConfigHeight != NDEF)
		pSize->cy = m_nConfigHeight;

	REGION4 r = {0};
	m_pListCtrlBase->GetItemContentPadding(&r);	
	pSize->cx += r.left + r.right;
}


// ��ȡһ��ITEM����Ⱦֵ
long  ListItemBase::OnGetRenderState(unsigned int, WPARAM, LPARAM)
{
	bool bSelected = IsSelected();
	// bool bFocus = IsFocus();
    bool bPress = IsPress();
    bool bHover = IsHover() || IsDragDropHover();
    bool bCtrlFocus = m_pListCtrlBase->IsFocus();

	int nRenderState = 0;

    // TODO: ������ȥ���������Ϣ
	LISTCTRLSTYLE s = {0};
	s.popuplistbox = 1;
    if (m_pListCtrlBase->GetIListCtrlBase()->TestListCtrlStyle(&s))    // �������굯������������Ϊ��ѡ�е���
    {
        if (bHover)
        {
            nRenderState = LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER;
        }
        else if(nullptr == m_pListCtrlBase->GetHoverItem() && IsSelected())
        {
            nRenderState = LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_HOVER;
        }
        return nRenderState;
    }

	if (IsDisable())
	{
		nRenderState =  bSelected ?
					LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_DISABLE :
					LISTCTRLITEM_FOREGND_RENDER_STATE_DISABLE ;
	}
	else if (bPress)
	{
		nRenderState = bSelected ? 
					LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_PRESS :
					LISTCTRLITEM_FOREGND_RENDER_STATE_PRESS ;
	}
	else if (nullptr == m_pListCtrlBase->GetPressItem() && bHover)
	{
		nRenderState = bSelected ?
					LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_HOVER :
					LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER ;
	}
	else
	{
		nRenderState =  bSelected ?
			(bCtrlFocus ? 
					LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NORMAL:
					LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NOT_FOCUS)
						:
					LISTCTRLITEM_FOREGND_RENDER_STATE_NORMAL ;
	}

	return (long)nRenderState;
}

void  ListItemBase::SetIconFromFile(const wchar_t* szIconPath)
{
	UIASSERT(0);
#if 0
    SAFE_RELEASE(m_pIconRender);
    if (nullptr == szIconPath)
        return;

    if (nullptr == m_pListCtrlBase)
        return;

    IApplication*  pUIApplication = GetIListCtrlBase()->GetUIApplication();
    if (nullptr == pUIApplication)
    {
        UIASSERT(0);
        return;
    }

    pUIApplication->CreateRenderBase(
        RENDER_TYPE_IMAGE, 
        GetIListCtrlBase(),
        &m_pIconRender);

    IImageRender*  pImageForeRender = (IImageRender*)m_pIconRender->QueryInterface(__uuidof(IImageRender));

    UI::IRenderBitmap*  pRenderBitmap = nullptr;
    UI::UICreateRenderBitmap(
        GetIListCtrlBase()->GetUIApplication(),
        GetIListCtrlBase()->GetGraphicsRenderLibraryType(), 
        IMAGE_ITEM_TYPE_IMAGE, 
        &pRenderBitmap);
    pRenderBitmap->LoadFromFile(szIconPath, RENDER_BITMAP_LOAD_CREATE_ALPHA_CHANNEL);
    pImageForeRender->SetRenderBitmap(pRenderBitmap);
    pImageForeRender->SetImageDrawType(UI::DRAW_BITMAP_CENTER);
    SAFE_RELEASE(pRenderBitmap);
#endif
}

void  ListItemBase::SetIconFromImageId(const wchar_t* szImageId)
{
    SAFE_RELEASE(m_pIconRender);
    if (nullptr == szImageId)
        return;

    if (nullptr == m_pListCtrlBase)
        return;

    IResourceBundle*  resource_bundle = GetIListCtrlBase()->GetSkinRes();
    if (nullptr == resource_bundle)
    {
        UIASSERT(0);
        return;
    }

    UI::IRenderBitmap*  pRenderBitmap = nullptr;
    resource_bundle->GetImageRes().GetBitmap(
        szImageId, 
        GRAPHICS_RENDER_LIBRARY_TYPE_GDI/*GetIListCtrlBase()->GetGraphicsRenderLibraryType()*/, 
        &pRenderBitmap);
    if (!pRenderBitmap)
        return;

    IApplication* pUIApp = resource_bundle->GetUIApplication();
    pUIApp->CreateRenderBase(
        RENDER_TYPE_IMAGE, 
        GetIListCtrlBase(),
        &m_pIconRender);

    IImageRender*  pImageForeRender = (IImageRender*)m_pIconRender->
        QueryInterface(__uuidof(IImageRender));
    pImageForeRender->SetRenderBitmap(pRenderBitmap);
    pImageForeRender->SetImageDrawType(UI::DRAW_BITMAP_CENTER);
    SAFE_RELEASE(pRenderBitmap);
}

BOOL  ListItemBase::ProcessItemMKMessage(UIMSG* pMSG)
{
    if (nullptr == m_pListCtrlBase)
        return FALSE;

    return m_pListCtrlBase->ProcessItemMKMessage(pMSG, this);
}

int  ListItemBase::CalcDepth()
{
    int i = 0;
    ListItemBase*  pParent = this;
    while (pParent = pParent->m_pParent)
        i++;

    return i;
}
int  ListItemBase::GetIndentByDepth()
{
    int nDepth = 0;
    ListItemBase*  pParent = this;
    while (pParent = pParent->m_pParent)
    {
        if (!pParent->m_itemStyle.bNoChildIndent)
            nDepth++;
    }

    return nDepth * m_pListCtrlBase->GetChildNodeIndent();
}


void  ListItemBase::ModifyStyle(LISTITEMSTYLE* pAdd, LISTITEMSTYLE* pRemove)
{
#define  MODIFY(x) \
    if (pAdd && pAdd->x) \
        m_itemStyle.x = 1; \
    if (pRemove && pRemove->x) \
        m_itemStyle.x = 0;

    MODIFY(bNotSelectable);
    MODIFY(bNotFocusable);
    MODIFY(bOwnerDraw);
    MODIFY(bNoChildIndent);
    MODIFY(bFloat);
	MODIFY(bDelayRemoving);
	MODIFY(bMoveAnimating);
	MODIFY(bLayer);

	if (pAdd && pAdd->bLayer)
		m_itemLayer.CreateLayer();
	if (pRemove && pRemove->bLayer)
		m_itemLayer.TryDestroyLayer();
}

bool  ListItemBase::TestStyle(const LISTITEMSTYLE& s)
{
#define TEST(x) \
    if (s.x && !m_itemStyle.x) return false; 

    TEST(bNotSelectable);
    TEST(bNotFocusable);
    TEST(bOwnerDraw);
    TEST(bNoChildIndent);
    TEST(bFloat);
	TEST(bDelayRemoving);
	TEST(bMoveAnimating);
	TEST(bLayer);

    return true;
}


bool  ListItemBase::IsFloat()
{
    return m_itemStyle.bFloat;
}
void  ListItemBase::SetFloat(bool b)
{
    FloatItemMgr&  mgr = m_pListCtrlBase->GetFloatItemMgr();
    if (b)
        mgr.AddFloatItem(this);
    else
        mgr.RemoveFloatItem(this);
}

void  ListItemBase::SetFloatRect(LPCRECT prc)
{
    FloatItemMgr&  mgr = m_pListCtrlBase->GetFloatItemMgr();
    mgr.SetItemFloatRect(this, prc);
}

void  ListItemBase::GetFloatRect(RECT* prc)
{
    FloatItemMgr&  mgr = m_pListCtrlBase->GetFloatItemMgr();
    mgr.GetItemFloatRect(this, prc);
}
void  ListItemBase::GetParentOrFloatRect(RECT* prc)
{
    if (m_itemStyle.bFloat)
        GetFloatRect(prc);
    else
        GetParentRect(prc);
}

void  ListItemBase::Invalidate()
{
    if (m_pListCtrlBase)
        m_pListCtrlBase->InvalidateItem(this);
}

Layer*  ListItemBase::GetSelfLayer()
{
	return m_itemLayer.GetLayer();
}
// Ϊ����׼��һ��layer�����û���򴴽�layer
Layer*  ListItemBase::GetLayerForAnimate()
{
	Layer* layer = GetSelfLayer();
	if (layer)
		return layer;

	m_itemLayer.CreateLayer();
	return GetSelfLayer();
}

void  ListItemBase::GetVisibleRectInWindow(LPRECT prcRet)
{
	if (nullptr == prcRet || !m_pListCtrlBase)
		return;

	CRect rcItem(m_rcParent);

	int xOffset = 0, yOffset = 0;
	m_pListCtrlBase->GetScrollPos(&xOffset, &yOffset);
	rcItem.OffsetRect(-xOffset, -yOffset);

	CRect rcWindow;
	m_pListCtrlBase->GetClientRectInWindow(&rcWindow);
	rcItem.OffsetRect(rcWindow.left, rcWindow.top);

	IntersectRect(prcRet, &rcItem, &rcWindow);
}
