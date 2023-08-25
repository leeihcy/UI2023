#pragma once
#include "src/render/renderbase.h"
#include "include/interface/icolorrender.h"
#include "include/util/struct.h"

namespace ui
{


//
// 背景颜色+边框颜色（两者都是可选的）
//
class ColorRender : public RenderBase
{
public:
    ColorRender(IColorRender* p);
    ~ColorRender();

    UI_DECLARE_RENDERBASE(ColorRender, XML_RENDER_TYPE_COLOR, RENDER_TYPE_COLOR)

	UI_BEGIN_MSG_MAP()
        UIMSG_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_QUERYINTERFACE(ColorRender)
        UIMSG_SERIALIZE(OnSerialize)
    UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)


	void  OnSerialize(SerializeParam* pData);
	void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

public:
	void  SetBkColor(COLORREF col);
    void  SetBorderColor(COLORREF col);
    void  SetBorderRegion(const Rect* prc);

public:
    IColorRender*  m_pIColorRender;

	Color*  m_pBkColor;
    Color*  m_pBorderColor;
    Rect  m_rcBorder;
};

// 特用于某些控件需要使用系统主题颜色背景，当系统主题改变时，相应的颜色也需要改变 
class SysColorRender : public /*Theme*/RenderBase
{
public:
    SysColorRender(ISysColorRender* p);
    ~SysColorRender();

    UI_DECLARE_RENDERBASE(SysColorRender, XML_RENDER_TYPE_SYSCOLOR, RENDER_TYPE_THEME_SYSCOLOR)

	UI_BEGIN_MSG_MAP()
        UIMSG_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_QUERYINTERFACE(SysColorRender)
        UIMSG_SERIALIZE(OnSerialize)
    UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)


	void  OnSerialize(SerializeParam* pData);
	void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

    void  SetBkColor(int nColorIndex);
    void  SetBorderColor(int nColorIndex);

    // 由于改成每次重新获取颜色值，因此不需要响应主题改变的消息
	// virtual const wchar_t* GetThemeName() { return nullptr; }
	// virtual void  OnThemeChanged();

public:
    ISysColorRender*  m_pISysColorRender;
	int  m_nBkColorIndex;
	int  m_nBorderColorIndex;
    REGION4  m_rcBorder;

	// COLORREF    m_bkColor;     // 由于在WM_THEMECHANGED消息中，直接调用GetSysColor获取到的值仍然有可能是旧的
	// COLORREF    m_borderColor; // 因此这里放弃保存颜色值，而是每次绘制时都调用GetSysColor
};

// 横向渐变颜色背景
class GradientRender : public RenderBase
{
public:
    GradientRender(IGradientRender* p);
    ~GradientRender();

    UI_DECLARE_RENDERBASE(GradientRender, XML_RENDER_TYPE_GRADIENT, RENDER_TYPE_GRADIENT)

	UI_BEGIN_MSG_MAP()
        UIMSG_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_QUERYINTERFACE(GradientRender)
        UIMSG_SERIALIZE(OnSerialize)
    UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)


	void  OnSerialize(SerializeParam* pData);
	void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

    void  LoadFromColor(const wchar_t* sz){
        _LoadColor(sz, m_pColorFrom);
    }
    void  LoadToColor(const wchar_t* sz){
        _LoadColor(sz, m_pColorTo);
    }
    void  LoadBorderColor(const wchar_t* sz){
        _LoadColor(sz, m_pBorderColor);
    }
    const wchar_t*  SaveFromColor(){
        return _GetColorId(m_pColorFrom);
    }
    const wchar_t*  SaveToColor(){
        return _GetColorId(m_pColorTo);
    }
    const wchar_t*  SaveBorderColor(){
        return _GetColorId(m_pBorderColor);
    }
public:
    IGradientRender*  m_pIGradientRender;
	Color*  m_pColorFrom;
	Color*  m_pColorTo;
    Color*  m_pBorderColor;
    REGION4  m_rcBorder;

    int  m_nGradientDir;
};



//
//	直接使用颜色值作为背景
//
class ColorListRender : public RenderBase
{
public:
    ColorListRender(IColorListRender* p);
    ~ColorListRender();

    UI_DECLARE_RENDERBASE(ColorListRender, XML_RENDER_TYPE_COLORLIST, RENDER_TYPE_COLORLIST)

	UI_BEGIN_MSG_MAP()
        UIMSG_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_QUERYINTERFACE(ColorListRender)
        UIMSG_SERIALIZE(OnSerialize)
    UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)
	
	void  Clear();

    void  OnSerialize(SerializeParam* pData);
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

	void  SetStateColor(int nState, COLORREF colorBk, bool bSetBk, COLORREF colBorder, bool bSetBorder);
	
    void  SetCount(int n);
    int  GetCount();
    void  LoadBkColor(const wchar_t* szColor);
    const wchar_t*  SaveBkColor();
    void  LoadBorderColor(const wchar_t* szColor);
    const wchar_t*  SaveBorderColor();

private:
    IColorListRender*  m_pIColorListRender;
	std::vector<Color*>  m_vBkColor;
	std::vector<Color*>  m_vBorderColor;
	int   m_nCount; 

};
}