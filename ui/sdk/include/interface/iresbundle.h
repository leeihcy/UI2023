#ifndef _UI_ISKINRES_H_
#define _UI_ISKINRES_H_

#include "include/base/uidefine.h"

namespace ui
{
	struct IResourceManager;
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

	class ResBundle;
	struct UIAPI IResBundle
	{
		IResBundle(ResBundle*);
		ResBundle*  GetImpl();
		void  SetParentSkinRes(IResBundle*);

		IApplication*  GetUIApplication();
        IResourceManager&    GetResourceManager();
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
		ResBundle*  m_pImpl;
	};


	struct ISkinDataSource;
	class ResourceManager;
	struct UIAPI IResourceManager : public IRootInterface
	{
		IResourceManager(ResourceManager*);
		ResourceManager*  GetImpl();

		IApplication*  GetUIApplication();
		void  ChangeSkinHLS(short h, short l, short s, int nFlag);
		bool  Save(IResBundle* pSkinRes);

		unsigned int  GetSkinResCount();
		IResBundle*  GetSkinResByIndex(unsigned int i);
		IResBundle*  GetSkinResByName(const wchar_t* szName);

        void  SetCurrentLanguage(const wchar_t*);
        const wchar_t*  GetCurrentLanguage();

	private:
		ResourceManager*  m_pImpl;
	};
}
#endif