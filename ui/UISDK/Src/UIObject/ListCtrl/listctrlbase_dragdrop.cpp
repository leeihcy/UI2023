#include "stdafx.h"
#include "listctrlbase.h"
#include "Inc\Interface\ilistitembase.h"
#include "ListItemBase\listitembase.h"
using namespace UI;

// 拖拽过程中，鼠标下面的列表项。用于绘制高亮状态
// 默认同一时间只有一个拖拽操作
// 默认每次拖拽完后后，g_pItemUnderDrag将恢复为nullptr
ListItemBase*  g_pItemUnderDrag = nullptr;  

void  ListCtrlBase::ListItemDragDropEvent(
    UI::DROPTARGETEVENT_TYPE eEvent, ListItemBase* pItem)
{
    UIMSG  msg;
    msg.message = UI_LISTITEM_MSG_DRAGDROP_EVENT;
    
    if (eEvent == UI::_DragLeave)
    {
        if (g_pItemUnderDrag)
        {
            g_pItemUnderDrag->SetDragDropHover(false);
            g_pItemUnderDrag->Invalidate();

            msg.pMsgTo = g_pItemUnderDrag->GetIListItemBase();
            msg.wParam = (WPARAM)eEvent;
            UISendMessage(&msg);
            
            g_pItemUnderDrag = nullptr;
        }
        return;
    }

    if (g_pItemUnderDrag != pItem && g_pItemUnderDrag)
    {
        g_pItemUnderDrag->SetDragDropHover(false);
        g_pItemUnderDrag->Invalidate();

        msg.pMsgTo = g_pItemUnderDrag->GetIListItemBase();
        msg.wParam = UI::_DragLeave;
        UISendMessage(&msg);
    }

    if (g_pItemUnderDrag)
        g_pItemUnderDrag->RemoveDelayRef((void**)&g_pItemUnderDrag);

    g_pItemUnderDrag = pItem;

    if (g_pItemUnderDrag)
        g_pItemUnderDrag->AddDelayRef((void**)&g_pItemUnderDrag);

    if (g_pItemUnderDrag)
    {
        bool bOld = g_pItemUnderDrag->IsDragDropHover();

        if (eEvent == UI::_Drop)
            g_pItemUnderDrag->SetDragDropHover(false);
        else
            g_pItemUnderDrag->SetDragDropHover(true);

        if (bOld != g_pItemUnderDrag->IsDragDropHover())
        {
            g_pItemUnderDrag->Invalidate();
        }

        msg.pMsgTo = g_pItemUnderDrag->GetIListItemBase();
        msg.wParam = eEvent;
        UISendMessage(&msg);
     
        if (eEvent == UI::_Drop)
            g_pItemUnderDrag = nullptr;
    }
}

// 拖拽过程中，拖拽到顶部或者底部时，滚动列表
void  ListCtrlBase::ListCtrlDragScroll()
{
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(GetHWND(), &pt);

    POINT ptObject;
    WindowPoint2ObjectPoint(&pt, &ptObject, true);

    CRect rcClient;
    GetClientRectInObject(&rcClient);
    rcClient.DeflateRect(10,10,10,10);

    bool bNeedScroll = true;
    if (ptObject.y < rcClient.top)
    {
        this->ScrollY(-10, true);
    }
    else if (ptObject.y > rcClient.bottom)
    {
        this->ScrollY(10, true);
    }
    else
    {
        bNeedScroll = false;
    }
}