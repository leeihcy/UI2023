#pragma once

namespace UI
{
	class UIApplication;
	interface ITextRenderBase;

}
class TextRenderFactory
{
public:
	TextRenderFactory(UIApplication& app);
	~TextRenderFactory();

	void  Init();
	void  Clear();

	bool  RegisterUITextRenderBaseCreateData(
            LPCTSTR szName, 
            int nType, 
            pfnUICreateTextRenderBasePtr pfunc);

	bool  CreateTextRenderBaseByName(
            ISkinRes* pSkinRes, 
            LPCTSTR szName, 
            IObject* pObject, 
            ITextRenderBase** ppOut);

	bool  CreateTextRender(
            ISkinRes* pSkinRes, 
            int nType, 
            IObject* pObject, 
            ITextRenderBase** ppOut);

	LPCTSTR  GetTextRenderBaseName(int nType);

	void  EnumTextRenderBaseName(
            pfnEnumTextRenderBaseNameCallback callback, 
            WPARAM wParam, 
            LPARAM lParam);
private:
	UIApplication&  m_app;

	struct UITEXTRENDERBASE_CREATE_INFO
	{
		String     m_strName;
		int        m_nRenderType;
		pfnUICreateTextRenderBasePtr m_func;
	};

	typedef vector<UITEXTRENDERBASE_CREATE_INFO*>    UITEXTRENDERBASE_CREATE_DATA;
	UITEXTRENDERBASE_CREATE_DATA  m_vecUITextRenderBaseCreateData;  // 用于从字符串创建TextRenderBase
};