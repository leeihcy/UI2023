#pragma once
#include "fontres.h"
#include "src/util/DataStruct/list.h"

namespace ui
{
	struct IFontManager;
	struct UIElement;
	struct IUIElement;

class  FontTagElementInfo
{
public:
	String  strId;  // ģ����
	UIElement*  pXmlElement;  // xml���
};

class FontManager
{
public:
	FontManager(ResBundle* p);
	~FontManager(void);
   
    IFontManager&  GetIFontManager();
	FontRes&  GetFontRes();

public:
	void       Clear();
	int        GetFontCount();
	IFontResItem*  GetFontItemInfo(int nIndex);
	
    static long  UIParseFontTagCallback(IUIElement*, IResBundle* pSkinRes);
private:
    long  ParseNewElement(UIElement* pElem);
    void  OnNewChild(UIElement* pElem, HDC);

private:
	// ���ݳ־ò�
    IFontManager*  m_pIFontManager;
   
	typedef  UIList<FontTagElementInfo>  _MyList;
	typedef  UIListItem<FontTagElementInfo>  _MyListItem;
	_MyList  m_listUIElement;

	// ��������
	FontRes   m_resFont;
	ResBundle*  m_pSkinRes;
};

}