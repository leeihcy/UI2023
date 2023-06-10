#pragma once
#include "iuiaccessible.h"


namespace UI
{
class AccessibleImpl : public IAccessible
{
public:
	#define NOTIMPL  { return E_NOTIMPL; }
	AccessibleImpl();
	virtual ~AccessibleImpl();

#pragma region // IAccessible
	virtual HRESULT STDMETHODCALLTYPE get_accParent( 
		IDispatch **ppdispParent)  override;

	virtual HRESULT STDMETHODCALLTYPE get_accChildCount( 
		long *pcountChildren) override;

	virtual HRESULT STDMETHODCALLTYPE get_accChild( 
		VARIANT varChild, IDispatch **ppdispChild) override;

	virtual HRESULT STDMETHODCALLTYPE get_accName( 
		VARIANT varChild, BSTR *pszName) override;

	virtual HRESULT STDMETHODCALLTYPE get_accValue( 
		VARIANT varChild, BSTR *pszValue) override;

	virtual HRESULT STDMETHODCALLTYPE get_accDescription( 
		VARIANT varChild, BSTR *pszDescription) override;

	virtual HRESULT STDMETHODCALLTYPE get_accRole( 
		VARIANT varChild, VARIANT *pvarRole) override;

	virtual HRESULT STDMETHODCALLTYPE get_accState( 
		VARIANT varChild,
		VARIANT *pvarState) override;

	virtual HRESULT STDMETHODCALLTYPE get_accHelp( 
		VARIANT varChild,
		BSTR *pszHelp) override NOTIMPL;

	virtual HRESULT STDMETHODCALLTYPE get_accHelpTopic( 
		__RPC__deref_out_opt BSTR *pszHelpFile,
		VARIANT varChild,
		long *pidTopic) override NOTIMPL;

	virtual HRESULT STDMETHODCALLTYPE get_accKeyboardShortcut( 
		VARIANT varChild,
		BSTR *pszKeyboardShortcut) override NOTIMPL;

	virtual HRESULT STDMETHODCALLTYPE get_accFocus( 
		VARIANT *pvarChild) override NOTIMPL;

	virtual HRESULT STDMETHODCALLTYPE get_accSelection( 
		VARIANT *pvarChildren) override NOTIMPL;

	virtual HRESULT STDMETHODCALLTYPE get_accDefaultAction( 
		VARIANT varChild,
		BSTR *pszDefaultAction) override NOTIMPL;

	virtual HRESULT STDMETHODCALLTYPE accSelect( 
		long flagsSelect,
		VARIANT varChild) override NOTIMPL;

	virtual HRESULT STDMETHODCALLTYPE accLocation( 
		long *pxLeft,
		long *pyTop,
		long *pcxWidth,
		long *pcyHeight,
		VARIANT varChild) override;

	virtual HRESULT STDMETHODCALLTYPE accNavigate( 
		long navDir,
		VARIANT varStart,
		VARIANT *pvarEndUpAt) override;

	virtual HRESULT STDMETHODCALLTYPE accHitTest( 
		long xLeft,
		long yTop,
		VARIANT *pvarChild) override;

	virtual HRESULT STDMETHODCALLTYPE accDoDefaultAction( 
		VARIANT varChild) override NOTIMPL;

	virtual HRESULT STDMETHODCALLTYPE put_accName( 
		VARIANT varChild,
		__RPC__in BSTR szName) override NOTIMPL;

	virtual HRESULT STDMETHODCALLTYPE put_accValue( 
		VARIANT varChild,
		__RPC__in BSTR szValue) override NOTIMPL;
#pragma endregion

#pragma region // IDispatch
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
		__RPC__out UINT *pctinfo) override 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( 
		UINT iTInfo,
		LCID lcid,
		ITypeInfo **ppTInfo) override NOTIMPL;

	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
		REFIID riid,
		LPOLESTR *rgszNames,
		UINT cNames,
		LCID lcid,
		DISPID *rgDispId) override NOTIMPL;

	virtual HRESULT STDMETHODCALLTYPE Invoke( 
		DISPID dispIdMember,
		REFIID riid,
		LCID lcid,
		WORD wFlags,
		DISPPARAMS *pDispParams,
		VARIANT *pVarResult,
		EXCEPINFO *pExcepInfo,
		UINT *puArgErr) override NOTIMPL;
#pragma endregion

#pragma region // IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(
			REFIID riid, void **ppvObject) override;
	virtual ULONG STDMETHODCALLTYPE AddRef(void) override;
	virtual ULONG STDMETHODCALLTYPE Release(void) override;
#pragma endregion 

public:
	void  AddUIAccessible(IUIAccessible*);

protected:
	
private:
	typedef list<IUIAccessible*>::iterator UIAIter;
	list<IUIAccessible*>  m_listUIA;
	long  m_lRef;
};
}