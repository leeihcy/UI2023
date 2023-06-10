#include "stdafx.h"
#include "Inc\Interface\iobject.h"
#include "Src\Base\Object\object.h"
#include "Src\UIObject\Window\windowbase.h"
#include "Inc\Interface\ixmlwrap.h"
#include "..\..\Layer\layer.h"
#include "..\Application\uiapplication.h"
#include "Src\Resource\skinres.h"

using namespace UI;

UI_IMPLEMENT_INTERFACE(Object, Message)

//////////////////////////////////////////////////////////////////////////
// 
// TCHAR*  IObject::GetObjectName()                          
// { 
// 	return __pImpl->GetObjectName();
// }
// OBJ_TYPE  IObject::GetObjectType()                        
// { 
// 	return __pImpl->GetObjectType(); 
// }
// CONTROL_TYPE  IObject::GetObjectExtentType()             
// {
// 	return __pImpl->GetObjectExtentType();
// }

void  IObject::SetDescription(IObjectDescription* p)
{
    __pImpl->SetDescription(p);
}
IObjectDescription*  IObject::GetDescription()
{
    return __pImpl->GetDescription();
}

LPCTSTR  IObject::GetId()                          
{
	return __pImpl->GetId();
}
void  IObject::SetId(LPCTSTR szText)
{
	__pImpl->SetId(szText); 
}

IWindowBase*  IObject::GetWindowObject() 
{ 
    WindowBase* p = __pImpl->GetWindowObject();
    if (p)
        return p->GetIWindowBase();
    else
        return nullptr;
}

ILayer*  IObject::GetLayer()
{
    Layer* p = __pImpl->GetLayer();
    if (!p)
        return nullptr;

    return p->GetILayer();
}

ILayer*  IObject::GetSelfLayer()
{
	UI::Layer* layer = __pImpl->GetSelfLayer();
	if (layer)
		return layer->GetILayer();
	return nullptr;
}

HWND  IObject::GetHWND()                                   
{
	return __pImpl->GetHWND(); 
}

IUIApplication* IObject::GetUIApplication()                
{
	UIApplication* p = __pImpl->GetUIApplication();
	if (!p)
		return nullptr;

	return p->GetIUIApplication();
}
ISkinRes*  IObject::GetSkinRes()
{
	SkinRes* p = __pImpl->GetSkinRes();
	if (!p)
		return nullptr;

	return p->GetISkinRes();
}

void  IObject::SetOutRef(void** ppOutRef)                  
{
	__pImpl->SetOutRef(ppOutRef); 
}

void  IObject::InitDefaultAttrib()                          
{
	__pImpl->InitDefaultAttrib(); 
}
void  IObject::SetObjectPos(int x, int y, int cx, int cy, int nFlag)
{ 
	__pImpl->SetObjectPos(x, y, cx, cy, nFlag);
}
void  IObject::SetObjectPos(LPCRECT prc, int nFlag)          
{
	__pImpl->SetObjectPos(prc, nFlag);
}
// void  IObject::SetUserData(LPVOID p)                        
// { 
// 	__pImpl->SetUserData(p); 
// }
// LPVOID  IObject::GetUserData()                             
// { 
// 	return __pImpl->GetUserData(); 
// }
void  IObject::ModifyObjectStyle(OBJSTYLE* add, OBJSTYLE* remove)
{
	__pImpl->ModifyObjectStyle(add, remove);
}
bool  IObject::TestObjectStyle(const OBJSTYLE& test)
{
	return __pImpl->TestObjectStyle(test);
}
// void  IObject::ModifyStyleEx( UINT add, UINT remove, bool bNotify) 
// {
// 	__pImpl->ModifyStyleEx(add, remove, bNotify); 
// }
// bool  IObject::TestStyleEx(UINT nStyleEx)                  
// {
// 	return __pImpl->TestStyleEx(nStyleEx); 
// }
// UINT  IObject::GetStyleEx()                                
// {
// 	return __pImpl->GetStyleEx(); 
// }
// void  IObject::SetStyleEx(UINT n)                           
// { 
// 	return __pImpl->SetStyleEx(n);
// }
// void  IObject::SetCanRedraw(bool bReDraw)                   
// {
// 	__pImpl->SetCanRedraw(bReDraw);
// }
// bool  IObject::CanRedraw()                                  
// {
// 	return __pImpl->CanRedraw(); 
// }
IRenderFont*  IObject::GetRenderFont()
{
	return __pImpl->GetRenderFont(); 
}
// void  IObject::SetRenderLayer(bool b)                       
// {
// 	return __pImpl->SetRenderLayer(b);
// }

bool  IObject::IsFocus()                                   
{ 
	return __pImpl->IsFocus(); 
}
bool  IObject::SetFocusInWindow()
{
	return __pImpl->SetFocusInWindow();
}
// bool  IObject::IsTabstop()         
// { 
// 	return __pImpl->IsTabstop(); 
// }
bool  IObject::CanTabstop()      
{
	return __pImpl->CanTabstop();
}
// void  IObject::SetTabstop(bool b)  
// {
// 	__pImpl->SetTabstop(b);
// }
bool  IObject::IsVisible()        
{ 
	return __pImpl->IsVisible(); 
}
bool  IObject::IsCollapsed()    
{ 
	return __pImpl->IsCollapsed(); 
}
bool  IObject::IsSelfVisible()  
{
	return __pImpl->IsSelfVisible();
}
bool  IObject::IsEnable()        
{ 
	return __pImpl->IsEnable(); 
}
void  IObject::SetVisible(bool b) 
{
	__pImpl->SetVisible(b); 
}
void  IObject::SetVisibleEx(VISIBILITY_TYPE b)
{
	__pImpl->SetVisibleEx(b); 
}
void  IObject::SetEnable(bool b) 
{
	__pImpl->SetEnable(b); 
}
bool  IObject::IsDefault()                                 
{ 
	return __pImpl->IsDefault();
}
bool  IObject::IsHover()                                  
{ 
	return __pImpl->IsHover(); 
}
bool  IObject::IsPress()                                  
{ 
	return __pImpl->IsPress(); 
}
bool  IObject::IsForcePress()                           
{ 
	return __pImpl->IsForcePress(); 
}
bool  IObject::IsForceHover()                            
{
	return __pImpl->IsForceHover(); 
}
// bool  IObject::IsSelected()                              
// { 
// 	return __pImpl->IsSelected(); 
// }
// void  IObject::SetDefault(bool b, bool bNotify)        
// {
// 	__pImpl->SetDefault(b, bNotify);
// }
void  IObject::SetForceHover(bool b, bool bNotify)        
{
	__pImpl->SetForceHover(b, bNotify); 
}
void  IObject::SetForcePress(bool b, bool bNotify)         
{ 
	__pImpl->SetForcePress(b, bNotify);
}
// void  IObject::SetHover(bool b, bool bNotify)              
// { 
// 	__pImpl->SetHover(b, bNotify); 
// }
// void  IObject::SetPress(bool b, bool bNotify)              
// {
// 	__pImpl->SetPress(b, bNotify);
// }
// void  IObject::SetSelected(bool b, bool bNotify)          
// {
// 	__pImpl->SetSelected(b, bNotify); 
// }

// bool  IObject::IsTransparent()                            
// {
// 	return __pImpl->IsTransparent();
// }
// void  IObject::SetTransparent(bool b)                      
// { 
// 	__pImpl->SetTransparent(b); 
// }
// bool  IObject::IsNcObject()                                
// {
// 	return __pImpl->IsNcObject();
// }
// void  IObject::SetAsNcObject(bool b)                       
// { 
// 	__pImpl->SetAsNcObject(b); 
// }
// bool  IObject::IsRejectMouseMsgAll()                       
// {
// 	return __pImpl->IsRejectMouseMsgAll(); 
// }
// bool  IObject::IsRejectMouseMsgSelf()                      
// { 
// 	return __pImpl->IsRejectMouseMsgSelf(); 
// }
// void  IObject::SetRejectMouseMsgSelf(bool b)                      
// { 
// 	__pImpl->SetRejectMouseMsgSelf(b); 
// }
void  IObject::SetZorderDirect(int lz)                      
{ 
	return __pImpl->SetZorderDirect(lz);
}
int   IObject::GetZOrder()                                 
{ 
	return __pImpl->GetZorder(); 
}
void  IObject::SortChildByZorder()
{
	__pImpl->SortChildByZorder();
}

IObject*  IObject::FindObject(LPCTSTR szObjId) 
{
    Object* p = __pImpl->FindObject(szObjId); 
    if (p)
        return p->GetIObject();
    return nullptr;
}

IObject*  IObject::TryFindObject(LPCTSTR szObjId)
{
    Object* p = __pImpl->TryFindObject(szObjId);
    if (p)
        return p->GetIObject();
    return nullptr;
}

IObject*  IObject::FindNcObject(LPCTSTR szObjId)
{
	Object* p = __pImpl->FindNcObject(szObjId); 
	if (p)
		return p->GetIObject();
	return nullptr;
}
IObject*  IObject::FindObject(UUID uuid)
{
    Object* p = __pImpl->FindObject(uuid);
    if (p)
        return p->GetIObject();
    return nullptr;
}
IObject*  IObject::FindNcObject(UUID uuid)
{
	Object* p = __pImpl->FindNcObject(uuid);
	if (p)
		return p->GetIObject();
	return nullptr;
}

IObject*  IObject::GetParentObject()  
{
    Object* p = __pImpl->GetParentObject(); 
    if (p)
        return p->GetIObject();
    return nullptr;
}
IObject*  IObject::GetChildObject() 
{ 
    Object* p = __pImpl->GetChildObject(); 
    if (p)
        return p->GetIObject();
    return nullptr;
}
// IObject*  IObject::GetNcChildObject() 
// { 
//     Object* p = __pImpl->GetNcChildObject(); 
//     if (p)
//         return p->GetIObject();
//     return nullptr;
// }
IObject*  IObject::GetLastChildObject()
{ 
    Object* p = __pImpl->GetLastChildObject(); 
    if (p)
        return p->GetIObject();
    return nullptr;
}
IObject*  IObject::GetNextObject()
{ 
    Object* p = __pImpl->GetNextObject(); 
    if (p)
        return p->GetIObject();
    return nullptr;
}
IObject*  IObject::GetPrevObject() 
{
    Object* p = __pImpl->GetPrevObject();
    if (p)
        return p->GetIObject();
    return nullptr;
}
// IObject*  IObject::GetRootObject()
// { 
//     Object* p = __pImpl->GetRootObject(); 
//     if (p)
//         return p->GetIObject();
//     return nullptr;
// }

// IObject*  IObject::GetChildObjectByIndex(unsigned long lIndex)
// {
// 	Object* pObject = __pImpl->GetChildObjectIndex(lIndex);
// 	if (!pObject)
// 		return nullptr;
// 
// 	return pObject->GetIObject();
// }
// 
IObject*  IObject::EnumChildObject(IObject* p)  
{
    Object* pObject = nullptr;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = __pImpl->EnumChildObject(pObject); 
    if (pRet)
        return pRet->GetIObject();

    return nullptr;
}
IObject*  IObject::REnumChildObject(IObject* p) 
{ 
    Object* pObject = nullptr;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = __pImpl->REnumChildObject(pObject); 
    if (pRet)
        return pRet->GetIObject();

    return nullptr;
}
IObject*  IObject::EnumNcChildObject(IObject* p) 
{
    Object* pObject = nullptr;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = __pImpl->EnumNcChildObject(pObject); 
    if (pRet)
        return pRet->GetIObject();

    return nullptr;
}

IObject*  IObject::EnumAllChildObject(IObject* p) 
{ 
    Object* pObject = nullptr;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = __pImpl->EnumAllChildObject(pObject);
    if (nullptr == pRet)
        return nullptr;

    return pRet->GetIObject();
}
// IObject*  IObject::EnumParentObject(IObject* p)
// { 
//     Object* pObject = nullptr;
//     if (p)
//         pObject = p->GetImpl();
// 
//     Object* pRet = __pImpl->EnumParentObject(pObject); 
//     if (nullptr == pRet)
//         return nullptr;
// 
//     return pRet->GetIObject();
// }
// IObject*  IObject::REnumParentObject(IObject* p)
// { 
//     Object* pObject = nullptr;
//     if (p)
//         pObject = p->GetImpl();
// 
//     Object* pRet = __pImpl->REnumParentObject(pObject); 
//     if (nullptr == pRet)
//         return nullptr;
// 
//     return pRet->GetIObject();
// }
// 
// IObject*  IObject::GetNextTreeItemObject()
// { 
//     Object* p = __pImpl->GetNextTreeItemObject(); 
//     if (nullptr == p)
//         return nullptr;
// 
//     return p->GetIObject();
// }
// IObject*  IObject::GetPrevTreeItemObject()
// { 
//     Object* p = __pImpl->GetPrevTreeItemObject(); 
//     if (nullptr == p)
//         return nullptr;
// 
//     return p->GetIObject();
// }
// IObject*  IObject::GetNextTreeTabstopItemObject() 
// { 
//     Object* p = __pImpl->GetNextTreeTabstopItemObject(); 
//     if (nullptr == p)
//         return nullptr;
// 
//     return p->GetIObject();
// }
// IObject*  IObject::GetPrevTreeTabstopItemObject() 
// { 
//     Object* p =  __pImpl->GetPrevTreeTabstopItemObject(); 
//     if (nullptr == p)
//         return nullptr;
// 
//     return p->GetIObject();
// }
// IObject*  IObject::GetNextTabObject()
// { 
//     Object* p = __pImpl->GetNextTabObject(); 
//     if (nullptr == p)
//         return nullptr;
// 
//     return p->GetIObject();
// }
// IObject*  IObject::GetPrevTabObject()
// {
//     Object* p = __pImpl->GetPrevTabObject(); 
//     if (nullptr == p)
//         return nullptr;
// 
//     return p->GetIObject();
// }
// 
// void  IObject::SetParentObjectDirect(IObject* p) 
// { 
//     if (p)
//         __pImpl->SetParentObjectDirect(p->GetImpl()); 
//     else
//         __pImpl->SetParentObjectDirect(nullptr); 
// 
// }
// void  IObject::SetChildObjectDirect(IObject* p)
// {
//     if (p)
//         __pImpl->SetChildObjectDirect(p->GetImpl()); 
//     else
//         __pImpl->SetChildObjectDirect(nullptr); 
// 
// }
// void  IObject::SetNcChildObjectDirect(IObject* p)
// {
//     if (p)
//         __pImpl->SetNcChildObjectDirect(p->GetImpl()); 
//     else
//         __pImpl->SetNcChildObjectDirect(nullptr); 
// 
// }
// void  IObject::SetNextObjectDirect(IObject* p) 
// {
//     if (p)
//         __pImpl->SetNextObjectDirect(p->GetImpl()); 
//     else
//         __pImpl->SetNextObjectDirect(nullptr); 
// 
// }
// void  IObject::SetPrevObjectDirect(IObject* p) 
// {
//     if (p)
//         __pImpl->SetPrevObjectDirect(p->GetImpl()); 
//     else
//         __pImpl->SetPrevObjectDirect(nullptr); 
// }
// 
// void  IObject::AddChild(IObject* p)
// { 
//     if (p)
//         __pImpl->AddChild(p->GetImpl()); 
// }
void  IObject::InsertChild(IObject* pObj, IObject* pInsertAfter)
{
    Object*  pInsertAfterImpl = nullptr;
    if (pInsertAfter)
        pInsertAfterImpl = pInsertAfter->GetImpl();

    if (pObj)
        __pImpl->InsertChild(pObj->GetImpl(), pInsertAfterImpl);
}
// void  IObject::AddNcChild(IObject*p) 
// {
//     if (p)
//         __pImpl->AddNcChild(p->GetImpl()); 
// }
// void  IObject::InsertAfter(IObject* pInsertAfter)
// {
//     Object* p = nullptr;
// 
//     if (pInsertAfter)
//         p = pInsertAfter->GetImpl();
// 
//     __pImpl->InsertAfter(p);
// }
// void  IObject::InsertBefore(IObject* pInsertBefore)
// {
//     Object* p = nullptr;
// 
//     if (pInsertBefore)
//         p = pInsertBefore->GetImpl();
// 
//     __pImpl->InsertBefore(p);
// }
bool  IObject::IsMyChild(IObject* pChild, bool bFindInGrand) 
{
    if (nullptr == pChild)
        return false;

    return __pImpl->IsMyChild(pChild->GetImpl(), bFindInGrand); 
}
bool  IObject::RemoveChildInTree(IObject* pChild) 
{ 
    if (nullptr == pChild)
        return false;

    return __pImpl->RemoveChildInTree(pChild->GetImpl()); 
}
// void  IObject::RemoveMeInTheTree() 
// { 
//     __pImpl->RemoveMeInTheTree(); 
// }
// 
// void  IObject::UpdateObjectNonClientRegion()
// {
//     return __pImpl->UpdateObjectNonClientRegion();
// }
// void  IObject::Standalone() { __pImpl->ClearMyTreeRelationOnly(); }
// void  IObject::MoveToAsFirstChild() { __pImpl->MoveToAsFirstChild(); }
// void  IObject::MoveToAsLastChild() { __pImpl->MoveToAsLastChild(); }
// bool  IObject::SwapObject(IObject* pObj1, IObject* pObj2)
// {
//     Object* p1 = nullptr;
//     Object* p2 = nullptr;
// 
//     if (pObj1)
//         p1 = pObj1->GetImpl();
//     if (pObj2)
//         p2 = pObj2->GetImpl();
// 
//     return __pImpl->SwapObject(p1, p2);
// }

void  IObject::Invalidate()
{
	__pImpl->Invalidate();
}
void  IObject::Invalidate(LPCRECT prcObj)
{
	__pImpl->Invalidate(prcObj);
}
void  IObject::Invalidate(RECT* prcObj, int nCount)
{
	__pImpl->Invalidate(prcObj, nCount);
}
// void  IObject::UpdateObject(bool bUpdateNow)
// { 
//     __pImpl->UpdateObject(bUpdateNow); 
// }
// void  IObject::UpdateObjectEx(RECT* prcObjArray, int nCount, bool bUpdateNow) 
// { 
//     __pImpl->UpdateObjectEx(prcObjArray, nCount, bUpdateNow); 
// }
// void  IObject::UpdateObjectBkgnd(bool bUpdateNow) 
// {
//     __pImpl->UpdateObjectBkgnd(bUpdateNow);
// }
// void  IObject::UpdateLayout() 
// { 
//     __pImpl->UpdateLayout();
// }
// void  IObject::UpdateMyLayout()
// { 
//     __pImpl->UpdateMyLayout();
// }

void  IObject::GetNonClientRegion(REGION4* prc) 
{
    __pImpl->GetNonClientRegion(prc); 
}
void  IObject::SetExtNonClientRegion(REGION4* prc)
{
    __pImpl->SetExtNonClientRegion(prc); 
}
void  IObject::GetExtNonClientRegion(REGION4* prc)
{
    __pImpl->GetExtNonClientRegion(prc);
}

void  IObject::GetBorderRegion(REGION4* prc){ __pImpl->GetBorderRegion(prc); }
// void  IObject::SetBorderRegion(REGION4* prc) { __pImpl->SetBorderRegion(prc); }
void  IObject::GetPaddingRegion(REGION4* prc){ __pImpl->GetPaddingRegion(prc); }
void  IObject::SetPaddingRegion(REGION4* prc){ __pImpl->SetPaddingRegion(prc); }
void  IObject::SetMarginRegion(REGION4* prc) { __pImpl->SetMarginRegion(prc); }
void  IObject::GetMarginRegion(REGION4* prc) { __pImpl->GetMarginRegion(prc); }
void  IObject::GetClientRectInObject(RECT* prc)        
{ 
	__pImpl->GetClientRectInObject(prc); 
}
void  IObject::GetObjectClientRect(RECT* prc) 
{
	__pImpl->GetObjectClientRect(prc); 
}
void  IObject::GetClientRectInWindow(RECT* prc)
{ 
	__pImpl->GetClientRectInWindow(prc); 
}
void  IObject::GetParentRect(RECT* prc)       
{ 
	__pImpl->GetParentRect(prc); 
}

// POINT  IObject::GetRealPosInWindow()                                              
// { 
//     return __pImpl->GetWindowPoint(); 
// }
void  IObject::GetWindowRect(RECT* prc)                                          
{ 
    __pImpl->GetWindowRect(prc);
}
void  IObject::WindowRect2ObjectRect(const RECT* rcWindow, RECT* rcObj)
{
    __pImpl->WindowRect2ObjectRect(rcWindow, rcObj); 
}
void  IObject::WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform)      
{
    __pImpl->WindowPoint2ObjectPoint(ptWindow, ptObj, bCalcTransform); 
}
void  IObject::WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj, bool bCalcTransform)
{ 
    __pImpl->WindowPoint2ObjectClientPoint(ptWindow, ptObj, bCalcTransform); 
}
// void  IObject::ObjectPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj) 
// {
//     Object::ObjectPoint2ObjectClientPoint(__pImpl, ptWindow, ptObj); 
// }
void  IObject::ClientRect2ObjectRect(const RECT* rcClient, RECT* rcObj)           
{
    Object::ObjectClientRect2ObjectRect(__pImpl, rcClient, rcObj); 
}
// void  IObject::ObjectRect2ClientRect(const RECT* rcObj, RECT* rcClient)
// {
//     Object::ObjectRect2ObjectClientRect(__pImpl, rcObj, rcClient);
// }
void  IObject::ClientRect2WindowRect(const RECT* rcClient, RECT* rcWnd)
{
	Object::ObjectClientRect2WindowRect(__pImpl, rcClient, rcWnd);
}

// bool  IObject::GetScrollOffset(int* pxOffset, int* pyOffset)
// { 
// 	return __pImpl->GetScrollOffset(pxOffset, pyOffset);
// }
// bool  IObject::GetScrollRange(int* pxRange, int* pyRange)   
// {
// 	return __pImpl->GetScrollRange(pxRange, pyRange); 
// }
// bool  IObject::GetRectInWindow(RECT* prc, bool bVisiblePart)      
// {
// 	return __pImpl->GetRectInWindow(prc, bVisiblePart);
// }
// bool  IObject::GetRectInLayer(RECT* prc, bool bVisiblePart)        
// { 
// 	return __pImpl->GetRectInLayer(prc, bVisiblePart); 
// }
// bool  IObject::GetVisibleClientRectInLayer(RECT* prc)
// { 
// 	return __pImpl->GetVisibleClientRectInLayer(prc);
// }

int  IObject::GetWidth()            
{ 
	return __pImpl->GetWidth(); 
}
int  IObject::GetHeight()            
{ 
	return __pImpl->GetHeight(); 
}
int  IObject::GetMaxWidth()
{
	return __pImpl->GetMaxWidth(); 
}
int  IObject::GetMaxHeight()
{
	return __pImpl->GetMaxHeight(); 
}
void  IObject::SetMaxWidth(int n)
{
	__pImpl->SetMaxWidth(n);
}
void  IObject::SetMaxHeight(int n)
{
	__pImpl->SetMaxHeight(n);
}


int  IObject::GetMinWidth()
{
	return __pImpl->GetMinWidth(); 
}
int  IObject::GetMinHeight()
{
	return __pImpl->GetMaxHeight(); 
}
void  IObject::SetMinWidth(int n)
{
	__pImpl->SetMinWidth(n);
}
void  IObject::SetMinHeight(int n)
{
	__pImpl->SetMinHeight(n);
}
// int  IObject::GetWidthWithMargins()  
// {
// 	return __pImpl->GetWidthWithMargins();
// }
// int  IObject::GetHeightWithMargins()
// { 
// 	return __pImpl->GetHeightWithMargins();
// }

ILayoutParam*  IObject::GetSafeLayoutParam() 
{ 
	return __pImpl->GetSafeLayoutParam();
}
// void  IObject::CreateLayoutParam()
// {
// 	__pImpl->CreateLayoutParam();
// }
// int   IObject::GetConfigWidth()        
// { 
// 	return __pImpl->GetConfigWidth();
// }
// int   IObject::GetConfigHeight()       
// { 
// 	return __pImpl->GetConfigHeight();
// }
// int   IObject::GetConfigLayoutFlags()  
// { 
// 	return __pImpl->GetConfigLayoutFlags();
// }
// int   IObject::GetConfigLeft()         
// {
// 	return __pImpl->GetConfigLeft(); 
// }
// int   IObject::GetConfigRight()       
// { 
// 	return __pImpl->GetConfigRight();
// }
// int   IObject::GetConfigTop()         
// { 
// 	return __pImpl->GetConfigTop(); 
// }
// int   IObject::GetConfigBottom()       
// { 
// 	return __pImpl->GetConfigBottom(); 
// }
// void  IObject::SetConfigWidth(int n)   
// { 
// 	__pImpl->SetConfigWidth(n); 
// }
// void  IObject::SetConfigHeight(int n)  
// { 
// 	__pImpl->SetConfigHeight(n); 
// }
// void  IObject::SetConfigLayoutFlags(int n) 
// { 
// 	__pImpl->SetConfigLayoutFlags(n); 
// }
// void  IObject::SetConfigLeft(int n)    
// { 
// 	__pImpl->SetConfigLeft(n); 
// }
// void  IObject::SetConfigRight(int n)   
// { 
// 	__pImpl->SetConfigRight(n);
// }
// void  IObject::SetConfigTop(int n)     
// { 
// 	__pImpl->SetConfigTop(n); 
// }
// void  IObject::SetConfigBottom(int n) 
// {
// 	__pImpl->SetConfigBottom(n);
// }

ITextRenderBase*  IObject::GetTextRender() 
{ 
	return __pImpl->GetTextRender();
}
IRenderBase*  IObject::GetBackRender()       
{ 
	return __pImpl->GetBackRender();
}
IRenderBase*  IObject::GetForeRender()     
{
	return __pImpl->GetForeRender();
}
void  IObject::SetBackRender(IRenderBase* p)
{
	__pImpl->SetBackRender(p); 
}
// void  IObject::SetForegndRender(IRenderBase* p) 
// { 
// 	__pImpl->SetForegndRender(p);
// }
void  IObject::SetTextRender(ITextRenderBase* p)
{
	__pImpl->SetTextRender(p);
}

// void  IObject::SetAttributeByPrefix(LPCTSTR szPrefix, IMapAttribute* pMatAttrib, bool bReload, bool bErase) 
// { 
// 	__pImpl->SetAttributeByPrefix(szPrefix, pMatAttrib, bReload, bErase); 
// }
void  IObject::LoadAttributeFromMap(IMapAttribute* pMatAttrib, bool bReload)
{
	__pImpl->LoadAttributeFromMap(pMatAttrib, bReload); 
}
void  IObject::LoadAttributeFromXml(IUIElement* pXmlElement, bool bReload)
{
    __pImpl->LoadAttributeFromXml(
            pXmlElement ? pXmlElement->GetImpl():nullptr, bReload);
}
LPCTSTR  IObject::GetAttribute(LPCTSTR szKey, bool bErase) 
{ 
	return __pImpl->GetAttribute(szKey, bErase);
}
void  IObject::AddAttribute(LPCTSTR szKey, LPCTSTR  szValue)
{ 
	__pImpl->AddAttribute(szKey, szValue); 
}
void  IObject::GetMapAttribute(IMapAttribute** ppMapAttribute)
{ 
	__pImpl->GetMapAttribute(ppMapAttribute); 
}
SIZE  IObject::GetDesiredSize()
{ 
	return __pImpl->GetDesiredSize(); 
}

// HBITMAP  IObject::TakeSnapshot() 
// {
// 	return __pImpl->TakeSnapshot();
// }
// HBITMAP  IObject::TakeBkgndSnapshot()
// { 
// 	return __pImpl->TakeBkgndSnapshot(); 
// }

bool  IObject::SetMouseCapture(int nNotifyMsgId)
{ 
	return __pImpl->SetMouseCapture(nNotifyMsgId);
}
bool  IObject::ReleaseMouseCapture()
{ 
	return __pImpl->ReleaseMouseCapture(); 
}
// bool  IObject::SetKeyboardCapture(int nNotifyMsgId)
// { 
// 	return __pImpl->SetKeyboardCapture(nNotifyMsgId);
// }
// bool  IObject::ReleaseKeyboardCapture()
// {
// 	return __pImpl->ReleaseKeyboardCapture(); 
// }
// 
// void  IObject::LoadColor(LPCTSTR szColor, Color*& pColorRef)
// {
//     return __pImpl->LoadColor(szColor, pColorRef);
// }
// LPCTSTR  IObject::SaveColor(Color*& pColorRef)
// {
//     return __pImpl->SaveColor(pColorRef);
// }
// void  IObject::LoadTextRender(LPCTSTR szName)
// {
// 	__pImpl->LoadTextRender(szName);
// }
// LPCTSTR  IObject::SaveTextRender()
// {
// 	return __pImpl->SaveTextRender();
// }

void  IObject::AddChild(IObject* p)
{
	if (p)
		__pImpl->AddChild(p->GetImpl());
}
void  IObject::AddNcChild(IObject*p)
{
	if (p)
		__pImpl->AddNcChild(p->GetImpl());
}
void  IObject::RemoveMeInTheTree()
{
	__pImpl->RemoveMeInTheTree();
}

void IObject::ForwardMessageToChildObject(UIMSG* pMsg)
{
    Object::ForwardMessageToChildObject(__pImpl, pMsg);
}
void IObject::ForwardInitializeMessageToDecendant()
{
    Object::ForwardInitializeMessageToDecendant(__pImpl);
}

unsigned char IObject::GetOpacity() const
{
	return __pImpl->GetOpacity();
}
void  IObject::SetOpacity(const unsigned char alpha, LayerAnimateParam* param)
{
	__pImpl->SetOpacity(alpha, param);
}

void  IObject::TranslateBy(float x, float y, float z, LayerAnimateParam* param)
{
	__pImpl->TranslateBy(x,y,z, param);
}

void  IObject::TranslateTo(float x, float y, float z, LayerAnimateParam* param)
{
	__pImpl->TranslateTo(x, y, z, param);
}
void  IObject::TranslateToParent(int x, int y, LayerAnimateParam* param)
{
	__pImpl->TranslateToParent(x, y, param);

}
void  IObject::TranslateXTo(float x, LayerAnimateParam* param)
{
	__pImpl->TranslateXTo(x, param);
}
void  IObject::TranslateYTo(float y, LayerAnimateParam* param)
{
	__pImpl->TranslateYTo(y, param);
}
void  IObject::RotateXTo(float degree, LayerAnimateParam* param)
{
	__pImpl->RotateXTo(degree, param);
}
void  IObject::RotateXBy(float degree, LayerAnimateParam* param)
{
	__pImpl->RotateXBy(degree, param);
}
void  IObject::RotateYTo(float degree, LayerAnimateParam* param)
{
	__pImpl->RotateYTo(degree, param);
}
void  IObject::RotateYBy(float degree, LayerAnimateParam* param)
{
	__pImpl->RotateYBy(degree, param);
}
void  IObject::RotateZTo(float degree, LayerAnimateParam* param)
{
	__pImpl->RotateZTo(degree, param);
}
void  IObject::RotateZBy(float degree, LayerAnimateParam* param)
{
	__pImpl->RotateZBy(degree, param);
}
void  IObject::ScaleTo(float x, float y, LayerAnimateParam* param)
{
	__pImpl->ScaleTo(x, y, param);
}
