#include "stdafx.h"
#include "listitemlayout.h"
#include "src/UIObject\ListCtrl\listctrlbase.h"
#include "src/UIObject\ListCtrl\ListItemBase\listitembase.h"

namespace ui
{
ListCtrlLayoutMgr::ListCtrlLayoutMgr()
{
    m_pListCtrl = nullptr;
    m_pLayout = nullptr;
}
ListCtrlLayoutMgr::~ListCtrlLayoutMgr() 
{
    SAFE_RELEASE(m_pLayout);
};

void  ListCtrlLayoutMgr::SetListCtrlBase(ListCtrlBase* pListCtrlBase) 
{
    m_pListCtrl = pListCtrlBase; 
    if (m_pLayout)
        m_pLayout->SetIListCtrlBase(m_pListCtrl->GetIListCtrlBase());
}
IListCtrlLayout*  ListCtrlLayoutMgr::GetLayout()
{
    return m_pLayout;
}
void  ListCtrlLayoutMgr::SetLayout(IListCtrlLayout* p) 
{
    SAFE_RELEASE(m_pLayout);
    m_pLayout = p;
}

//	更新从pStart开始后面的每一个item的 m_rcParent，比如新插入一个pItem
//  该函数仅更新每一个item的rect，不管滚动条的显隐
bool  ListCtrlLayoutMgr::Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent) 
{
    if (nullptr == m_pLayout)
        return false;

    m_pLayout->Arrange(pStartToArrange, pSizeContent);
    return true;
}

void  ListCtrlLayoutMgr::Measure(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;
    if (nullptr == m_pLayout || nullptr == m_pListCtrl)
        return;

    m_pLayout->Measure(pSize);

    REGION4 rcNonClient = {0};
    m_pListCtrl->GetNonClientRegion(&rcNonClient);

    pSize->cx += rcNonClient.left + rcNonClient.right;
    pSize->cy += rcNonClient.top + rcNonClient.bottom;

    *pSize = m_pListCtrl->GetAdaptWidthHeight(pSize->cx, pSize->cy);
}

ListCtrlItemSimpleLayout::ListCtrlItemSimpleLayout()
{
    m_pIListCtrlBase = nullptr;
}
void ListCtrlItemSimpleLayout::Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent)
{
    int  nItemHeight = m_pIListCtrlBase->GetItemHeight();
    CRect  rcClient;
    CRect  rcItem;

    m_pIListCtrlBase->GetClientRectInObject(&rcClient);

    int  nWidth = rcClient.Width();
    int  yTop = 0;
    int  yPrevItemBottom = 0;
    int  nVSpace = m_pIListCtrlBase->GetVertSpacing();

    IListItemBase* pIItem = m_pIListCtrlBase->FindVisibleItemFrom(pStartToArrange); // m_pListCtrl->GetFirstItem();
    if (nullptr == pIItem)
        return;

    ListItemBase* pItem = pIItem->GetImpl();
    ListItemBase* pPrevItem = pItem->GetPrevVisibleItem();
    if (nullptr == pPrevItem)   // 第一个
    {
        CRect rc(0, 0, nWidth, nItemHeight);
        pItem->SetParentRect(&rc);

        yPrevItemBottom = rc.bottom;
        pItem = pItem->GetNextVisibleItem();
    }
    else
    {
        yPrevItemBottom = pPrevItem->GetParentRectPtr()->bottom;
    }

    while (pItem)
    {
        yTop = yPrevItemBottom + nVSpace;
        yPrevItemBottom = yTop + nItemHeight;

        rcItem.SetRect(0, yTop, nWidth ,yPrevItemBottom);
        pItem->SetParentRect(&rcItem);
        pItem = pItem->GetNextVisibleItem();
    }

    if (pSizeContent)
    {
        pSizeContent->cx = nWidth;
        pSizeContent->cy = yPrevItemBottom;
    }
}

void ListCtrlItemSimpleLayout::Measure(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;

    int   nVisibleCount = 0;
    SIZE  sizeItem = {0, 0};
    int   nMaxWidth = 0;
    int   nItemHeight = m_pIListCtrlBase->GetItemHeight();

    IListItemBase* pItem = m_pIListCtrlBase->FindVisibleItemFrom(nullptr);
    while (pItem)
    {
        pItem->GetDesiredSize(&sizeItem);
        if (sizeItem.cx > nMaxWidth)
            nMaxWidth = sizeItem.cx;
    
        nVisibleCount ++;
        pItem = pItem->GetNextVisibleItem();
    }

    pSize->cx = nMaxWidth;
    pSize->cy = nVisibleCount*nItemHeight;

    // 加上行间隙
    if (nVisibleCount > 1)
        pSize->cy += (nVisibleCount-1) * m_pIListCtrlBase->GetVertSpacing();

}

//////////////////////////////////////////////////////////////////////////
ListCtrlItemVariableHeightLayout::ListCtrlItemVariableHeightLayout()
{
    m_pIListCtrlBase = nullptr;
}

void  ListCtrlItemVariableHeightLayout::Measure(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;

    int   nVisibleCount = 0;
    SIZE  sizeItem = {0, 0};
    int   nMaxWidth = 0;
    int   nItemHeight = m_pIListCtrlBase->GetItemHeight();

    IListItemBase* pItem = m_pIListCtrlBase->FindVisibleItemFrom(nullptr);
    while (pItem)
    {
        pItem->GetDesiredSize(&sizeItem);
        if (sizeItem.cx > nMaxWidth)
            nMaxWidth = sizeItem.cx;

        pSize->cy += sizeItem.cy;

        nVisibleCount ++;
        pItem = pItem->GetNextVisibleItem();
    }

    pSize->cx = nMaxWidth;

    // 加上行间隙
    if (nVisibleCount > 1)
        pSize->cy += (nVisibleCount-1) * m_pIListCtrlBase->GetVertSpacing();
}

void ListCtrlItemVariableHeightLayout::Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent)
{
    int  nItemHeight = m_pIListCtrlBase->GetItemHeight();

    CRect  rcItem;
    CRect  rcClient;
    m_pIListCtrlBase->GetClientRectInObject(&rcClient);

    int  nWidth = rcClient.Width();
    int  nMaxDisiredWidth = 0; // m_pListCtrl->GetMaxDesiredWidth(nullptr);
    {
        SIZE  sizeItem = {0, 0};
        IListItemBase* pItem = m_pIListCtrlBase->FindVisibleItemFrom(nullptr);
        while (pItem)
        {
            UISendMessage(pItem, UI_MSG_GETDESIREDSIZE, (WPARAM)&sizeItem);
            if (sizeItem.cx > nMaxDisiredWidth)
                nMaxDisiredWidth = sizeItem.cx;

            pItem = pItem->GetNextVisibleItem();
        }
    }

    if (nWidth < nMaxDisiredWidth)
        nWidth = nMaxDisiredWidth;

    int  yTop = 0;
    int  yPrevItemBottom = 0;
    int  nVSpace = m_pIListCtrlBase->GetVertSpacing();

    IListItemBase* pIItem = m_pIListCtrlBase->FindVisibleItemFrom(pStartToArrange); // m_pListCtrl->GetFirstItem();
    if (nullptr == pIItem)
        return;

    ListItemBase* pItem = pIItem->GetImpl();
    ListItemBase* pPrevItem = pItem->GetPrevVisibleItem();
    if (nullptr == pPrevItem)   // 第一个
    {
        SIZE s = {0};
        pItem->GetDesiredSize(&s);

        CRect rc(0, 0, nWidth, s.cy);
        pItem->SetParentRect(&rc);

        yPrevItemBottom = rc.bottom;
        pItem = pItem->GetNextVisibleItem();
    }
    else
    {
        yPrevItemBottom = pPrevItem->GetParentRectPtr()->bottom;
    }

    while (pItem)
    {
        SIZE s = {0};
        pItem->GetDesiredSize(&s);

        yTop = yPrevItemBottom + nVSpace;
        yPrevItemBottom = yTop + s.cy;

        rcItem.SetRect(0, yTop, nWidth ,yPrevItemBottom);
        pItem->SetParentRect(&rcItem);

        pItem = pItem->GetNextVisibleItem();
    }

    if (pSizeContent)
    {
        pSizeContent->cx = nWidth;
        pSizeContent->cy = yPrevItemBottom;
    }
}

//////////////////////////////////////////////////////////////////////////

void  ListCtrlItemHorzAverageLayout::Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent)
{
    CRect  rcClient;
    CRect  rcItem;

    m_pIListCtrlBase->GetClientRectInObject(&rcClient);

    int  nTotalWidth = rcClient.Width();
    int  nItemCount = m_pIListCtrlBase->GetItemCount();

    if (nItemCount <= 0)
        return;

    int  nItemWidth = nTotalWidth/nItemCount;
    int  xCursor = 0;

    IListItemBase* pIItem = m_pIListCtrlBase->FindVisibleItemFrom(pStartToArrange);
    if (nullptr == pIItem)
        return;    

    ListItemBase* pItem = pIItem->GetImpl();
    ListItemBase* pPrevItem = pItem->GetPrevVisibleItem();
    if (pPrevItem)  
        xCursor = pPrevItem->GetParentRectRefPtr()->right;

    rcItem.top = 0;
    rcItem.bottom = rcClient.bottom;
    rcItem.left = xCursor;

    while (pItem)
    {
        rcItem.left = xCursor;
        rcItem.right = xCursor + nItemWidth;
        pItem->SetParentRect(&rcItem);
        pItem = pItem->GetNextVisibleItem();

        xCursor += nItemWidth;
    }

    if (pSizeContent)
    {
        pSizeContent->cx = nItemCount*nItemWidth;
        pSizeContent->cy = rcClient.Height();
    }
}

//////////////////////////////////////////////////////////////////////////

ListCtrlItemFixHeightFlowLayout::ListCtrlItemFixHeightFlowLayout()
{
	m_nItemSpace = -1;
	m_nLineSpace = -1;
}

void  ListCtrlItemFixHeightFlowLayout::SetLineSpace(int n)
{
	m_nLineSpace = n;
}
void  ListCtrlItemFixHeightFlowLayout::SetItemSpace(int n)
{
	m_nItemSpace = n;
}
int  ListCtrlItemFixHeightFlowLayout::GetLineSpace()
{
	if (m_nLineSpace < 0)
	{
		m_nLineSpace = (int)UISendMessage(
			m_pIListCtrlBase, 
			UI_LCM_LAYOUT_GET_LINESPACE, 0,0,0,0,UI_MSGMAPID_LC_LAYOUT);
	}

	return m_nLineSpace;
}
int  ListCtrlItemFixHeightFlowLayout::GetItemSpace()
{
	if (m_nItemSpace < 0)
	{
		m_nItemSpace = (int)UISendMessage(
			m_pIListCtrlBase, 
			UI_LCM_LAYOUT_GET_ITEMSPACE, 0,0,0,0,UI_MSGMAPID_LC_LAYOUT);
	}

	return m_nItemSpace;
}


void  ListCtrlItemFixHeightFlowLayout::Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent)
{
    CRect  rcClient;
    CRect  rcItem;

    m_pIListCtrlBase->GetClientRectInObject(&rcClient);
    int  nCtrlWidth = rcClient.Width();
    int  nItemHeight = m_pIListCtrlBase->GetItemHeight();
    int  xCursor = 0;
    int  yCursor = 0;

	GetItemSpace();
	GetLineSpace();

    int  nLastWidth = 0;
    int  nLastHeight = 0;

    IListItemBase* pIItem = m_pIListCtrlBase->FindVisibleItemFrom(pStartToArrange);
    if (nullptr == pIItem)
        return;    

    ListItemBase* pItem = pIItem->GetImpl();
    ListItemBase* pPrevItem = pItem->GetPrevVisibleItem();
    if (pPrevItem) 
    {
        xCursor = pPrevItem->GetParentRectRefPtr()->right + m_nItemSpace;
        yCursor = pPrevItem->GetParentRectRefPtr()->top;
    }

    while (pItem)
    {
        SIZE sizeItem = {0};
        pItem->GetIListItemBase()->GetDesiredSize(&sizeItem);

        if (xCursor != 0)  // 不是该行第一个，加上间距
            xCursor += m_nItemSpace;

        if (sizeItem.cx > (nCtrlWidth-xCursor) && xCursor != 0)  // 小于本行剩余宽度，换行
        {
            yCursor += nItemHeight + m_nLineSpace;
            xCursor = 0;
        }

        rcItem.left = xCursor;
        rcItem.right = rcItem.left + sizeItem.cx;
        rcItem.top = yCursor;
        rcItem.bottom = yCursor + nItemHeight;

        pItem->SetParentRect(&rcItem);
        pItem = pItem->GetNextVisibleItem();

        if (rcItem.right > nLastWidth)
            nLastWidth = rcItem.right;
        if (rcItem.bottom > nLastHeight)
            nLastHeight = rcItem.bottom;

        xCursor += sizeItem.cx;
    }

    if (pSizeContent)
    {
        pSizeContent->cx = nLastWidth;
        pSizeContent->cy = nLastHeight;
    }
}

void  ListCtrlItemFixHeightFlowLayout::Measure(SIZE* pSize)
{
	int nMaxWidth = m_pIListCtrlBase->GetMaxWidth();
	if (nMaxWidth < 0)
		nMaxWidth = 0x7FFFFFFF;

	int  xCursor = 0;
	int  yCursor = 0;

	int  nItemSpace = GetItemSpace();
	int  nLineSpace = GetLineSpace();

	int  nLastWidth = 0;
	int  nLastHeight = 0;

	ListItemBase* pItem = 
		m_pIListCtrlBase->GetImpl()->FindVisibleItemFrom(nullptr);
	while (pItem)
	{
		SIZE sizeItem = {0};
		pItem->GetIListItemBase()->GetDesiredSize(&sizeItem);

		if (xCursor != 0)  // 不是该行第一个，加上间距
			xCursor += nItemSpace;

		if (yCursor == 0)
			yCursor = sizeItem.cy;

		if (sizeItem.cx > (nMaxWidth-xCursor) && xCursor != 0)  // 小于本行剩余宽度，换行
		{
			yCursor += sizeItem.cy + m_nLineSpace;

			nLastWidth = max(nLastWidth, xCursor);
			xCursor = sizeItem.cx;
		}
		else
		{
			xCursor += sizeItem.cx;
			nLastWidth = max(nLastWidth, xCursor);
		}

		pItem = pItem->GetNextVisibleItem();
	}

	pSize->cx = nLastWidth;
	pSize->cy = yCursor;
}

void  ListCtrlItemVariableHeightFlowLayout::CurrentLineItems::AddItem(ListItemBase* p, int w, int h)
{
    if (m_nxCursor + w > m_nCtrlWidth && m_nxCursor != 0)
    {
        CommitLine();
    }

    ListItemData item = {0};
    item.pItem = p;
    item.x = m_nxCursor;
    item.w = w;

    m_vecItems.push_back(item);
    m_nxCursor += w + m_hSpace;
    if (h > m_nMaxHeight)
        m_nMaxHeight = h;
}

void  ListCtrlItemVariableHeightFlowLayout::CurrentLineItems::AddSingleLineItem(ListItemBase* pItem, int h)
{
    this->CommitLine();

    RECT rc = {0, m_nyCursor, m_nCtrlWidth, m_nyCursor + h};
    pItem->SetParentRect(&rc);

    m_nyCursor += h + m_vSpace;
    m_nxCursor = 0;
}

// in: 当前行的起始高度
// out: 返回下一行的超始高度
void ListCtrlItemVariableHeightFlowLayout::CurrentLineItems::CommitLine()
{
    size_t  size = m_vecItems.size();
    if (size <= 0)
        return;

    m_nContentHeight = m_nyCursor+m_nMaxHeight;

    for (size_t i = 0; i < size; i++)
    {
        ListItemData& item = m_vecItems[i];
        RECT rc = {item.x, m_nyCursor, item.x+item.w, m_nContentHeight};
        item.pItem->SetParentRect(&rc);
    }

    m_vecItems.clear();
    m_nyCursor += m_nMaxHeight + m_vSpace;
    m_nxCursor = 0;
    m_nMaxHeight = 0;
}

void  ListCtrlItemVariableHeightFlowLayout::Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent)
{
    CRect  rcClient;
    m_pIListCtrlBase->GetObjectClientRect(&rcClient);
    int  nItemSpace = (int)UISendMessage(m_pIListCtrlBase, UI_LCM_LAYOUT_GET_ITEMSPACE, 0,0,0,0,UI_MSGMAPID_LC_LAYOUT);
    int  nLineSpace = (int)UISendMessage(m_pIListCtrlBase, UI_LCM_LAYOUT_GET_LINESPACE, 0,0,0,0,UI_MSGMAPID_LC_LAYOUT);

    CurrentLineItems  lineop;   // 记录一行的信息
    lineop.SetHSpace(nItemSpace);
    lineop.SetVSpace(nLineSpace);
    lineop.SetCtrlWidth(rcClient.Width());

    IListItemBase* pIItem = m_pIListCtrlBase->FindVisibleItemFrom(pStartToArrange);
    if (nullptr == pIItem)
        return;    

    ListItemBase* pItem = pIItem->GetImpl();
    ListItemBase* pPrevItem = pItem->GetPrevVisibleItem();
    if (pPrevItem) 
    {
        RECT rc;
        pPrevItem->GetParentRect(&rc);
        lineop.SetXYCursor(rc.right+nItemSpace, rc.top);
    }

    while (pItem)
    {
        SIZE sizeItem = {0};
        pItem->GetIListItemBase()->GetDesiredSize(&sizeItem);

        //         if (sizeItem.cx == UI_LISTCTRL_LAYOUT_HORZ_REMAINLINE)  // 填满当前行
        //         {
        //             UIASSERT(0 && "todo:");
        //         }
        //         else 
        if (sizeItem.cx == UI_LISTCTRL_LAYOUT_HORZ_SINGLELINE)  // 另起一行，填满
        {
            lineop.AddSingleLineItem(pItem, sizeItem.cy);
        }
        else
        {
            lineop.AddItem(pItem, sizeItem.cx, sizeItem.cy);
        }

        pItem = pItem->GetNextVisibleItem();
    }

    // 剩余部分
    lineop.CommitLine();

    lineop.GetContentSize(pSizeContent);
}

void  ListCtrlItemVariableHeightFlowLayout::Measure(SIZE* pSize)
{
    UIASSERT(0);
}

//////////////////////////////////////////////////////////////////////////

IListCtrlLayout*  CreateListCtrlLayout(int nType, IListCtrlBase* p)
{
    IListCtrlLayout* pLayout = nullptr;
    switch (nType)
    {
    case LISTCTRL_ITEM_LAYOUT_TYPE_1:
        {
            pLayout = new ListCtrlItemSimpleLayout();
            pLayout->SetIListCtrlBase(p);
        }
        break;

    case LISTCTRL_ITEM_LAYOUT_TYPE_2:
        {
            pLayout = new ListCtrlItemVariableHeightLayout();
            pLayout->SetIListCtrlBase(p);
        }
        break;
    }

    return pLayout;
}

}