#pragma once
#include "colorres.h"
#include "src/util/DataStruct/list.h"

namespace ui
{
struct UIElement;
struct IUIElement;
struct IColorManager;
class ResBundle;

class  ColorTagElementInfo
{
public:
	std::wstring  strId;  // 模块名
	UIElement*  pXmlElement;  // xml结点
};

class ColorManager
{
public:
    ColorManager(ResBundle* pSkinRes);
    ~ColorManager(void);

    ColorRes&  GetColorRes();
    long  ParseNewElement(UIElement* pElem);
    IColorManager&  GetIColorManager();

public:
    void  Clear();
    int   GetColorCount();
    IColorResItem*  GetColorItemInfo(int nIndex);

    bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

    static long  UIParseColorTagCallback(IUIElement*, IResBundle* pSkinRes);

private:
    void  OnNewChild(UIElement* pElem);

private:
    IColorManager*  m_pIColorManager;

	typedef UIList<ColorTagElementInfo> _MyList;
	typedef UIListItem<ColorTagElementInfo> _MyListItem;
	_MyList  m_listUIElement;

    ColorRes     m_resColor;              
    ResBundle*     m_pSkinRes;
};
}