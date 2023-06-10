#include "stdafx.h"
#include "Inc\Interface\iuiapplication.h"
#include "Src\Helper\topwindow\topwindowmanager.h"
#include "Inc\Interface\iwindow.h"
#include "Inc\Interface\iuires.h"
#include "Inc\Interface\iuiinterface.h"

namespace UI
{

ITopWindowManager::ITopWindowManager(TopWindowManager* p) 
{
    m_pImpl = p;
}
TopWindowManager* ITopWindowManager::GetImpl()
{
    return m_pImpl;
}

HRESULT  ITopWindowManager::AddTopWindowObject(IWindowBase* p)
{
    if (nullptr == p)
        return E_INVALIDARG;

    return m_pImpl->AddTopWindowObject(p->GetImpl());
}
HRESULT  ITopWindowManager::RemoveTopWindowObject(IWindowBase* p)
{
    if (nullptr == p)
        return E_INVALIDARG;

    return m_pImpl->RemoveTopWindowObject(p->GetImpl());
}
// void  ITopWindowManager::ChangeSkin(ISkinRes* pNewSkinRes)
// {
//     if (nullptr == pNewSkinRes)
//         return;
// 
//     m_pImpl->ChangeSkin(pNewSkinRes->GetImpl());
// }

bool  ITopWindowManager::UpdateAllWindow() { return m_pImpl->UpdateAllWindow(); }
void  ITopWindowManager::SendMessage2AllWnd(UIMSG*  pMsg) { return m_pImpl->SendMessage2AllWnd(pMsg); }
//void  ITopWindowManager::PostMessage2AllWnd(UIMSG* pMsg)  { return m_pImpl->PostMessage2AllWnd(pMsg); }
void  ITopWindowManager::ForwardMessage2AllObj(UIMSG*  pMsg) { return m_pImpl->ForwardMessage2AllObj(pMsg); }

//////////////////////////////////////////////////////////////////////////

}