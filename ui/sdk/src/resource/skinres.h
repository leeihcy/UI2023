#pragma once
#include "include/interface/iskindatasource.h"
#include "colormanager.h"
#include "imagemanager.h"
#include "fontmanager.h"
#include "stylemanager.h"
#include "layoutmanager.h"
#include "i18nmanager.h"

namespace ui
{
class UIApplication;
class Object;
class SkinManager;
struct UIDocument;
struct SkinDataSource;
struct SKIN_HLS_INFO;
struct ISkinManager;

enum SKIN_RES_LOAD_STATE
{
    SKIN_RES_LOAD_STATE_NONE,     // 仅new SkinRes
    SKIN_RES_LOAD_STATE_LOADED,   // 已加载    
};


//
//	一款皮肤资源管理
//	
class SkinRes
{
public:
    SkinRes(SkinManager& o);
	~SkinRes();

	bool  Load();
	bool  Unload();
	bool  Save();
	const wchar_t*  GetName();
	const wchar_t*  GetPath();
	void  SetName(const wchar_t*);
	void  SetPath(const wchar_t*);
	SkinDataSource*  CreateDataSource(SKIN_PACKET_TYPE eType); 
	bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

	ISkinRes*        GetISkinRes();
    UIApplication*   GetUIApplication();
    ISkinManager&    GetISkinManager();

//     IImageManager*   GetIImageManager()   { return m_mgrImage.GetIImageManager(); }
//     IColorManager*   GetIColorManager()   { return m_mgrColor.GetIColorManager(); }
//     IFontManager*    GetIFontManager()    { return m_mgrFont.GetIFontManager(); }
//     IStyleManager*   GetIStyleManager()   { return m_mgrStyle.GetIStyleManager(); }
//     ILayoutManager*  GetILayoutManager()  { return m_mgrLayout.GetILayoutManager(); }
//     IImageRes*       GetIImageRes()       { return m_mgrImage.GetImageRes().GetIImageRes(); }
//     IFontRes*        GetIFontRes()        { return m_mgrFont.GetFontRes().GetIFontRes(); }
//     IColorRes*       GetIColorRes()       { return m_mgrColor.GetColorRes().GetIColorRes(); }
//     IStyleRes*       GetIStyleRes()       { return m_mgrStyle.GetStyleRes().GetIStyleRes(); }
	

	SkinManager&     GetSkinMgr()         { return m_mgrSkinRef; } //  内部调用
	ImageManager&    GetImageManager()    { return m_mgrImage;  }
	ColorManager&    GetColorManager()    { return m_mgrColor;  }
	FontManager&     GetFontManager()     { return m_mgrFont;   }
	StyleManager&    GetStyleManager()    { return m_mgrStyle;  }
	LayoutManager&   GetLayoutManager()   { return m_mgrLayout; }
	SKIN_HLS_INFO*   GetHLSInfo()         { return m_pHLSInfo; }
	SkinDataSource*  GetDataSource()      { return m_pDataSource; }
	I18nManager&     GetI18nManager()     { return m_mgrI18n; }

	ImageRes&  GetImageRes();
	ColorRes&  GetColorRes();
	FontRes&   GetFontRes();
    StyleRes&  GetStyleRes();
	I18nRes&   GetI18nRes();

    unsigned int  GetXmlDocCount()   { return (unsigned int)m_listDoc.size(); }
	UIDocument*  GetXmlDocByName(const wchar_t* szName);
    UIDocument*  GetXmlDoc(unsigned int nIndex);

	void  OnNewUIDocument(UIDocument* pDoc);    
	
	void  SetParentSkinRes(SkinRes*);
	SkinRes*  GetParentSkinRes();

private:
    ISkinRes*  m_pISkinRes;
	SkinManager&  m_mgrSkinRef;

	// 父一级的资源，本地找不着时，向上查找
	// 用于插件资源中引用父一级的资源
	SkinRes*  m_pParentSkinRes;  

	String  m_strSkinResName;
    SkinDataSource*  m_pDataSource;
	ColorManager   m_mgrColor;
	ImageManager   m_mgrImage;
	FontManager    m_mgrFont;
	StyleManager   m_mgrStyle;
	LayoutManager  m_mgrLayout;
	I18nManager    m_mgrI18n;

	SKIN_RES_LOAD_STATE  m_eLoadState;
    SKIN_HLS_INFO*  m_pHLSInfo;

    typedef std::vector<UIDocument*> _DocList;
    _DocList  m_listDoc;   // 各个xml文档指针
};

}