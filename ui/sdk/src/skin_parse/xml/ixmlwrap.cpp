#include "include/interface/ixmlwrap.h"
#include "include/macro/helper.h"
#include "xmlwrap.h"

namespace ui 
{
IUIElement::IUIElement(UIElement* p) : m_pImpl(p)
{

}

const char*  IUIElement::GetData()
{
    return m_pImpl->GetData();
}

const char*  IUIElement::GetTagName()
{
    return m_pImpl->GetTagName();
}

bool  IUIElement::SetData(const char* szText)
{
    return m_pImpl->SetData(szText);
}

void  IUIElement::SetTagName(const char* sz)
{
    m_pImpl->SetTagName(sz);
}

UIElement*  IUIElement::GetImpl()
{
    return m_pImpl;
}

long  IUIElement::AddRef()
{
    return m_pImpl->AddRef();
}
long  IUIElement::Release()
{
    return m_pImpl->Release();
}

#if defined(OS_WIN)
bool  IUIElement::GetAttrib(const char* szKey, BSTR* pbstrValue)
{   
    return m_pImpl->GetAttrib(szKey, pbstrValue);
}
#endif

bool  IUIElement::HasAttrib(const char* szKey)
{
    return m_pImpl->HasAttrib(szKey);
}
bool  IUIElement::AddAttrib(const char* szKey, const char* szValue)
{
    return m_pImpl->AddAttrib(szKey, szValue);
}
bool  IUIElement::SetAttrib(const char* szKey, const char* szValue)
{
    return m_pImpl->SetAttrib(szKey, szValue);
}
bool  IUIElement::RemoveAttrib(const char* szKey)
{
    return m_pImpl->RemoveAttrib(szKey);
}
bool  IUIElement::ModifyAttrib(const char* szKey, const char* szValue)
{
    return m_pImpl->ModifyAttrib(szKey, szValue);
}
bool  IUIElement::ClearAttrib()
{
    return m_pImpl->ClearAttrib();
}

void  IUIElement::GetAttribList(IMapAttribute* ppMapAttrib)
{
    m_pImpl->GetAttribList(ppMapAttrib);
}

void  IUIElement::GetAttribList2(IListAttribute** ppListAttrib)
{
    m_pImpl->GetAttribList2(ppListAttrib);
}
bool  IUIElement::SetAttribList2(IListAttribute* pListAttrib)
{
    return m_pImpl->SetAttribList2(pListAttrib);
}

//
// 将UIElementProxy作为IUIElementProxy返回给外部
//
#define RETURN_IUIElementProxy(UIElementProxy_) \
    if (UIElementProxy_) \
        return IUIElementProxy(UIElementProxy_.detach()->GetIUIElement()); \
    else \
        return IUIElementProxy();

IUIElementProxy  IUIElement::AddChild(const char* szNodeName)
{
    UIElementProxy proxy = m_pImpl->AddChild(szNodeName);
    RETURN_IUIElementProxy(proxy);
}

bool  IUIElement::RemoveChild(IUIElement* p)
{
    if (!p)
        return false;

    return m_pImpl->RemoveChild(p->GetImpl());
}

bool  IUIElement::RemoveSelfInParent()
{
    return m_pImpl->RemoveSelfInParent();
}

bool  IUIElement::AddChildBefore(IUIElement* pElem, IUIElement* pInsertBefore)
{
    return m_pImpl->AddChildBefore(
            pElem ? pElem->GetImpl():nullptr,
            pInsertBefore ? pInsertBefore->GetImpl():nullptr);
}
bool  IUIElement::AddChildAfter(IUIElement*  pElem, IUIElement* pInsertAfter)
{
    return m_pImpl->AddChildAfter(
            pElem ? pElem->GetImpl():nullptr,
            pInsertAfter ? pInsertAfter->GetImpl():nullptr);
}
IUIElementProxy  IUIElement::AddChildBefore(const char* szNodeName, IUIElement* pInsertBefore)
{
    UIElementProxy proxy = m_pImpl->AddChildBefore(
            szNodeName,
            pInsertBefore ? pInsertBefore->GetImpl():nullptr);

    RETURN_IUIElementProxy(proxy);
}
IUIElementProxy  IUIElement::AddChildAfter(const char* szNodeName, IUIElement* pInsertAfter)
{
    UIElementProxy proxy = m_pImpl->AddChildAfter(
            szNodeName,
            pInsertAfter ? pInsertAfter->GetImpl():nullptr);

    RETURN_IUIElementProxy(proxy);
}
// bool  IUIElement::MoveChildAfterChild(IUIElement* pChild2Move, IUIElement* pChildInsertAfter)
// {
//     return m_pImpl->MoveChildAfterChild(
//             pChild2Move ? pChild2Move->GetImpl():nullptr,
//             pChildInsertAfter ? pChildInsertAfter->GetImpl():nullptr);
// }

bool  IUIElement::MoveTo(IUIElement* pNewParent, IUIElement* pChildInsertAfter)
{
    return m_pImpl->MoveTo(
        pNewParent ? pNewParent->GetImpl() : nullptr,
        pChildInsertAfter ? pChildInsertAfter->GetImpl() : nullptr
        );
}

IUIElementProxy  IUIElement::FirstChild()
{
    UIElementProxy proxy = m_pImpl->FirstChild();
    RETURN_IUIElementProxy(proxy);
}

IUIElementProxy  IUIElement::NextElement()
{
    UIElementProxy proxy = m_pImpl->NextElement();
    RETURN_IUIElementProxy(proxy);
}

IUIElementProxy  IUIElement::FindChild(const char* szChildName)
{
    UIElementProxy proxy = m_pImpl->FindChild(szChildName);
    RETURN_IUIElementProxy(proxy);
}

IUIDocument*  IUIElement::GetDocument()
{
    UIDocument* pDoc = m_pImpl->GetDocument();
    if (pDoc)
        return pDoc->GetIUIDocument();
    else
        return nullptr;
}

//////////////////////////////////////////////////////////////////////////

IUIDocument::IUIDocument(UIDocument* p):m_pImpl(p)
{
}

long  IUIDocument::AddRef()
{
    return m_pImpl->AddRef();
}
long  IUIDocument::Release()
{
    return m_pImpl->Release();
}

void  IUIDocument::SetDirty(bool b)
{
    return m_pImpl->SetDirty(b);
}
bool  IUIDocument::IsDirty()
{
    return m_pImpl->IsDirty();
}

IUIElementProxy  IUIDocument::FindElem(const char* szText)
{
    UIElementProxy proxy = m_pImpl->FindElem(szText);
    RETURN_IUIElementProxy(proxy);
}

const char*  IUIDocument::GetSkinPath()
{
    return m_pImpl->GetSkinPath();
}

//////////////////////////////////////////////////////////////////////////


IUIElementProxy::IUIElementProxy()
{
    m_ptr = nullptr;
}
IUIElementProxy::IUIElementProxy(IUIElement* pNew) 
{
    UIASSERT(pNew);

    m_ptr = pNew;
    m_ptr->AddRef();
}
IUIElementProxy::~IUIElementProxy()
{
    release();
}

void  IUIElementProxy::release()
{
    if (m_ptr)
    {
        m_ptr->Release();
    }
}

IUIElementProxy::IUIElementProxy(const IUIElementProxy& o)
{
    m_ptr = o.m_ptr;
    if (m_ptr)
        m_ptr->AddRef();
}
IUIElementProxy& IUIElementProxy::operator=(const IUIElementProxy& o)
{
    release();

    m_ptr = o.m_ptr;
    if (m_ptr)
        m_ptr->AddRef();

    return *this;
}

IUIElement* IUIElementProxy::operator->()
{
    return m_ptr;
}

IUIElementProxy::operator bool() const
{
    if (!m_ptr)
        return false;

    return true;
}

IUIElement*  IUIElementProxy::get()
{
    return m_ptr;
}

}