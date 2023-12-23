#pragma once

// 2015/7/2
//
//  开启float需要做什么
//   
//  1. 列表控件增加float_group_head样式
//
//  2. 列表控件响应 UI_LCN_FIRSTLASTDRAWITEM_UPDATED 消息（见下面的示例）
//
//  3. float list item响应WM_SIZE消息，调整float rect
//
//  4. float list item的onpaint中，需要调用GetParentOrFloatRect进行绘制
//
// 遗留问题
//
//  1. float item必须是不透明的。因为刷新float item时，不好处理float item下面的item的刷新
//
//  2. 目前的浮动组头变化机制是在CalcFirstLastDrawItem中发送一个消息给控件，效率有些低下
//
//
/*
TreeView::~TreeView()
{
    if (m_pFloatGroup)
    {
        m_pFloatGroup->GetIListItemBase()->RemoveDelayRef((void**)&m_pFloatGroup);
        m_pFloatGroup = nullptr;
    }
}
long  TreeView::OnFirstLastDrawItemUpdated(unsigned int, WPARAM, LPARAM)
{
    if (!m_pFirstDrawItem)
        return 0;

    // 逻辑1：
    // 1. 当第一个可见layout为子结点时，查找它的父结点
    // 2. 自己作为父结点，但自己是折叠起来的，忽略 
    // 3. 如果下一个结点也是组结点，则可能需要调用浮动组头的位置
    ListItemBase* pFloatItem = nullptr;

    ListItemBase* pNextVisibleItem = nullptr;
    if (m_pFirstDrawItem)
    {
        pFloatItem = m_pFirstDrawItem->GetAncestorItem();
        
        if (pFloatItem->IsCollapsed())
            pFloatItem = nullptr;

        if (pFloatItem)
            pNextVisibleItem = m_pFirstDrawItem->GetNextVisibleItem();
    }
    
    // 清理现在的float item
    if (m_pFloatGroup && m_pFloatGroup != pFloatItem)
    {
        m_pFloatGroup->SetFloat(false);
        m_pFloatGroup->GetIListItemBase()->RemoveDelayRef((void**)&m_pFloatGroup);
        m_pFloatGroup = nullptr;
    }

    // 设置/更新 float item rect
    if (pFloatItem && pFloatItem->GetItemFlag() == IOfficeSmsSelectContactTreeGroupItem::ITEMFLAG)
    {
        m_pFloatGroup = pFloatItem;
        pFloatItem->GetIListItemBase()->AddDelayRef((void**)&m_pFloatGroup);

        CRect rcItem;
        pFloatItem->GetParentRect(rcItem);
        CRect rcFloat(0,0, rcItem.Width(), rcItem.Height());

        // 下一个也是一个组，当前这个浮动组头要往上走，不能遮住了这个组
        if (pNextVisibleItem && pNextVisibleItem->GetItemFlag() == IOfficeSmsSelectContactTreeGroupItem::ITEMFLAG)
        {
            RECT rcNext;
            pNextVisibleItem->GetParentRect(&rcNext);
            ItemRect2ObjectRect(&rcNext, &rcNext);

            if (rcNext.top < rcFloat.Height())
            {
                rcFloat.OffsetRect(0, rcNext.top - rcFloat.Height());
            }
        }

        pFloatItem->SetFloatRect(&rcFloat);
    }
    
    return 0;
}
*/

namespace ui
{
class ListItemBase;
class ListCtrlBase;

struct FloatItemData
{
    ListItemBase*  pItem;

    // 将浮动坐标放在这里存储,因为不是每个ITEM都需要.
    RECT  rcFloat;
    // 浮层z序，超大的在上面
    long  zOrder;  
};

class FloatItemMgr
{
public:
    FloatItemMgr();
    ~FloatItemMgr();
    void  SetListCtrlBase(ListCtrlBase* p);

    void  OnRemoveItem(ListItemBase* pItem);
    void  OnRemoveAllItem();

    void  AddFloatItem(ListItemBase* p);
    void  RemoveFloatItem(ListItemBase* p);
    void  SetItemFloatRect(ListItemBase* pItem, LPCRECT prcFloat);
    bool  GetItemFloatRect(ListItemBase* pItem, RECT* prcFloat);

    void  DoPaint(IRenderTarget* pRenderTarget);
    ListItemBase*  HitTest(POINT ptWindow);

private:
    FloatItemData&  add_float_item(ListItemBase* pItem);

private:
    typedef map<ListItemBase*,  FloatItemData> _map;
    typedef map<ListItemBase*,  FloatItemData>::iterator _mapiter;
    map<ListItemBase*,  FloatItemData>  m_mapFloatItems;

    ListCtrlBase*  m_pListCtrl;
};

}