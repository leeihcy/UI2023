#include "stdafx.h"
#include "include/interface/ilistctrlbase.h"
#include "src/UIObject\ListCtrl\listctrlbase.h"
#include "include/interface/ilistitembase.h"
#include "src/UIObject\ListCtrl\ListItemBase\listitembase.h"
#include "CustomItem\custom_listitem.h"

namespace ui
{

UI_IMPLEMENT_INTERFACE(ListCtrlBase, Control)
UI_IMPLEMENT_INTERFACE(ListItemShareData, Message)

bool  IListCtrlBase::AddItem(IListItemBase* pItem)              
{ 
    if (!pItem)
        return false;

    return __pImpl->AddItem(pItem->GetImpl()); 
}

bool  IListCtrlBase::InsertItem(
            IListItemBase* pItem, 
            int nPos) 
{ 
    if (nullptr == pItem)
        return false;

    return __pImpl->InsertItem(pItem->GetImpl(), nPos); 
}

bool  IListCtrlBase::InsertItem(
        IListItemBase* pItem, 
        IListItemBase* pInsertAfter) 
{ 
    if (nullptr == pItem)
        return false;

    ListItemBase*  pAfter = nullptr;
    if (pInsertAfter)
        pAfter = pInsertAfter->GetImpl();

    return __pImpl->InsertItem(pItem->GetImpl(), pAfter); 
}

bool  IListCtrlBase::InsertItem(
            IListItemBase* pItem, 
            IListItemBase* pParent, 
            IListItemBase* pInsertAfter)
{ 
    if (nullptr == pItem)
        return false;

    return __pImpl->InsertItem(pItem->GetImpl(), pParent, pInsertAfter); 
}

ICustomListItem*  IListCtrlBase::InsertCustomItem(
                    const wchar_t*  szLayoutName,
                    long lId,
                    IListItemBase* pParent, 
                    IListItemBase* pInsertAfter)
{
    CustomListItem* p = __pImpl->InsertCustomItem(
            szLayoutName, lId, pParent, pInsertAfter);
    
    if (p)
        return p->GetICustomListItem();

    return nullptr;
}

ICustomListItem*  IListCtrlBase::GetCustomListItem(IObject* p)
{
	if (!p)
		return nullptr;

	CustomListItem* pItem = __pImpl->GetCustomListItem(p->GetImpl());
	if (pItem)
		return pItem->GetICustomListItem();

	return nullptr;
}

void  IListCtrlBase::RemoveItem(IListItemBase* pItem)             
{ 
    if (nullptr == pItem)
        return ;

    __pImpl->RemoveItem(pItem->GetImpl()); 
}   
void  IListCtrlBase::RemoveItemFromTree(IListItemBase* pItem)
{
	if (nullptr == pItem)
		return;

	__pImpl->RemoveItemFromTree(pItem->GetImpl());
}
void  IListCtrlBase::RemoveAllChildItems(IListItemBase* pParent)
{
    if (nullptr == pParent)
        return;

    __pImpl->RemoveAllChildItems(pParent->GetImpl());
}

void  IListCtrlBase::RemoveItem(int nIndex)            
{
    __pImpl->RemoveItem(nIndex); 
}
void  IListCtrlBase::RemoveAllItem()                      
{
    __pImpl->RemoveAllItem(); 
}
void  IListCtrlBase::DelayRemoveItem(IListItemBase* pItem)
{ 
    if (nullptr == pItem)
        return;

    __pImpl->DelayRemoveItem(pItem->GetImpl());
}

bool  IListCtrlBase::MoveItem(
        IListItemBase* p,
        IListItemBase* pNewParent, 
        IListItemBase* pInsertAfter)
{
	if (!p)
		return false;

	return __pImpl->MoveItem(p->GetImpl(), pNewParent, pInsertAfter);
}

void  IListCtrlBase::ModifyListCtrlStyle(LISTCTRLSTYLE* add, LISTCTRLSTYLE* remove)
{
	__pImpl->ModifyListCtrlStyle(add, remove);
}
bool  IListCtrlBase::TestListCtrlStyle(LISTCTRLSTYLE* test)
{
	return __pImpl->TestListCtrlStyle(test);
}

int  IListCtrlBase::GetItemCount()                                        
{
    return __pImpl->GetItemCount(); 
}
int  IListCtrlBase::GetVisibleItemCount()                                 
{
    return __pImpl->GetVisibleItemCount(); 
}
void  IListCtrlBase::CalcFirstLastVisibleItem()                           
{
    __pImpl->CalcFirstLastDrawItem(); 
} 
void  IListCtrlBase::SetCalcFirstLastVisibleItemFlag()                   
{
    __pImpl->SetCalcFirstLastDrawItemFlag(); 
}


void  IListCtrlBase::ToggleItemExpand(IListItemBase* pItem, bool bUpdate) 
{ 
    if (!pItem)
        return;
    __pImpl->ToggleItemExpand(pItem->GetImpl(), bUpdate);
}
void  IListCtrlBase::CollapseItem(IListItemBase* pItem, bool bUpdate)     
{
    if (!pItem)
        return;
    __pImpl->CollapseItem(pItem->GetImpl(), bUpdate);
}
void  IListCtrlBase::ExpandItem(IListItemBase* pItem, bool bUpdate)      
{ 
    if (!pItem)
        return;
    __pImpl->ExpandItem(pItem->GetImpl(), bUpdate); 
}
void  IListCtrlBase::CollapseAll(bool bUpdate)
{
    __pImpl->CollapseAll(bUpdate);
}
void  IListCtrlBase::ExpandAll(bool bUpdate)
{
    __pImpl->ExpandAll(bUpdate);
}


IListItemBase*  IListCtrlBase::GetHoverItem()           
{ 
    ListItemBase* p = __pImpl->GetHoverItem(); 
    if (p)
        return p->GetIListItemBase();

    return nullptr;
}
IListItemBase*  IListCtrlBase::GetPressItem()           
{ 
    ListItemBase* p = __pImpl->GetPressItem(); 
    if (p)
        return p->GetIListItemBase();

    return nullptr;
}
void  IListCtrlBase::SetHoverItem(IListItemBase* pItem)
{
    if (pItem)
        __pImpl->SetHoverItem(pItem->GetImpl());
    else
        __pImpl->SetHoverItem(nullptr);
}
void  IListCtrlBase::SetPressItem(IListItemBase* pItem)
{
    if (pItem)
        __pImpl->SetPressItem(pItem->GetImpl());
    else
        __pImpl->SetPressItem(nullptr);
}
IListItemBase*  IListCtrlBase::GetFocusItem()           
{ 
    ListItemBase* p = __pImpl->GetFocusItem(); 
    if (p)
        return p->GetIListItemBase();

    return nullptr;
}
void  IListCtrlBase::SetFocusItem(IListItemBase* pItem) 
{
    if (pItem)
        __pImpl->SetFocusItem(pItem->GetImpl()); 
    else
        __pImpl->SetFocusItem(nullptr); 
}
IListItemBase*  IListCtrlBase::GetEditingItem()
{
	ListItemBase* p = __pImpl->GetEditingItem(); 
	if (p)
		return p->GetIListItemBase();

	return nullptr;
}
void  IListCtrlBase::SetEditingItem(IListItemBase* pItem)
{
	if (pItem)
		__pImpl->SetEditingItem(pItem->GetImpl()); 
	else
		__pImpl->SetEditingItem(nullptr); 
}

IObject*  IListCtrlBase::GetHoverObject()                                
{ 
    Object* p = __pImpl->GetHoverObject();
    if (p)
        return p->GetIObject();

    return nullptr;
}
IObject*  IListCtrlBase::GetPressObject()                                 
{ 
    Object* p = __pImpl->GetPressObject(); 
    if (p)
        return p->GetIObject();

    return nullptr;
}
void  IListCtrlBase::SetFocusObject(IObject* pObj)
{   
    __pImpl->SetFocusObject(pObj?pObj->GetImpl():nullptr);
}
IObject*  IListCtrlBase::GetFocusObject()
{
    Object* p = __pImpl->GetFocusObject(); 
    if (p)
        return p->GetIObject();

    return nullptr;
}

IListItemBase*  IListCtrlBase::GetItemByWindowPoint(POINT ptWindow)
{ 
    ListItemBase* p = __pImpl->GetItemByWindowPoint(ptWindow); 
    if (p)        
        return p->GetIListItemBase();
    else
        return nullptr;
}
IScrollBarManager*  IListCtrlBase::GetIScrollBarMgr()                    
{ 
    return __pImpl->GetIScrollBarMgr();
}
void  IListCtrlBase::SetSortCompareProc(ListItemCompareProc p)           
{ 
    __pImpl->SetSortCompareProc(p); 
}
void  IListCtrlBase::Sort()
{ 
    __pImpl->Sort(); 
}
void  IListCtrlBase::SwapItemPos(IListItemBase*  p1, IListItemBase* p2) 
{ 
    if (nullptr == p1 || nullptr == p2)
        return;

    __pImpl->SwapItemPos(p1->GetImpl(), p2->GetImpl()); 
}
void  IListCtrlBase::ItemRect2WindowRect(LPCRECT prc, LPRECT prcRet)
{ 
    __pImpl->ItemRect2WindowRect(prc, prcRet);
}
void  IListCtrlBase::WindowPoint2ItemPoint(IListItemBase* pItem, const POINT* pt, POINT* ptRet)
{
	if (!pItem)
		return;

	__pImpl->WindowPoint2ItemPoint(pItem->GetImpl(), pt, ptRet);
}

IListItemBase*  IListCtrlBase::GetItemByPos(int i, bool bVisibleOnly) 
{ 
    ListItemBase* p = __pImpl->GetItemByPos(i, bVisibleOnly); 
    if (p)
        return p->GetIListItemBase();
    return nullptr;
}
int  IListCtrlBase::GetItemPos(IListItemBase* p, bool bVisibleOnly)
{
    return __pImpl->GetItemPos(p?p->GetImpl():nullptr, bVisibleOnly);
}
IListItemBase*  IListCtrlBase::GetItemUnderCursor()
{
    ListItemBase* p = __pImpl->GetItemUnderCursor(); 
    if (p)
        return p->GetIListItemBase();
    return nullptr;
}
IListItemBase*  IListCtrlBase::GetItemById(long lId)
{
    ListItemBase* p = __pImpl->GetItemById(lId); 
    if (p)
        return p->GetIListItemBase();
    return nullptr;
}
IListItemBase*  IListCtrlBase::FindItemByText(const wchar_t* szText, IListItemBase* pStart)
{
    ListItemBase* pStartImpl = nullptr;
    if (pStart)
        pStartImpl = pStart->GetImpl();

    ListItemBase* p = __pImpl->FindItemByText(szText, pStartImpl);
    if (p)
        return p->GetIListItemBase();
    return nullptr;
}
IListItemBase*  IListCtrlBase::FindChildItemByText(const wchar_t* szText, 
                                                   IListItemBase* pParent,
                                                   IListItemBase* pStart)
{
    ListItemBase* pStartImpl = nullptr;
    ListItemBase* pParentImpl = nullptr;
    if (pStart)
        pStartImpl = pStart->GetImpl();
    if (pParent)
        pParentImpl = pParent->GetImpl();

    ListItemBase* p = __pImpl->FindChildItemByText(szText, pParentImpl, pStartImpl);
    if (p)
        return p->GetIListItemBase();
    return nullptr;
}
IListItemBase*  IListCtrlBase::GetFirstItem()     
{ 
    ListItemBase* p = __pImpl->GetFirstItem(); 
    if (p)
        return p->GetIListItemBase();
    return nullptr;
}
IListItemBase*  IListCtrlBase::GetLastItem()      
{ 
    ListItemBase* p = __pImpl->GetLastItem(); 
    if (p)
        return p->GetIListItemBase();
    return nullptr;
}
IListItemBase*  IListCtrlBase::GetFirstDrawItem() 
{ 
    ListItemBase* p = __pImpl->GetFirstDrawItem(); 
    if (p)
        return p->GetIListItemBase();
    return nullptr;
}
IListItemBase*  IListCtrlBase::GetLastDrawItem()  
{
    ListItemBase* p = __pImpl->GetLastDrawItem(); 
    if (p)
        return p->GetIListItemBase();
    return nullptr;
}
IListItemBase*  IListCtrlBase::FindVisibleItemFrom(IListItemBase* pFindFrom) 
{ 
    ListItemBase* p = __pImpl->FindVisibleItemFrom(pFindFrom ? pFindFrom->GetImpl() : nullptr); 
    if (p)
        return p->GetIListItemBase();
    return nullptr;
}
IListItemBase* IListCtrlBase::EnumItemByProc(ListItemEnumProc pProc, IListItemBase* pEnumFrom, WPARAM w, LPARAM l)
{
    ListItemBase* pEnumFrom2 = nullptr;
    if (pEnumFrom)
        pEnumFrom2 = pEnumFrom->GetImpl();

    ListItemBase* pRet = __pImpl->EnumItemByProc(pProc, pEnumFrom2, w, l);
    if (pRet)
        return pRet->GetIListItemBase();

    return nullptr;
}

IListItemBase*  IListCtrlBase::GetFirstSelectItem() 
{ 
    ListItemBase* p = __pImpl->GetFirstSelectItem(); 
    if (p)
        return p->GetIListItemBase();
    return nullptr;
}
IListItemBase*  IListCtrlBase::GetLastSelectItem()  
{ 
    ListItemBase* p = __pImpl->GetLastSelectItem(); 
    if (p)
        return p->GetIListItemBase();
    return nullptr;
}

IRenderBase*  IListCtrlBase::GetFocusRender()                               
{
    return __pImpl->GetFocusRender();
}
void  IListCtrlBase::SetFocusRender(IRenderBase* p)                        
{
    __pImpl->SetFocusRender(p); 
}
void  IListCtrlBase::SelectItem(IListItemBase* pItem, bool bNotify) 
{
    __pImpl->SelectItem(pItem ? pItem->GetImpl():nullptr, bNotify); 
}

void  IListCtrlBase::ClearSelectItem(bool bNotify)
{
	__pImpl->ClearSelectItem(bNotify);
}
void  IListCtrlBase::UpdateListIfNeed()
{
    __pImpl->UpdateListIfNeed();
}
void  IListCtrlBase::SetNeedLayoutItems()
{
    __pImpl->SetNeedLayoutItems();
}
void  IListCtrlBase::LayoutItem(IListItemBase* pStart, bool bRedraw)       
{
    __pImpl->LayoutItem(pStart?pStart->GetImpl():nullptr, bRedraw); 
}
void  IListCtrlBase::UpdateItemIndex(IListItemBase* pStart)
{
    __pImpl->UpdateItemIndex(pStart?pStart->GetImpl():nullptr); 
}
void  IListCtrlBase::EnableInnerDrag(bool b)
{
	__pImpl->EnableInnerDrag(b);
}
void  IListCtrlBase::EnableReusable(IListCtrlReusableDelegate* p)
{
	__pImpl->EnableReusable(p);
}

void  IListCtrlBase::SetLayout(IListCtrlLayout* pLayout)                   
{
    __pImpl->SetLayout(pLayout); 
}
IListCtrlLayout*  IListCtrlBase::GetLayout()                                
{
    return __pImpl->GetLayout(); 
}
void  IListCtrlBase::SetLayoutFixedHeight()                                    
{
    __pImpl->SetLayoutFixedHeight(); 
}
void  IListCtrlBase::SetLayoutVariableHeight()                                   
{
    __pImpl->SetLayoutVariableHeight(); 
}
IListCtrlItemFixHeightFlowLayout*  IListCtrlBase::SetLayoutFixedHeightFlow()
{
	return __pImpl->SetLayoutFixedHeightFlow();
}

void  IListCtrlBase::InvalidateItem(IListItemBase* pItem) 
{
    if (nullptr == pItem)
        return;

    __pImpl->InvalidateItem(pItem->GetImpl());
}

// void  IListCtrlBase::RedrawItem(IListItemBase** ppItemArray, int nCount) 
// {
//     if (nCount < 0)
//         return;
// 
//     ListItemBase** pp = new ListItemBase*[nCount];
//     for (int i = 0; i < nCount; i++)
//     {
//         if (ppItemArray[i])
//         {
//             pp[i] = ppItemArray[i]->GetImpl();
//         }
//     }
//     __pImpl->RedrawItem(pp, nCount);
//     SAFE_ARRAY_DELETE(pp);
// }
void  IListCtrlBase::RedrawItemByInnerCtrl(IRenderTarget* pRenderTarget, 
                                           IListItemBase* pItem) 
{ 
    __pImpl->RedrawItemByInnerCtrl(pRenderTarget, pItem?pItem->GetImpl():nullptr); 
}
void  IListCtrlBase::MakeItemVisible(IListItemBase* pItem, bool* pbNeedUpdate) 
{
    __pImpl->MakeItemVisible(pItem?pItem->GetImpl():nullptr, pbNeedUpdate); 
}

IListItemShareData*  IListCtrlBase::GetItemTypeShareData(int lType)    
{
    return __pImpl->GetItemTypeShareData(lType); 
}
void  IListCtrlBase::SetItemTypeShareData(int lType, IListItemShareData* pData) 
{
    __pImpl->SetItemTypeShareData(lType, pData); 
}
void  IListCtrlBase::RemoveItemTypeShareData(int lType)                   
{
    __pImpl->RemoveItemTypeShareData(lType);
}

SIZE  IListCtrlBase::GetAdaptWidthHeight(int nWidth, int nHeight)           
{
    return __pImpl->GetAdaptWidthHeight(nWidth, nHeight); 
}
short  IListCtrlBase::GetVertSpacing()                                      
{
    return __pImpl->GetVertSpacing(); 
}
short  IListCtrlBase::GetHorzSpacing()                                      
{
    return __pImpl->GetHorzSpacing(); 
}

int   IListCtrlBase::GetChildNodeIndent()                                   
{
    return __pImpl->GetChildNodeIndent(); 
}
void  IListCtrlBase::SetChildNodeIndent(int n)                              
{
    __pImpl->SetChildNodeIndent(n); 
}
void  IListCtrlBase::GetItemContentPadding(REGION4* prc)
{
	__pImpl->GetItemContentPadding(prc);
}
void  IListCtrlBase::SetItemContentPadding(REGION4* prc)
{
	__pImpl->SetItemContentPadding(prc);
}
void  IListCtrlBase::SetMinWidth(int n)                                     
{
    __pImpl->SetMinWidth(n); 
}
void  IListCtrlBase::SetMinHeight(int n)                                    
{
    __pImpl->SetMinHeight(n); 
}
int   IListCtrlBase::GetMinWidth()                                         
{
    return __pImpl->GetMinWidth(); 
} 
int   IListCtrlBase::GetMinHeight()                                         
{
    return __pImpl->GetMinHeight(); 
}
int   IListCtrlBase::GetMaxWidth()                                         
{
    return __pImpl->GetMaxWidth(); 
}
int   IListCtrlBase::GetMaxHeight()                                        
{
    return __pImpl->GetMaxHeight(); 
}
int  IListCtrlBase::GetItemHeight()                                        
{
    return __pImpl->GetItemHeight(); 
}
void  IListCtrlBase::SetItemHeight(int nHeight, bool bUpdate)              
{
    __pImpl->SetItemHeight(nHeight, bUpdate); 
}

void  IListCtrlBase::Scroll2Y(int nY, bool bUpdate)                         
{
    __pImpl->Scroll2Y(nY, bUpdate); 
}
void  IListCtrlBase::Scroll2X(int nX, bool bUpdate)                        
{
    __pImpl->Scroll2X(nX, bUpdate); 
}
void  IListCtrlBase::ScrollY(int nY, bool bUpdate)                          
{
    __pImpl->ScrollY(nY, bUpdate); 
}
void  IListCtrlBase::ScrollX(int nX, bool bUpdate)                         
{
    __pImpl->ScrollX(nX, bUpdate); 
}
void  IListCtrlBase::SetScrollPos(int nX, int nY, bool bUpdate)            
{
    __pImpl->SetScrollPos(nX, nY, bUpdate); 
}
void  IListCtrlBase::ListItemDragDropEvent(UI::DROPTARGETEVENT_TYPE eEvent, IListItemBase* pItem)
{
    __pImpl->ListItemDragDropEvent(
        eEvent, pItem?pItem->GetImpl():nullptr); 
}
void  IListCtrlBase::ListCtrlDragScroll()
{
    __pImpl->ListCtrlDragScroll();
}

bool  IListCtrlBase::IsItemRectVisibleInScreen(LPCRECT prc)
{
    return __pImpl->IsItemRectVisibleInScreen(prc);
}


signal<IListCtrlBase*>&  IListCtrlBase::SelectChangedEvent()
{
	return __pImpl->select_changed;
}
signal<ListCtrlClickEventParam*>&  IListCtrlBase::ClickEvent()
{
	return __pImpl->click;
}
#if _MSC_VER >= 1800
signal<IListCtrlBase*, IListItemBase*>&  IListCtrlBase::RClickEvent()
{
    return __pImpl->rclick;
}
signal<IListCtrlBase*, IListItemBase*>&  IListCtrlBase::MClickEvent()
{
    return __pImpl->mclick;
}
signal<IListCtrlBase*, IListItemBase*>&  IListCtrlBase::DBClickEvent()
{
    return __pImpl->dbclick;
}
signal<IListCtrlBase*, unsigned int, bool&>&  IListCtrlBase::KeyDownEvent()
{
    return __pImpl->keydown;
}
#else
signal2<IListCtrlBase*, IListItemBase*>&  IListCtrlBase::RClickEvent()
{
	return __pImpl->rclick;
}
signal2<IListCtrlBase*, IListItemBase*>&  IListCtrlBase::MClickEvent()
{
	return __pImpl->mclick;
}
signal2<IListCtrlBase*, IListItemBase*>&  IListCtrlBase::DBClickEvent()
{
	return __pImpl->dbclick;
}
signal3<IListCtrlBase*, unsigned int, bool&>&  IListCtrlBase::KeyDownEvent()
{
	return __pImpl->keydown;
}
#endif
// void  IListCtrlBase::SetListItemRectChangedCallback(
//         bool (__stdcall *func)(IListItemBase& item,LPCRECT prcOld,LPCRECT prcNew))
// {
//     ListCtrlAnimateCallback::SetListItemRectChangedCallback(func);
// }

IListCtrlBase*  IListItemShareData::GetListCtrlBase()                   
{
    return __pImpl->GetListCtrlBase(); 
}


}