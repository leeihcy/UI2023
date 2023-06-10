#pragma once
#include "accessibleimpl.h"
#include "..\Object\object.h"

namespace UI
{

class ObjectAccessible : public IUIAccessible
{
public:
	ObjectAccessible(Object& o);
	~ObjectAccessible();

	HWND  GetHWND();

private:
	virtual HRESULT get_accParent( 
		IDispatch **ppdispParent, BOOL& bHandled) override;

	virtual HRESULT  get_accChildCount( 
		long *pcountChildren, BOOL& bHandled) override;

	virtual HRESULT  get_accChild( VARIANT varChild,
		IDispatch **ppdispChild, BOOL& bHandled) override;

	virtual HRESULT  get_accRole(VARIANT varChild,
		VARIANT *pvarRole, BOOL& bHandled) override;

	virtual HRESULT  get_accName(VARIANT varChild,
		BSTR *pszName, BOOL& bHandled) override;

	virtual HRESULT  get_accDescription(VARIANT varChild,
		BSTR *pszDescription, BOOL& bHandled) override;

	virtual HRESULT  get_accState(VARIANT varChild,
		VARIANT *pvarState, BOOL& bHandled) override;

	virtual HRESULT  accNavigate(long navDir, VARIANT varStart,
		VARIANT *pvarEndUpAt, BOOL& bHandled) override;

	virtual HRESULT  accHitTest(long xLeft, long yTop,
		VARIANT *pvarChild, BOOL& bHandled) override;

	virtual HRESULT  accLocation( 
		long *pxLeft,
		long *pyTop,
		long *pcxWidth,
		long *pcyHeight,
		VARIANT varChild, 
		BOOL& bHandled) override;


private:
	Object&  m_refObject;
};

}