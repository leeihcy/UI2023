#pragma once

namespace ui
{
	class UIApplication;
	struct ITextRenderBase;

class TextRenderFactory
{
public:
	TextRenderFactory(UIApplication& app);
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
	UIApplication&  m_app;

	struct UITEXTRENDERBASE_CREATE_INFO
	{
		std::wstring   m_strName;
		int        m_nRenderType;
		pfnUICreateTextRenderBasePtr m_func;
	};

	typedef std::vector<UITEXTRENDERBASE_CREATE_INFO*>    UITEXTRENDERBASE_CREATE_DATA;
	UITEXTRENDERBASE_CREATE_DATA  m_vecUITextRenderBaseCreateData;  // 用于从字符串创建TextRenderBase
};
}