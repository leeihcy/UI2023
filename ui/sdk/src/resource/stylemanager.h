#pragma once
#include "styleres.h"
#include "src/util/DataStruct/tree.h"
#include "src/util/DataStruct/list.h"

namespace ui
{
	struct IStyleManager;
	struct IUIElement;

class  StyleTagElementInfo
{
public:
	String  strId;  // 模块名
	UIElement*  pXmlElement;  // xml结点
};

class StyleManager
{
public:
	StyleManager(SkinRes* p);
	~StyleManager(void);
	
	StyleRes&  GetStyleRes();
    IStyleManager&  GetIStyleManager();

    UIElement*  GetStyleXmlElem(const wchar_t* szId);

    // bool  ParseStyle(IMapAttribute* pMapAttrib);
    // bool  ReverseParseStyle(IListAttribute* pListAttrib);

public:
	// Editor
	void  OnStyleAdd(StyleResItem* p);
	void  OnStyleRemove(StyleResItem* p);
	void  OnStlyeModify(StyleResItem* p);
	void  OnStyleAttributeAdd(StyleResItem* p, const wchar_t* szKey);
	void  OnStyleAttributeRemove(StyleResItem* p, const wchar_t* szKey);
	void  OnStyleAttributeModify(StyleResItem* p, const wchar_t* szKey);

public:
	void  Clear();
	int   GetStyleCount( );
//  bool     GetStyleItemInfo( int nIndex, IStyleResItem** ppStyleItemInfo );
//  bool     GetStyleItemInfo( STYLE_SELECTOR_TYPE eType, const wchar_t* szSelectorID, IStyleResItem** ppStyleItemInfo );

    static long  UIParseStyleTagCallback(IUIElement*, ISkinRes* pSkinRes);

private:
    long  ParseNewElement(UIElement* pElem);
    void  OnNewChild(UIElement* pElem);

    bool  parse_inherit(tree<StyleResItem*>* pTreeItem, StyleRes* pStyleRes);
    bool  MakeInheritString(const STYLE_SELECTOR_TYPE& eStyletype, const String& strStypeName, wchar_t* szInherit);
    bool  ParseInheritString(const String& strInherit, STYLE_SELECTOR_TYPE& eStyletype, wchar_t* szStyleName);
    
private:
	// 数据持久层
    IStyleManager*  m_pIStyleManager;
    
	typedef UIList<StyleTagElementInfo> _MyList;
	typedef UIListItem<StyleTagElementInfo> _MyListItem;
	_MyList  m_listUIElement;

	// 对象属性
	StyleRes  m_StyleRes;
	SkinRes*  m_pSkinRes;
};;

}