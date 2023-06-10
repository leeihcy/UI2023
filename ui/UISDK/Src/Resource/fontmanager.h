#pragma once
#include "fontres.h"
#include "Src\Util\DataStruct\list.h"

namespace UI
{
	interface IFontManager;
	interface UIElement;
	interface IUIElement;

class  FontTagElementInfo
{
public:
	String  strId;  // 模块名
	UIElement*  pXmlElement;  // xml结点
};

class FontManager
{
public:
	FontManager(SkinRes* p);
	~FontManager(void);
   
    IFontManager&  GetIFontManager();
	FontRes&  GetFontRes();

public:
	void       Clear();
	int        GetFontCount();
	IFontResItem*  GetFontItemInfo(int nIndex);
	
    static HRESULT  UIParseFontTagCallback(IUIElement*, ISkinRes* pSkinRes);
private:
    HRESULT  ParseNewElement(UIElement* pElem);
    void  OnNewChild(UIElement* pElem, HDC);

private:
	// 数据持久层
    IFontManager*  m_pIFontManager;
   
	typedef  UIList<FontTagElementInfo>  _MyList;
	typedef  UIListItem<FontTagElementInfo>  _MyListItem;
	_MyList  m_listUIElement;

	// 对象属性
	FontRes   m_resFont;
	SkinRes*  m_pSkinRes;
};

}