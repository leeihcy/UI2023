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
	std::string  strId;  // ģ����
	UIElement*  pXmlElement;  // xml���
};

class FontManager
{
public:
	FontManager(Resource* p);
	~FontManager(void);
   
    IFontManager&  GetIFontManager();
	FontRes&  GetFontRes();

public:
	void       Clear();
	int        GetFontCount();
	IFontResItem*  GetFontItemInfo(int nIndex);
	
    static int  UIParseFontTagCallback(IUIElement*, IResource* pSkinRes);
private:
    int  ParseNewElement(UIElement* pElem);
    void  OnNewChild(UIElement* pElem, HDC);

private:
	// ���ݳ־ò�
    IFontManager*  m_pIFontManager;
   
	typedef  UIList<FontTagElementInfo>  _MyList;
	typedef  UIListItem<FontTagElementInfo>  _MyListItem;
	_MyList  m_listUIElement;

	// ��������
	FontRes   m_resFont;
	Resource*  m_pSkinRes;
};

}