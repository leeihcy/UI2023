#include "stdafx.h"
#include "include/interface/iscrollbarmanager.h"
#include "src/UIObject\ScrollBar\scrollbarmanager.h"

using namespace ui;

IScrollBarManager::IScrollBarManager(ScrollBarManager* p) : IMessage(CREATE_IMPL_FALSE)
{
    IMessage::m_pImpl = p;
    m_pImpl = p; 
}
IScrollBarManager::~IScrollBarManager()
{
    IMessage::m_pImpl = nullptr;
    m_pImpl = nullptr;
}

ScrollBarManager*  IScrollBarManager::GetImpl()                      
{ 
    return m_pImpl;
}

/*static*/ IScrollBarManager*  IScrollBarManager::CreateIScrollBarManager()
{
    ScrollBarManager* p = new ScrollBarManager;
    return p->GetIScrollBarMgr();
}
/*static*/ void  IScrollBarManager::DestroyIScrollBarManager(IScrollBarManager* p)
{
    if (p)
    {
        delete p->GetImpl();
        return;
    }
}

void  IScrollBarManager::SetBindObject(IObject* pBindObj)             
{
    m_pImpl->SetBindObject(pBindObj); 
}
IObject*  IScrollBarManager::GetBindObject()                          
{ 
    return m_pImpl->GetBindObject(); 
}
void  IScrollBarManager::SetHScrollBar(IMessage* p)                   
{
    m_pImpl->SetHScrollBar(p);
}
void  IScrollBarManager::SetVScrollBar(IMessage* p)                   
{ 
    m_pImpl->SetVScrollBar(p); 
}
IMessage*  IScrollBarManager::GetHScrollBar()                         
{ 
    return m_pImpl->GetHScrollBar(); 
}
IMessage*  IScrollBarManager::GetVScrollBar()                        
{ 
    return m_pImpl->GetVScrollBar(); 
}

void  IScrollBarManager::SetScrollBarVisibleType(SCROLLBAR_DIRECTION_TYPE eDirType, SCROLLBAR_VISIBLE_TYPE eVisType)
{ m_pImpl->SetScrollBarVisibleType(eDirType, eVisType); }
SCROLLBAR_VISIBLE_TYPE  IScrollBarManager::GetScrollBarVisibleType(SCROLLBAR_DIRECTION_TYPE eType)
{ return m_pImpl->GetScrollBarVisibleType(eType); }
void  IScrollBarManager::FireScrollMessage(SCROLLBAR_DIRECTION_TYPE eType, int nCode, int nTrackPos) 
{ m_pImpl->FireScrollMessage(eType, nCode, nTrackPos); }

BOOL  IScrollBarManager::virtualProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook) { return m_pImpl->nvProcessMessage(pMsg, nMsgMapID, bDoHook); }
void  IScrollBarManager::Serialize(SerializeParam* pData) { m_pImpl->Serialize(pData); }

void  IScrollBarManager::SetHScrollPos(int nPos)                      { m_pImpl->SetHScrollPos(nPos); }
void  IScrollBarManager::SetVScrollPos(int nPos)                      { m_pImpl->SetVScrollPos(nPos); }

void  IScrollBarManager::SetVScrollLine(int nLine)                    { m_pImpl->SetVScrollLine(nLine); }
void  IScrollBarManager::SetHScrollLine(int nLine)                    { m_pImpl->SetHScrollLine(nLine); }
void  IScrollBarManager::SetVScrollWheel(int nWheel)                  { m_pImpl->SetVScrollWheel(nWheel); }
void  IScrollBarManager::SetHScrollWheel(int nWheel)                  { m_pImpl->SetHScrollWheel(nWheel); }

void  IScrollBarManager::SetScrollRange(int nX, int nY)               { m_pImpl->SetScrollRange(nX, nY); }
void  IScrollBarManager::SetHScrollRange(int nX)                      { m_pImpl->SetHScrollRange(nX); }
void  IScrollBarManager::SetVScrollRange(int nY)                      
{ 
    m_pImpl->SetVScrollRange(nY); 
}
void  IScrollBarManager::SetScrollPageAndRange(const SIZE* pPage, const SIZE* pRange) 
{
    m_pImpl->SetScrollPageAndRange(pPage, pRange); 
}

void  IScrollBarManager::GetScrollInfo(SCROLLBAR_DIRECTION_TYPE eDirType, UISCROLLINFO* pInfo) 
{ 
    m_pImpl->GetScrollInfo(eDirType, pInfo); 
}

void  IScrollBarManager::GetScrollPos(int* pX, int* pY)              
{
    m_pImpl->GetScrollPos(pX, pY);
}
int   IScrollBarManager::GetScrollPos(SCROLLBAR_DIRECTION_TYPE eDirType) 
{ 
    return m_pImpl->GetScrollPos(eDirType);
}
int   IScrollBarManager::GetHScrollPos()                             
{ 
    return m_pImpl->GetHScrollPos(); 
}
int   IScrollBarManager::GetVScrollPos()                              
{ 
    return m_pImpl->GetVScrollPos();
}

void  IScrollBarManager::GetScrollPage(int* pX, int* pY)             
{
    m_pImpl->GetScrollPage(pX, pY);
}
int   IScrollBarManager::GetScrollPage(SCROLLBAR_DIRECTION_TYPE eDirType)
{ 
    return m_pImpl->GetScrollPage(eDirType);
}
int   IScrollBarManager::GetHScrollPage()                             
{ 
    return m_pImpl->GetHScrollPage();
}
int   IScrollBarManager::GetVScrollPage()                             
{ 
    return m_pImpl->GetVScrollPage();
}
void  IScrollBarManager::SetVScrollPage(int nY)
{
    return m_pImpl->SetVScrollPage(nY);
}

void  IScrollBarManager::GetScrollRange(int *pX, int* pY)            
{ 
    m_pImpl->GetScrollRange(pX, pY); 
}
int   IScrollBarManager::GetScrollRange(SCROLLBAR_DIRECTION_TYPE eDirType) 
{ 
    return m_pImpl->GetScrollRange(eDirType); 
}
int   IScrollBarManager::GetHScrollRange() 
{ 
    return m_pImpl->GetHScrollRange(); 
}
int   IScrollBarManager::GetVScrollRange() 
{ 
    return m_pImpl->GetVScrollRange(); 
}
bool  IScrollBarManager::IsVScrollBarAtBottom()
{
	return m_pImpl->IsVScrollBarAtBottom();
}
bool  IScrollBarManager::DoPageDown()
{
    return m_pImpl->DoPageDown();
}
bool  IScrollBarManager::DoPageUp()
{
    return m_pImpl->DoPageUp();
}
bool  IScrollBarManager::DoLineDown()
{
    return m_pImpl->DoLineDown();
}
bool  IScrollBarManager::DoLineUp()
{
    return m_pImpl->DoLineUp();
}
bool  IScrollBarManager::DoHome()
{
    return m_pImpl->DoHome();
}
bool  IScrollBarManager::DoEnd()
{
    return m_pImpl->DoEnd();
}
bool  IScrollBarManager::IsSmoothScrolling()
{
    return m_pImpl->IsSmoothScrolling();
}

void  IScrollBarManager::SetVScrollInfo(UISCROLLINFO* lpsi)
{
    m_pImpl->SetVScrollInfo(lpsi);
}