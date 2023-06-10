#pragma once
#include "styleres.h"
#include "Src\Util\DataStruct\tree.h"
#include "Src\Util\DataStruct\list.h"

namespace UI
{
	interface IStyleManager;
	interface IUIElement;

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

    UIElement*  GetStyleXmlElem(LPCTSTR szId);

    // bool  ParseStyle(IMapAttribute* pMapAttrib);
    // bool  ReverseParseStyle(IListAttribute* pListAttrib);

public:
	// Editor
	void  OnStyleAdd(StyleResItem* p);
	void  OnStyleRemove(StyleResItem* p);
	void  OnStlyeModify(StyleResItem* p);
	void  OnStyleAttributeAdd(StyleResItem* p, LPCTSTR szKey);
	void  OnStyleAttributeRemove(StyleResItem* p, LPCTSTR szKey);
	void  OnStyleAttributeModify(StyleResItem* p, LPCTSTR szKey);

public:
	void  Clear();
	int   GetStyleCount( );
//  bool     GetStyleItemInfo( int nIndex, IStyleResItem** ppStyleItemInfo );
//  bool     GetStyleItemInfo( STYLE_SELECTOR_TYPE eType, LPCTSTR szSelectorID, IStyleResItem** ppStyleItemInfo );

    static HRESULT  UIParseStyleTagCallback(IUIElement*, ISkinRes* pSkinRes);

private:
    HRESULT  ParseNewElement(UIElement* pElem);
    void  OnNewChild(UIElement* pElem);

    bool  parse_inherit(tree<StyleResItem*>* pTreeItem, StyleRes* pStyleRes);
    bool  MakeInheritString(const STYLE_SELECTOR_TYPE& eStyletype, const String& strStypeName, TCHAR* szInherit);
    bool  ParseInheritString(const String& strInherit, STYLE_SELECTOR_TYPE& eStyletype, TCHAR* szStyleName);
    
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