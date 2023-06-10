#pragma once
namespace UI
{	
	class UIApplication;
	interface IRenderBase;
}

class RenderBaseFactory
{
public:
	RenderBaseFactory(UIApplication& app);
	~RenderBaseFactory();

	void  Init();
	void  Clear();

	bool  RegisterUIRenderBaseCreateData(
            LPCTSTR szName, 
            int nType,
            pfnUICreateRenderBasePtr pfunc);

	bool  CreateRenderBaseByName(
            ISkinRes* pSkinRes, 
            LPCTSTR szName, 
            IObject* pObject, 
            IRenderBase** ppOut);

	bool  CreateRenderBase(
            ISkinRes* pSkinRes, 
            int nType, 
            IObject* pObject, 
            IRenderBase** ppOut);

	LPCTSTR  GetRenderBaseName(int nType);

	void  EnumRenderBaseName(
            pfnEnumRenderBaseNameCallback callback, 
            WPARAM wParam, 
            LPARAM lParam);

private:
	UIApplication&  m_app;

	struct UIRENDERBASE_CREATE_INFO
	{
		String  m_strName;    // 存在很多种theme类型，但对应的控件类型不一样
		int     m_nRenderType;
		pfnUICreateRenderBasePtr m_func;
	};

	typedef vector<UIRENDERBASE_CREATE_INFO*>  UIRENDERBASE_CREATE_DATA;
	UIRENDERBASE_CREATE_DATA  m_vecUIRenderBaseCreateData;  // 用于创建RenderBase
};