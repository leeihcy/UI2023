#pragma once
#include "colorres.h"
#include "Src\Util\DataStruct\list.h"

namespace UI
{
interface UIElement;
interface IUIElement;
interface IColorManager;

class  ColorTagElementInfo
{
public:
	String  strId;  // 模块名
	UIElement*  pXmlElement;  // xml结点
};

class ColorManager
{
public:
    ColorManager(SkinRes* pSkinRes);
    ~ColorManager(void);

    ColorRes&  GetColorRes();
    HRESULT  ParseNewElement(UIElement* pElem);
    IColorManager&  GetIColorManager();

public:
    void  Clear();
    int   GetColorCount();
    IColorResItem*  GetColorItemInfo(int nIndex);

    bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

    static HRESULT  UIParseColorTagCallback(IUIElement*, ISkinRes* pSkinRes);

private:
    void  OnNewChild(UIElement* pElem);

private:
    IColorManager*  m_pIColorManager;

	typedef UIList<ColorTagElementInfo> _MyList;
	typedef UIListItem<ColorTagElementInfo> _MyListItem;
	_MyList  m_listUIElement;

    ColorRes     m_resColor;              
    SkinRes*     m_pSkinRes;
};
}