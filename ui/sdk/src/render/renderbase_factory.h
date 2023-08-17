#pragma once
namespace ui
{	
	class Application;
	struct IRenderBase;

class RenderBaseFactory
{
public:
	RenderBaseFactory(Application& app);
	~RenderBaseFactory();

	void  Init();
	void  Clear();

	bool  RegisterUIRenderBaseCreateData(
            const wchar_t* szName, 
            int nType,
            pfnUICreateRenderBasePtr pfunc);

	bool  CreateRenderBaseByName(
            IResource* pSkinRes, 
            const wchar_t* szName, 
            IObject* pObject, 
            IRenderBase** ppOut);

	bool  CreateRenderBase(
            IResource* pSkinRes, 
            int nType, 
            IObject* pObject, 
            IRenderBase** ppOut);

	const wchar_t*  GetRenderBaseName(int nType);

	void  EnumRenderBaseName(
            pfnEnumRenderBaseNameCallback callback, 
            long wParam, 
            long lParam);

private:
	Application&  m_app;

	struct UIRENDERBASE_CREATE_INFO
	{
		String  m_strName;    // 存在很多种theme类型，但对应的控件类型不一样
		int     m_nRenderType;
		pfnUICreateRenderBasePtr m_func;
	};

	typedef std::vector<UIRENDERBASE_CREATE_INFO*>  UIRENDERBASE_CREATE_DATA;
	UIRENDERBASE_CREATE_DATA  m_vecUIRenderBaseCreateData;  // 用于创建RenderBase
};

}