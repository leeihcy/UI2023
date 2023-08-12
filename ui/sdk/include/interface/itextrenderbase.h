#ifndef _UI_ITEXTRENDERBASE_H_
#define _UI_ITEXTRENDERBASE_H_

#include "irenderbase.h"
#include "renderlibrary.h"
namespace ui {

enum TEXTRENDER_TYPE {
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

#define TEXTRENDER_STATE_MASK 0xFFFF0000
#define TEXTRENDER_STATE_NORMAL 0x00010000
#define TEXTRENDER_STATE_SELECTED 0x00020000
#define TEXTRENDER_STATE_SELECTED_NOTFOCUS 0x00030000

struct IRenderFont;
class TextRenderBase;
struct UIAPI ITextRenderBase : public IMessage {
  IRenderFont *GetRenderFont();

  long AddRef();
  long Release();

  void SetObject(IObject *pObject);
  IObject *GetObject();

  void SetType(const TEXTRENDER_TYPE &nType);
  TEXTRENDER_TYPE GetType();
  void SetTextAlignment(int nDrawFlag);
  int GetTextAlignment();
  void SetTextEffect(TEXT_EFFECT);
  TEXT_EFFECT GetTextEffect();
  void SetDrawTextParam(long w, long l);

  Size GetDesiredSize(const wchar_t *szText, int nLimitWidth = -1);
  void DrawState(IRenderTarget *pRenderTarget, const Rect *prc, int nState,
                 const wchar_t *szText, int nDrawTextFlag = -1);
  void Init();
  void CheckSkinTextureChanged();

  void Serialize(SERIALIZEDATA *pData);

protected:
  void _LoadFont(const wchar_t *szFontId, IRenderFont *&pRenderFont);
  const wchar_t *_SaveFont(IRenderFont *&pRenderFont);
  void _LoadDefalutFont(IRenderFont **ppRenderFont);
  void _LoadColor(const wchar_t *szColorId, Color *&pColor);
  const wchar_t *_SaveColor(Color *&pColor);

  ITextRenderBase();
  UI_DECLARE_INTERFACE(TextRenderBase);
};

struct AttributeSerializerWrap;
class ThemeTextRenderBase;

// struct UIAPI IThemeTextRenderBase : public ITextRenderBase
// {
// 	IThemeTextRenderBase() {};
//
//     const wchar_t*  GetThemeName();
//     HTHEME  GetTHEME();
// 	void  Serialize(AttributeSerializerWrap* ps);
//
// 	UI_DECLARE_INTERFACE(ThemeTextRenderBase);
// };

class SimpleTextRender;
struct UIAPI ISimpleTextRender : public ITextRenderBase {
  void SetRenderFont(IRenderFont *);
  void SetColor(Color *pColText);
  bool GetColor(COLORREF &color);

  static Uuid UUID() { return Uuid("230E9C5F-7F13-4307-85E7-088AB6F44C9D"); }
  UI_DECLARE_INTERFACE(SimpleTextRender);
};

class ContrastColorTextRender;
struct UIAPI IContrastColorTextRender : public ITextRenderBase {
  static Uuid UUID() { return Uuid("43851BC4-E18A-457E-AFCC-C4D0C638F376"); }
  UI_DECLARE_INTERFACE(ContrastColorTextRender)
};

class ContrastColorListTextRender;
struct UIAPI IContrastColorListTextRender : public ITextRenderBase {
  static Uuid UUID() { return Uuid("69A57DAA-1E49-4FCD-A334-03FA3A5FF420"); }
  UI_DECLARE_INTERFACE(ContrastColorListTextRender);
};

class ColorListTextRender;
struct UIAPI IColorListTextRender : public ITextRenderBase {
  void SetRenderFont(IRenderFont *);
  void SetCount(int nCount);
  void SetColor(int nIndex, COLORREF col);

  static Uuid UUID() { return Uuid("9646EBCD-6F6C-4748-9603-847C57CF29D7"); }
  UI_DECLARE_INTERFACE(ColorListTextRender);
};

class FontColorListTextRender;
struct UIAPI IFontColorListTextRender : public ITextRenderBase {
  void SetCount(int nCount);
  void SetColor(int nIndex, unsigned int color);
  void SetFont(int nIndex, IRenderFont *);


  UI_DECLARE_INTERFACE(FontColorListTextRender);
  static Uuid UUID() { return {0xFE20349B, 0x8D97, 0x4C86, {0x82, 0xA4, 0x4A, 0x27, 0xC8, 0xF1, 0x52, 0xC1}}; }
};



} // namespace ui

#endif // _UI_ITEXTRENDERBASE_H_