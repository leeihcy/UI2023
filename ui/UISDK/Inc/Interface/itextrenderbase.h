#ifndef _UI_ITEXTRENDERBASE_H_
#define _UI_ITEXTRENDERBASE_H_

#include "irenderbase.h"
#include "renderlibrary.h"
namespace UI
{

enum TEXTRENDER_TYPE
{
    TEXTRENDER_TYPE_NULL,
    TEXTRENDER_TYPE_SIMPLE,
    TEXTRENDER_TYPE_COLORLIST,
    TEXTRENDER_TYPE_FONTLIST,
    TEXTRENDER_TYPE_FONTCOLORLIST,
    TEXTRENDER_TYPE_ENDALPHAMASK,

    TEXTRENDER_TYPE_THEME_FIRST,
    TEXTRENDER_TYPE_THEME_MENU,
    TEXTRENDER_TYPE_THEME_LISTVIEW,
    TEXTRENDER_TYPE_THEME_LAST,

    TEXTRENDER_TYPE_CONTRAST_FIRST,
    TEXTRENDER_TYPE_CONTRASTCOLOR,
    TEXTRENDER_TYPE_CONTRASTCOLORLIST,
    TEXTRENDER_TYPE_CONTRAST_LAST,
};

#define TEXTRENDER_STATE_MASK                0xFFFF0000
#define TEXTRENDER_STATE_NORMAL              0x00010000
#define TEXTRENDER_STATE_SELECTED            0x00020000
#define TEXTRENDER_STATE_SELECTED_NOTFOCUS   0x00030000

interface IRenderFont;
class TextRenderBase; 
interface UIAPI ITextRenderBase : public IMessage
{
    IRenderFont*  GetRenderFont();

	long  AddRef();
	long  Release();

    void  SetObject(IObject* pObject);
    IObject*  GetObject();

    void  SetType(const TEXTRENDER_TYPE& nType);
    TEXTRENDER_TYPE  GetType();
    void  SetTextAlignment(int nDrawFlag);
    int   GetTextAlignment();
    void  SetTextEffect(TEXT_EFFECT);
    TEXT_EFFECT  GetTextEffect();
    void  SetDrawTextParam(WPARAM w, LPARAM l);

	SIZE  GetDesiredSize(LPCTSTR szText, int nLimitWidth=-1);
	void  DrawState(IRenderTarget* pRenderTarget, LPCRECT prc, int nState, LPCTSTR szText, int nDrawTextFlag=-1);
	void  Init();
	void  CheckSkinTextureChanged();

    void  Serialize(SERIALIZEDATA* pData);
protected:
	void  _LoadFont(LPCTSTR szFontId, IRenderFont*& pRenderFont);
	LPCTSTR  _SaveFont(IRenderFont*& pRenderFont);
	void  _LoadDefalutFont(IRenderFont** ppRenderFont);
	void  _LoadColor(LPCTSTR szColorId, Color*& pColor);
	LPCTSTR  _SaveColor(Color*& pColor);

	ITextRenderBase();
	UI_DECLARE_INTERFACE(TextRenderBase);
};

interface AttributeSerializerWrap;
class ThemeTextRenderBase;

// interface UIAPI IThemeTextRenderBase : public ITextRenderBase
// {
// 	IThemeTextRenderBase() {};
// 
//     LPCTSTR  GetThemeName();
//     HTHEME  GetTHEME();
// 	void  Serialize(AttributeSerializerWrap* ps);
// 
// 	UI_DECLARE_INTERFACE(ThemeTextRenderBase);
// };

class SimpleTextRender;
interface __declspec(uuid("230E9C5F-7F13-4307-85E7-088AB6F44C9D"))
UIAPI ISimpleTextRender : public ITextRenderBase
{
    void  SetRenderFont(IRenderFont*);
    void  SetColor(Color* pColText);
	bool  GetColor(COLORREF& color);

	UI_DECLARE_INTERFACE(SimpleTextRender);
};

class ContrastColorTextRender;
interface __declspec(uuid("43851BC4-E18A-457E-AFCC-C4D0C638F376"))
UIAPI IContrastColorTextRender : public ITextRenderBase
{
    UI_DECLARE_INTERFACE(ContrastColorTextRender)
};

class ContrastColorListTextRender;
interface __declspec(uuid("69A57DAA-1E49-4FCD-A334-03FA3A5FF420"))
UIAPI IContrastColorListTextRender : public ITextRenderBase
{
    UI_DECLARE_INTERFACE(ContrastColorListTextRender);
};

class ColorListTextRender;
interface __declspec(uuid("9646EBCD-6F6C-4748-9603-847C57CF29D7"))
UIAPI IColorListTextRender : public ITextRenderBase
{
    void  SetRenderFont(IRenderFont*);
    void  SetCount(int nCount);
    void  SetColor(int nIndex, COLORREF col);

	UI_DECLARE_INTERFACE(ColorListTextRender);
};

class FontColorListTextRender;
interface __declspec(uuid("FE20349B-8D97-4C86-82A4-4A27C8F152C1"))
UIAPI IFontColorListTextRender : public ITextRenderBase
{
    void  SetCount(int nCount);
	void  SetColor(int nIndex, DWORD color);
	void  SetFont(int nIndex, IRenderFont*);

	UI_DECLARE_INTERFACE(FontColorListTextRender);
};
}

#endif // _UI_ITEXTRENDERBASE_H_