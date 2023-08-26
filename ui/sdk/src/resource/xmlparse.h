#pragma once
#include "UISDK\Kernel\Src\XmlParse\iresparse.h"

namespace ui
{
class Object;


#if 0
//
//	image信息描述文件
//
class CXmlImageParse : public IImageParse
{
protected:
	CXmlImageParse();
	~CXmlImageParse();

	friend class IParseClassFactory;
public:
	virtual  bool Create();
	virtual  bool Save( ImageRes*  pImageInfo );
	virtual  bool Load( ImageRes*  pImageInfo, CursorRes* pCursorInfo, GifRes* pGifInfo);
	virtual  bool Reload( ImageRes* pImageInfo, CursorRes* pCursorInfo, GifRes* pGifInfo);

	virtual  bool InsertImage( ImageResItem *pImageItemInfo );
	virtual  bool ModifyImage( ImageResItem *pImageItemInfo );
	virtual  bool RemoveImage( const std::string& strID );

private:
	bool  load_from_file( CMarkup* pXml, const std::string& strDataSource, ImageRes *pImageInfo, CursorRes* pCursorInfo, GifRes* pGifInfo);
	bool  insert_image_2_xml( ImageResItem *pImageItemInfo );
	bool  clear_save( ImageRes*  pImageInfo );
private:
	CMarkup  m_xml;
};

//
//	color信息描述文件
//
class CXmlColorParse : public IColorParse
{
protected:
	CXmlColorParse();
	~CXmlColorParse();

	friend class IParseClassFactory;
public:
	virtual  bool Create();
	virtual  bool Save( ColorRes*  pColorInfo );
	virtual  bool Load( ColorRes*  pColorInfo );
	// 从temp文件中重新加载
	virtual  bool Reload( ColorRes* pColorInfo );
	// 插入
	virtual  bool InsertColor( ColorResItem *pColorItemInfo );
	// 修改
	virtual  bool ModifyColor( ColorResItem *pColorItemInfo );
	// 删除
	virtual  bool RemoveColor( const std::string& strID );

private:
	bool  load_from_file( CMarkup* pXml, const std::string& strDataSource, ColorRes *pColorInfo );
	bool  insert_color_2_xml( ColorResItem *pColorInfoInfo );
	bool  clear_save(ColorRes* pColorInfo );

private:
	CMarkup  m_xml;
};

//
// font信息描述文件
//
class CXmlFontParse : public IFontParse
{
protected:
	CXmlFontParse();
	~CXmlFontParse();

	friend class IParseClassFactory;
public:
	virtual  bool Create();
	virtual  bool Save( FontRes*  pColorInfo );
	virtual  bool Load( FontRes* pFontInfo );
	virtual  bool Reload( FontRes* pFontInfo );

	virtual  bool InsertFont( FontResItem *pFontItemInfo );
	virtual  bool ModifyFont( FontResItem *pFontItemInfo );
	virtual  bool RemoveFont( const std::string& strID );

private:
	bool  load_from_file( CMarkup* pXml, const std::string& strDataSource, FontRes *pFontInfo );
	bool  insert_font_2_xml( FontResItem *pFontItemInfo );
	bool  clear_save( FontRes*  pFontInfo );
private:
	CMarkup  m_xml;
};

//
// style信息描述文件
//
class CXmlStyleParse : public IStyleParse
{
protected:
	CXmlStyleParse();
	~CXmlStyleParse();

	friend class IParseClassFactory;
public:
	virtual bool  Create();
	virtual bool  Save( StyleRes* pStyleInfo );
	virtual bool  Load( StyleRes* pStyleInfo );
	virtual bool  Reload( StyleRes* pStyleInfo );
	virtual bool  InsertStyle( StyleResItem *pStyleItemInfo );
	virtual bool  ModifyStyle( StyleResItem *pStyleItemInfo );
	virtual bool  RemoveStyle( STYLE_SELECTOR_TYPE type, const std::string& strID );
	virtual bool  InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const std::string& strID, const std::string& strKey, const std::string& strValue );
	virtual bool  ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const std::string& strID, const std::string& strKey, const std::string& strValue );
	virtual bool  RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const std::string& strID, const std::string& strKey );

private:
	bool  find_elem_in_xml( STYLE_SELECTOR_TYPE type, const std::string& strID );
	bool  find_elem_under_style( STYLE_SELECTOR_TYPE type, const std::string& strID );
	bool  load_from_file( CMarkup* pXml, const std::string& strDataSource, StyleRes *pStyleInfo );
	bool  insert_style_2_xml( StyleResItem *pStyleItemInfo );

	bool  parse_inherit(tree<StyleResItem*>* pTreeItem, StyleRes* pPojoStyle);
private:
	CMarkup  m_xml;
};

class MenuBase;
//
// layout信息描述文件
//
class CXmlLayoutParse : public ILayoutParse
{
protected:
	CXmlLayoutParse();
	~CXmlLayoutParse();

	friend class IParseClassFactory;
public:
	virtual  bool Create();
	virtual  bool Save( LayoutRes* pLayoutInfo );
	virtual  bool Load( LayoutRes* pLayoutInfo );
	virtual  bool LoadLayout(Object* pRootObj, LayoutRes* pResLayout );
	virtual  bool ReLoadLayout(Object* pRootObj, list<Object*>& listAllChild );
#if 0 // -- 架构改造
	virtual  Menu* LoadMenu( const std::string& strMenuId );
#endif
 //   virtual IObject*  LoadRootObject(const wchar_t* szTag, const wchar_t szId);

	// 编辑器专用函数
	virtual bool LoadAllWindowNodeInfo(LayoutWindowNodeInfo* pInfoArray, int* pArraySize);
private:
	IStyleManager* getStyleMgr(); 

	bool  load_from_file( CMarkup* pXml, const std::string& strDataSource, LayoutRes *pLayoutInfo );

	Object*   mapXmlToClass(const std::string& strXmlName );
	bool       loadAttributeForCurrentObjectInXml(Object* pObj, LayoutRes* pResLayout, bool bReload=false);
	bool       loadObjects(Object* pObjParent, LayoutRes* pResLayout );
	bool       reloadObjects(Object* pObjParent, list<Object*>& listAllChild);
#if 0 // -- 架构改造
	bool       loadMenuItems(MenuBase* pParentMenu);
#endif
	bool       loadRenderChain(Object* pObjParent);

private:
	CMarkup  m_xml;
};
#endif
//
//	类工厂
//
//		由于这些parse对象都是由service对象在析构函数中自己释放的
//		因此将这些对象的生成形式由new/delete改成 create/release，
//		避免使用时产生不对称，只有new而找不到delete
//		另一方面也便于以后的实现类整体替换
//
class IParseClassFactory
{
public:

#if 0
	static IImageParse*  CreateImageParseInstance(IApplication* pUIApp, const std::string& strDataSource)  
	{ 
		IImageParse* p = new CXmlImageParse(); 
		p->SetDataSource(strDataSource);
		p->SetUIApplicationPtr(pUIApp);
		return p;
	}

	static IColorParse*  CreateColorParseInstance(IApplication* pUIApp, const std::string& strDataSource)  
	{ 
		IColorParse* p = new CXmlColorParse(); 
		p->SetDataSource(strDataSource);
		p->SetUIApplicationPtr(pUIApp);
		return p;
	}

	static IFontParse*  CreateFontParseInstance(IApplication* pUIApp, const std::string& strDataSource)  
	{ 
		IFontParse* p = new CXmlFontParse(); 
		p->SetDataSource(strDataSource);
		p->SetUIApplicationPtr(pUIApp);
		return p;
	}

	static IStyleParse*  CreateStyleParseInstance(IApplication* pUIApp, const std::string& strDataSource ) 
	{ 
		IStyleParse* p = new CXmlStyleParse(); 
		p->SetDataSource(strDataSource);
		p->SetUIApplicationPtr(pUIApp);
		return p;
	}

	static ILayoutParse*  CreateLayoutParseInstance(IApplication* pUIApp, const std::string& strDataSource )
	{ 
		ILayoutParse* p = new CXmlLayoutParse(); 
		p->SetDataSource(strDataSource);
		p->SetUIApplicationPtr(pUIApp);
		return p;
	}
#endif
};
}