#pragma once

class LayoutFactory
{
public:
	LayoutFactory();
	~LayoutFactory();

	void  Init();
	void  Clear();

	bool  Register(LPCTSTR szName, int nType, pfnUICreateLayoutPtr);
	bool  Create(int nType, IObject* pObject, ILayout** ppOut);
	bool  CreateByName(LPCTSTR szName, IObject* pObject, bool bCreateDefault, ILayout** ppOut);   
	void  EnumLayoutType(pfnEnumLayoutTypeCallback callback, WPARAM wParam, LPARAM lParam);

private:
	struct UILAYOUT_CREATE_INFO
	{
		String  m_strName;
		int     m_nLayoutType;
		GUID    m_guid;
		pfnUICreateLayoutPtr  m_func;
	};
	typedef vector<UILAYOUT_CREATE_INFO*> UILAYOUT_CREATE_DATA;

	UILAYOUT_CREATE_DATA  m_vecData;
};