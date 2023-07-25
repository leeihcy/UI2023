#ifndef _UI_ISKINRES_H_
#define _UI_ISKINRES_H_

#include "include/base/uidefine.h"

namespace ui
{
	struct ISkinManager;
	struct IImageManager;
	struct IImageManager;
	struct IColorManager;
	struct IFontManager;
	struct IStyleManager;
	struct ILayoutManager;
	struct IImageRes;
	struct IFontRes;
	struct IColorRes;
	struct IStyleRes;
	struct ISkinDataSource;
	struct IUIDocument;
	struct II18nRes;

	class SkinRes;
	struct UIAPI ISkinRes
	{
		ISkinRes(SkinRes*);
		SkinRes*  GetImpl();
		void  SetParentSkinRes(ISkinRes*);

		IUIApplication*  GetUIApplication();
        ISkinManager&    GetSkinManager();
        IImageManager&   GetImageManager();
        IColorManager&   GetColorManager();
        IFontManager&    GetFontManager();
        IStyleManager&   GetStyleManager();
        ILayoutManager&  GetLayoutManager();
        IImageRes&       GetImageRes();
        IFontRes&        GetFontRes();
        IColorRes&       GetColorRes();
        IStyleRes&       GetStyleRes();
		II18nRes&        GetI18nRes();

		ISkinDataSource*  GetDataSource();

		unsigned int  GetXmlDocCount();
		IUIDocument*  GetXmlDoc(unsigned int nIndex);
		IUIDocument*  GetXmlDocByName(const wchar_t* szName);

		const wchar_t* GetName();
		const wchar_t* GetPath();

		bool  Load();
		bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

	private:
		SkinRes*  m_pImpl;
	};


	struct ISkinDataSource;
	class SkinManager;
	struct UIAPI ISkinManager : public IRootInterface
	{
		ISkinManager(SkinManager*);
		SkinManager*  GetImpl();

		IUIApplication*  GetUIApplication();
		void  ChangeSkinHLS(short h, short l, short s, int nFlag);
		bool  Save(ISkinRes* pSkinRes);

		unsigned int  GetSkinResCount();
		ISkinRes*  GetSkinResByIndex(unsigned int i);
		ISkinRes*  GetSkinResByName(const wchar_t* szName);

        void  SetCurrentLanguage(const wchar_t*);
        const wchar_t*  GetCurrentLanguage();

	private:
		SkinManager*  m_pImpl;
	};
}
#endif