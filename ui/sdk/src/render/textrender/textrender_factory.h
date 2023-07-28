#pragma once

namespace ui
{
	class Application;
	struct ITextRenderBase;

class TextRenderFactory
{
public:
	TextRenderFactory(Application& app);
	~TextRenderFactory();

	void  Init();
	void  Clear();

	bool  RegisterUITextRenderBaseCreateData(
            const wchar_t* szName, 
            int nType, 
            pfnUICreateTextRenderBasePtr pfunc);

	bool  CreateTextRenderBaseByName(
            IResBundle* pSkinRes, 
            const wchar_t* szName, 
            IObject* pObject, 
            ITextRenderBase** ppOut);

	bool  CreateTextRender(
            IResBundle* pSkinRes, 
            int nType, 
            IObject* pObject, 
            ITextRenderBase** ppOut);

	const wchar_t*  GetTextRenderBaseName(int nType);

	void  EnumTextRenderBaseName(
            pfnEnumTextRenderBaseNameCallback callback, 
            long wParam, 
            long lParam);
private:
	Application&  m_app;

	struct UITEXTRENDERBASE_CREATE_INFO
	{
		std::wstring   m_strName;
		int        m_nRenderType;
		pfnUICreateTextRenderBasePtr m_func;
	};

	typedef std::vector<UITEXTRENDERBASE_CREATE_INFO*>    UITEXTRENDERBASE_CREATE_DATA;
	UITEXTRENDERBASE_CREATE_DATA  m_vecUITextRenderBaseCreateData;  // ���ڴ��ַ�������TextRenderBase
};
}