#include "stdafx.h"
#include "accessibleimpl.h"

#pragma comment(lib, "Oleacc.lib")
using namespace ui;

AccessibleImpl::AccessibleImpl()
{
	m_lRef = 0;
}

AccessibleImpl::~AccessibleImpl()
{
	UIAIter iter = m_listUIA.begin();
	for (; iter != m_listUIA.end(); ++iter)
	{
		(*iter)->Release();
	}
	m_listUIA.clear();
}

HRESULT AccessibleImpl::QueryInterface(
		REFIID riid, void **ppvObj)
{   
	*ppvObj = nullptr;

	if (IsEqualIID(riid, IID_IUnknown))
	{
		*ppvObj = static_cast<IUnknown*>(this);
		AddRef();
	}
	else if (IsEqualIID(riid, IID_IDispatch)) 
	{
		*ppvObj = static_cast<IDispatch*>(this);
		AddRef();
	}
	else if (IsEqualIID(riid, IID_IAccessible))
	{
		*ppvObj = static_cast<IAccessible*>(this);
		AddRef();
	}

	return (nullptr == *ppvObj) ? E_NOINTERFACE : 0;
}

ULONG AccessibleImpl::AddRef(void)
{
	//m_lRef++;
	InterlockedIncrement(&m_lRef);
	return m_lRef;
}

ULONG AccessibleImpl::Release(void)
{
	//--m_lRef;
	InterlockedDecrement(&m_lRef);
	if (0 == m_lRef)
	{
		delete this;
		return 0;
	}
	return m_lRef;
}

// accHitTest必须处理，直接返回E_UNEXPECTED会导致AccExplorer32崩溃
HRESULT  AccessibleImpl::accHitTest( 
		long xLeft,
		long yTop,
		VARIANT *pvarChild)
{
	UIAIter iter = m_listUIA.begin();
	for (; iter != m_listUIA.end(); ++iter)
	{
		BOOL bHandled = FALSE;
		HRESULT hr = (*iter)->accHitTest(
			xLeft, yTop, pvarChild, bHandled);

		if (bHandled)
			return hr;
	}

	pvarChild->vt = VT_EMPTY;
	return S_FALSE;
}

HRESULT  AccessibleImpl::accLocation( 
		long *pxLeft,
		long *pyTop,
		long *pcxWidth,
		long *pcyHeight,
		VARIANT varChild) 
{
	UIAIter iter = m_listUIA.begin();
	for (; iter != m_listUIA.end(); ++iter)
	{
		BOOL bHandled = FALSE;
		HRESULT hr = (*iter)->accLocation(
			pxLeft, pyTop, pcxWidth, pcyHeight, varChild, bHandled);

		if (bHandled)
			return hr;
	}

	return E_NOTIMPL;
}

HRESULT  AccessibleImpl::accNavigate( 
		long navDir,
		VARIANT varStart,
		VARIANT *pvarEndUpAt)
{
	UIAIter iter = m_listUIA.begin();
	for (; iter != m_listUIA.end(); ++iter)
	{
		BOOL bHandled = FALSE;
		HRESULT hr = (*iter)->accNavigate(
			navDir, varStart, pvarEndUpAt, bHandled);

		if (bHandled)
			return hr;
	}

	return E_NOTIMPL;
}

HRESULT  AccessibleImpl::get_accParent(IDispatch **ppdispParent)
{
	UIAIter iter = m_listUIA.begin();
	for (; iter != m_listUIA.end(); ++iter)
	{
		BOOL bHandled = FALSE;
		HRESULT hr = (*iter)->get_accParent(
			ppdispParent, bHandled);

		if (bHandled)
			return hr;
	}

	return E_NOTIMPL;
}

HRESULT  AccessibleImpl::get_accChildCount(long *pcountChildren)
{
	UIAIter iter = m_listUIA.begin();
	for (; iter != m_listUIA.end(); ++iter)
	{
		BOOL bHandled = FALSE;
		HRESULT hr = (*iter)->get_accChildCount(
			pcountChildren, bHandled);

		if (bHandled)
			return hr;
	}

	return E_NOTIMPL;
}

HRESULT  AccessibleImpl::get_accChild(VARIANT varChild, IDispatch **ppdispChild)
{
	UIAIter iter = m_listUIA.begin();
	for (; iter != m_listUIA.end(); ++iter)
	{
		BOOL bHandled = FALSE;
		HRESULT hr = (*iter)->get_accChild(
			varChild, ppdispChild, bHandled);

		if (bHandled)
			return hr;
	}

	return E_NOTIMPL;
}

HRESULT  AccessibleImpl::get_accName( 
		VARIANT varChild, BSTR *pszName)
{
	UIAIter iter = m_listUIA.begin();
	for (; iter != m_listUIA.end(); ++iter)
	{
		BOOL bHandled = FALSE;
		HRESULT hr = (*iter)->get_accName(
			varChild, pszName, bHandled);

		if (bHandled)
			return hr;
	}

	return E_NOTIMPL;
}

HRESULT  AccessibleImpl::get_accValue( 
		VARIANT varChild, BSTR *pszValue)
{
	UIAIter iter = m_listUIA.begin();
	for (; iter != m_listUIA.end(); ++iter)
	{
		BOOL bHandled = FALSE;
		HRESULT hr = (*iter)->get_accValue(
			varChild, pszValue, bHandled);

		if (bHandled)
			return hr;
	}

	return E_NOTIMPL;
}

HRESULT  AccessibleImpl::get_accDescription( 
		VARIANT varChild, BSTR* pszDescription)
{
	UIAIter iter = m_listUIA.begin();
	for (; iter != m_listUIA.end(); ++iter)
	{
		BOOL bHandled = FALSE;
		HRESULT hr = (*iter)->get_accDescription(
			varChild, pszDescription, bHandled);

		if (bHandled)
			return hr;
	}

	return E_NOTIMPL;
}

HRESULT  AccessibleImpl::get_accRole( 
		VARIANT varChild, VARIANT *pvarRole) 
{
	UIAIter iter = m_listUIA.begin();
	for (; iter != m_listUIA.end(); ++iter)
	{
		BOOL bHandled = FALSE;
		HRESULT hr = (*iter)->get_accRole(
			varChild, pvarRole, bHandled);

		if (bHandled)
			return hr;
	}

	return E_NOTIMPL;
}

HRESULT  AccessibleImpl::get_accState(VARIANT varChild, 
		VARIANT *pvarState)
{
	UIAIter iter = m_listUIA.begin();
	for (; iter != m_listUIA.end(); ++iter)
	{
		BOOL bHandled = FALSE;
		HRESULT hr = (*iter)->get_accState(
			varChild, pvarState, bHandled);

		if (bHandled)
			return hr;
	}

	return E_NOTIMPL;
}

void  AccessibleImpl::AddUIAccessible(IUIAccessible* p)
{
	if (!p)
		return;

	p->AddRef();
	m_listUIA.push_back(p);
}