#pragma once
#include "colorres.h"
#include "src/util/DataStruct/list.h"

namespace ui
{
struct UIElement;
struct IUIElement;
struct IColorManager;
class Resource;

class  ColorTagElementInfo
{
public:
	std::string  strId;  // 模块名
	UIElement*  pXmlElement;  // xml结点
};

class ColorManager
{
public:
    ColorManager(Resource* pSkinRes);
    ~ColorManager(void);

    ColorRes&  GetColorRes();
    int  ParseNewElement(UIElement* pElem);
    IColorManager&  GetIColorManager();

public:
    void  Clear();
    int   GetCount();
    IColorResItem*  GetColorItemInfo(int nIndex);

    bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

    static int  UIParseColorTagCallback(IUIElement*, IResource* pSkinRes);

private:
    void  OnNewChild(UIElement* pElem);

private:
    IColorManager*  m_pIColorManager;

	typedef UIList<ColorTagElementInfo> _MyList;
	typedef UIListItem<ColorTagElementInfo> _MyListItem;
	_MyList  m_listUIElement;

    ColorRes     m_resColor;              
    Resource*     m_pSkinRes;
};
}