#include "stdafx.h"
#include "include/interface/ilistitembase.h"
#include "listitembase.h"
#include "..\CustomItem\custom_listitem.h"

using namespace ui;

UI_IMPLEMENT_INTERFACE(ListItemBase, Message);
UI_IMPLEMENT_INTERFACE(CustomListItem, ListItemBase);

void  IListItemBase::SetIListCtrlBase(IListCtrlBase* p)           
{
    __pImpl->SetIListCtrlBase(p); 
}
IListCtrlBase*  IListItemBase::GetIListCtrlBase()                 
{
    return __pImpl->GetIListCtrlBase(); 
}

IListItemBase*  IListItemBase::GetNextItem() 
{ 
    ListItemBase* p = __pImpl->GetNextItem();
    if (p)
        return p->GetIListItemBase();

    return nullptr;
}

IListItemBase*  IListItemBase::GetPrevItem()
{
    ListItemBase* p = __pImpl->GetPrevItem();
    if (p)
        return p->GetIListItemBase();

    return nullptr;
}
IListItemBase*  IListItemBase::GetParentItem()
{
    ListItemBase* p = __pImpl->GetParentItem();
    if (p)
        return p->GetIListItemBase();

    return nullptr;
}
IListItemBase*  IListItemBase::GetChildItem()
{
    ListItemBase* p = __pImpl->GetChildItem();
    if (p)
        return p->GetIListItemBase();

    return nullptr;
}
IListItemBase*  IListItemBase::GetLastChildItem()
{
    ListItemBase* p = __pImpl->GetLastChildItem();
    if (p)
        return p->GetIListItemBase();

    return nullptr;
}
IListItemBase*  IListItemBase::GetPrevSelection()
{
    ListItemBase* p = __pImpl->GetPrevSelection();
    if (p)
        return p->GetIListItemBase();

    return nullptr;
}
IListItemBase*  IListItemBase::GetNextSelection()
{
    ListItemBase* p = __pImpl->GetNextSelection();
    if (p)
        return p->GetIListItemBase();

    return nullptr;
}

IListItemBase*  IListItemBase::GetNextVisibleItem()
{
    ListItemBase* p = __pImpl->GetNextVisibleItem();
    if (p)
        return p->GetIListItemBase();

    return nullptr;
}
IListItemBase*  IListItemBase::GetPrevVisibleItem()
{
    ListItemBase* p = __pImpl->GetPrevVisibleItem();
    if (p)
        return p->GetIListItemBase();

    return nullptr;
}
IListItemBase*  IListItemBase::GetNextTreeItem()
{
    ListItemBase* p = __pImpl->GetNextTreeItem();
    if (p)
        return p->GetIListItemBase();

    return nullptr;
}
IListItemBase*  IListItemBase::GetPrevTreeItem()
{
    ListItemBase* p = __pImpl->GetPrevTreeItem();
    if (p)
        return p->GetIListItemBase();

    return nullptr;
}

bool  IListItemBase::IsMyChildItem(IListItemBase* pChild, bool bTestdescendant)
{
    if (nullptr == pChild)
        return false;

    return __pImpl->IsMyChildItem(pChild->GetImpl(), bTestdescendant);
}

bool  IListItemBase::HasChild() { return __pImpl->HasChild(); }

void  IListItemBase::SetNextItem(IListItemBase* p)
{
    ListItemBase* pListItem = nullptr;
    if (p)
        pListItem = p->GetImpl();

    __pImpl->SetNextItem(pListItem);

}
void  IListItemBase::SetPrevItem(IListItemBase* p)
{
    ListItemBase* pListItem = nullptr;
    if (p)
        pListItem = p->GetImpl();

    __pImpl->SetPrevItem(pListItem);
}
void  IListItemBase::SetParentItem(IListItemBase* p)
{
    ListItemBase* pListItem = nullptr;
    if (p)
        pListItem = p->GetImpl();

    __pImpl->SetParentItem(pListItem);
}
void  IListItemBase::SetChildItem(IListItemBase* p)
{
    ListItemBase* pListItem = nullptr;
    if (p)
        pListItem = p->GetImpl();

    __pImpl->SetChildItem(pListItem);
}
void  IListItemBase::SetLastChildItem(IListItemBase* p)
{
    ListItemBase* pListItem = nullptr;
    if (p)
        pListItem = p->GetImpl();

    __pImpl->SetLastChildItem(pListItem);
}
void  IListItemBase::AddChild(IListItemBase* p)
{
	ListItemBase* pListItem = nullptr;
	if (p)
		pListItem = p->GetImpl();

	__pImpl->AddChild(pListItem);
}
void  IListItemBase::AddChildFront(IListItemBase* p)
{
	ListItemBase* pListItem = nullptr;
	if (p)
		pListItem = p->GetImpl();

	__pImpl->AddChildFront(pListItem);
}

void  IListItemBase::SetNextSelection(IListItemBase* p)
{
    ListItemBase* pListItem = nullptr;
    if (p)
        pListItem = p->GetImpl();

    __pImpl->SetNextSelection(pListItem);
}
void  IListItemBase::SetPrevSelection(IListItemBase* p)
{
    ListItemBase* pListItem = nullptr;
    if (p)
        pListItem = p->GetImpl();

    __pImpl->SetPrevSelection(pListItem);
}

void  IListItemBase::GetParentRect(LPRECT prc)                                
{
    __pImpl->GetParentRect(prc); 
}
void  IListItemBase::SetParentRect(LPCRECT prc)                                
{
    __pImpl->SetParentRect(prc); 
}
void  IListItemBase::SetParentRectLightly(LPCRECT prc)
{
    __pImpl->SetParentRectLightly(prc); 
}
void  IListItemBase::SetFloatRect(LPCRECT prc)
{
    __pImpl->SetFloatRect(prc);
}
int   IListItemBase::GetItemType()                                            
{ 
    return __pImpl->GetItemType(); 
}
void  IListItemBase::SetItemType(long n)                                      
{
    __pImpl->SetItemType(n); 
}
LONG_PTR  IListItemBase::GetId()
{
    return __pImpl->GetId(); 
}
void  IListItemBase::SetId(LONG_PTR n)
{
    return __pImpl->SetId(n); 
}
void  IListItemBase::SetText(const TCHAR* szText)                             
{ 
    __pImpl->SetText(szText); 
}
const TCHAR*  IListItemBase::GetText()                                        
{
    return __pImpl->GetText(); 
}
void  IListItemBase::SetToolTip(const TCHAR* szText)                          
{
    return __pImpl->SetToolTip(szText); 
}
unsigned int  IListItemBase::GetItemFlag()                                            
{
    return __pImpl->GetItemFlag(); 
}
void  IListItemBase::SetItemFlag(int n)                                       
{
    __pImpl->SetItemFlag(n); 
}
const TCHAR*  IListItemBase::GetToolTip()                                     
{
    return __pImpl->GetToolTip(); 
}
long*  IListItemBase::GetData()
{
    return __pImpl->GetData(); 
}
void  IListItemBase::SetData(long* p)
{
    __pImpl->SetData(p); 
}
long*  IListItemBase::GetCtrlData()
{
    return __pImpl->GetCtrlData(); 
}
void  IListItemBase::SetCtrlData(long* p)
{
    __pImpl->SetCtrlData(p); 
}
void  IListItemBase::GetDesiredSize(SIZE* pSize)                              
{
    GetDesiredSizeMessage msg;
    this->RouteMessage(&msg);
    *pSize = msg.size;
}
int   IListItemBase::GetLineIndex()                                           
{
    return __pImpl->GetLineIndex(); 
}
void  IListItemBase::SetLineIndex(int n)                                      
{ 
    __pImpl->SetLineIndex(n); }
int   IListItemBase::GetTreeIndex()                                           
{ 
    return __pImpl->GetTreeIndex(); 
}
int   IListItemBase::GetNeighbourIndex()                                      
{
    return __pImpl->GetNeighbourIndex(); 
}
void  IListItemBase::SetTreeIndex(int n)                                      
{
    __pImpl->SetTreeIndex(n); 
}
void  IListItemBase::SetNeighbourIndex(int n)                                 
{
    __pImpl->SetNeighbourIndex(n);
}
int   IListItemBase::CalcDepth()
{
    return __pImpl->CalcDepth();
}
void  IListItemBase::SetConfigWidth(int n)                                    
{
    __pImpl->SetConfigWidth(n); 
}
void  IListItemBase::SetConfigHeight(int n)                               {
    __pImpl->SetConfigHeight(n);
}
int  IListItemBase::GetConfigWidth()
{
    return __pImpl->GetConfigWidth();
}
int  IListItemBase::GetConfigHeight()
{
    return __pImpl->GetConfigHeight();
}

IRenderBase*  IListItemBase::GetIconRender()                                 
{
    return __pImpl->GetIconRender(); 
}
void  IListItemBase::SetIconRender(IRenderBase* p)                            
{ 
    __pImpl->SetIconRender(p); 
}
void  IListItemBase::SetIconRenderType(RENDER_TYPE eType)                     
{
    __pImpl->SetIconRenderType(eType); 
}
void  IListItemBase::SetIconFromFile(const TCHAR* szIconPath)                 
{
    __pImpl->SetIconFromFile(szIconPath); 
}
void  IListItemBase::SetIconFromImageId(const TCHAR* szImageId)               
{
    __pImpl->SetIconFromImageId(szImageId); 
}

unsigned int  IListItemBase::GetItemState()                                           
{ 
    return __pImpl->GetItemState(); 
}
bool  IListItemBase::IsMySelfVisible()                                       
{
    return __pImpl->IsMySelfVisible(); 
}
bool  IListItemBase::IsVisible()                                             
{
    return __pImpl->IsVisible(); 
}
bool  IListItemBase::IsDisable()                                              
{
    return __pImpl->IsDisable(); 
}
bool  IListItemBase::IsHover()                                                
{
    return __pImpl->IsHover(); 
}
bool  IListItemBase::IsPress()                                                
{ 
    return __pImpl->IsPress(); 
}
bool  IListItemBase::IsFocus()                                               
{
    return __pImpl->IsFocus(); 
}
bool  IListItemBase::IsCollapsed()                                           
{
    return __pImpl->IsCollapsed(); 
}
bool  IListItemBase::IsExpand()                                               
{
    return __pImpl->IsExpand(); 
}
bool  IListItemBase::IsSelected()                                            
{
    return __pImpl->IsSelected(); 
}
bool IListItemBase::IsSelectable()
{
    return __pImpl->IsSelectable();
}
bool  IListItemBase::IsChecked()                                              
{
    return __pImpl->IsChecked(); 
}
bool  IListItemBase::IsRadioChecked()                                        
{
    return __pImpl->IsRadioChecked(); 
}
void  IListItemBase::SetPress(bool b, bool bNotify)                           
{
    __pImpl->SetPress(b, bNotify); 
}
void  IListItemBase::SetHover(bool b, bool bNotify)                          
{
    __pImpl->SetHover(b, bNotify); 
}
void  IListItemBase::SetExpand(bool b, bool bNotify)                          
{
    __pImpl->SetExpand(b, bNotify); 
}
void  IListItemBase::SetFocus(bool b, bool bNotify)                           
{
    __pImpl->SetFocus(b, bNotify); 
}
void  IListItemBase::SetSelected(bool b, bool bNotify)                        
{
    __pImpl->SetSelected(b, bNotify); 
}
void  IListItemBase::SetDisable(bool b, bool bNotify)                        
{
    __pImpl->SetDisable(b, bNotify); 
}
void  IListItemBase::SetChecked(bool b, bool bNotify)                        
{
    __pImpl->SetChecked(b, bNotify); 
}
void  IListItemBase::SetRadioChecked(bool b, bool bNotify)                    
{
    __pImpl->SetRadioChecked(b, bNotify); 
}
bool  IListItemBase::IsDragDropHover()
{
    return __pImpl->IsDragDropHover();
}
bool  IListItemBase::IsFloat()
{
    return __pImpl->IsFloat();
}
void  IListItemBase::SetDragDropHover(bool b, bool bNotify)
{
    __pImpl->SetDragDropHover(b, bNotify);
}
void  IListItemBase::SetSelectable(bool b)
{
    __pImpl->SetSelectable(b);
}

void  IListItemBase::SetVisible(bool b)
{
	__pImpl->SetVisible(b);
}

void  IListItemBase::ModifyStyle(
            LISTITEMSTYLE* pAdd, LISTITEMSTYLE* pRemove)
{
    __pImpl->ModifyStyle(pAdd, pRemove);
}
bool  IListItemBase::TestStyle(const LISTITEMSTYLE& s)
{
    return __pImpl->TestStyle(s);
}

IPanel*  IListItemBase::GetRootPanel()                                       
{
    return __pImpl->GetIRootPanel(); 
}
bool  IListItemBase::CreateRootPanel()                                       
{
    return __pImpl->CreateRootPanel(); 
}
void  IListItemBase::DrawItemInnerControl(IRenderTarget* pRenderTarget)
{
    __pImpl->DrawItemInnerControl(pRenderTarget); 
}
int   IListItemBase::GetItemRenderState()                                     
{
    return (int)UISendMessage(this, UI_LISTITEM_MSG_GETRENDERSTATE); 
}
unsigned int  IListItemBase::GetItemDelayOp()                                         
{
    return __pImpl->GetItemDelayOp(); 
}
void  IListItemBase::AddItemDelayOp(int n)                                    
{
    __pImpl->AddItemDelayOp(n); 
}
void  IListItemBase::RemoveDelayOp(int n)                                    
{
    __pImpl->RemoveDelayOp(n); 
}
void  IListItemBase::ClearDelayOp()
{
    __pImpl->ClearDelayOp(); 
}
long  IListItemBase::GetSort()
{
    return __pImpl->GetSort();
}
void  IListItemBase::SetSort(long l)
{
    __pImpl->SetSort(l);
}
void  IListItemBase::SetFloat(bool b)
{
	__pImpl->SetFloat(b);
}

void  IListItemBase::Invalidate()
{
    __pImpl->Invalidate();
}
//////////////////////////////////////////////////////////////////////////

IObject*  ICustomListItem::FindControl(const wchar_t* szId)
{
	return __pImpl->FindControl(szId);
}

void  ICustomListItem::SetStringData(LPCSTR data)
{
	__pImpl->SetStringData(data);
}
LPCSTR  ICustomListItem::GetStringData()
{
	return __pImpl->GetStringData();
}

//////////////////////////////////////////////////////////////////////////

void  IListItemBase::SetOpacity(
		const unsigned char alpha, LayerAnimateParam* param)
{
	__pImpl->GetLayerForAnimate()->SetOpacity(alpha, param);
}
ILayer*  IListItemBase::GetLayerForAnimate()
{
	return __pImpl->GetLayerForAnimate()->GetILayer();
}