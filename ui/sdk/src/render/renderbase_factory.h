#pragma once
namespace ui
{	
	class UIApplication;
	struct IRenderBase;

class RenderBaseFactory
{
public:
	RenderBaseFactory(UIApplication& app);
	~RenderBaseFactory();

	void  Init();
	void  Clear();

	bool  RegisterUIRenderBaseCreateData(
            const wchar_t* szName, 
            int nType,
            pfnUICreateRenderBasePtr pfunc);

	bool  CreateRenderBaseByName(
            ISkinRes* pSkinRes, 
            const wchar_t* szName, 
            IObject* pObject, 
            IRenderBase** ppOut);

	bool  CreateRenderBase(
            ISkinRes* pSkinRes, 
            int nType, 
            IObject* pObject, 
            IRenderBase** ppOut);

	const wchar_t*  GetRenderBaseName(int nType);

	void  EnumRenderBaseName(
            pfnEnumRenderBaseNameCallback callback, 
            long wParam, 
            long lParam);

private:
	UIApplication&  m_app;

	struct UIRENDERBASE_CREATE_INFO
	{
		String  m_strName;    // ���ںܶ���theme���ͣ�����Ӧ�Ŀؼ����Ͳ�һ��
		int     m_nRenderType;
		pfnUICreateRenderBasePtr m_func;
	};

	typedef std::vector<UIRENDERBASE_CREATE_INFO*>  UIRENDERBASE_CREATE_DATA;
	UIRENDERBASE_CREATE_DATA  m_vecUIRenderBaseCreateData;  // ���ڴ���RenderBase
};

}