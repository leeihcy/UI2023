#pragma once
#include <memory>
#if 0 // defined(OS_WIN)
#include "Vsstyle.h"
#include "uxtheme.h"
#pragma comment(lib, "uxtheme.lib")
#endif

#include "include/interface/itextrenderbase.h"
#include "include/macro/uimsg.h"
#include "src/object/message.h"

namespace ui {
class AttributeSerializer;
class Application;
class Object;
struct TEXTRENDERBASE_DRAWSTATE;
struct IFontRes;
class Resource;

class TextRenderBase : public Message {
public:
  TextRenderBase(ITextRenderBase *p);
  ~TextRenderBase();

  // UI_BEGIN_MSG_MAP()
  // UIMSG_GETDESIREDSIZE2(OnGetDesiredSize)
  // UI_END_MSG_MAP()

  void Serialize(AttributeSerializer *ps);
  void OnGetDesiredSize(Size *pSize, GETDESIREDSIZEINFO *pInfo);
  Size GetDesiredSize(const char *szText, int nLimitWidth = -1);

public:
  void SetRenderFont(IRenderFont *pFont) {}

  void SetTextAlignment(int nDrawFlag);
  int GetTextAlignment() { return m_nDrawTextFlag; }
  void SetTextEffect(TEXT_EFFECT e) { m_eDrawTextEffect = e; }
  TEXT_EFFECT GetTextEffect() { return m_eDrawTextEffect; }
  void SetDrawTextParam(int w, int l) {
    m_wparamDrawText = w;
    m_lparamDrawText = l;
  }

  void SetObject(Object *pObject) { m_pObject = pObject; }
  Object *GetObject() { return m_pObject; }

  void SetTextRenderType(const TEXTRENDER_TYPE &nType) {
    m_nTextRenderType = nType;
  }
  TEXTRENDER_TYPE GetTextRenderType() { return m_nTextRenderType; }

  IColorRes *GetSkinColorRes();
  IFontRes *GetSkinFontRes();
  Resource *GetResource();

  bool IsThemeRender();
  void CheckSkinTextureChanged();

  std::shared_ptr<IRenderFont> _LoadFont(const char *szFontId);
  const char *_SaveFont(IRenderFont *pRenderFont);
  std::shared_ptr<IRenderFont> _LoadDefalutFont();
  void _LoadColor(const char *szColorId, Color *&pColor);
  const char *_SaveColor(Color *&pColor);

  void LoadHaloColor(const char *szColorId);
  const char *GetHaloColorId();

protected:
  ITextRenderBase *m_pITextRenderBase;

  Object *m_pObject;                 // 绑定的对象，要绘制谁的文字
  TEXTRENDER_TYPE m_nTextRenderType; // 自己的类型
  int m_nDrawTextFlag;              // DrawText的flag标识

  TEXT_EFFECT m_eDrawTextEffect;
  Color *m_pColorTextBkgnd;
  int m_wparamDrawText;
  int m_lparamDrawText;
};

class SimpleTextRender : public TextRenderBase {
public:
  SimpleTextRender(ISimpleTextRender *p);
  ~SimpleTextRender();

  UI_DECLARE_TEXTRENDERBASE(SimpleTextRender, XML_TEXTRENDER_TYPE_SIMPLE,
                            TEXTRENDER_TYPE_SIMPLE)

  // UI_BEGIN_MSG_MAP()
  // UIMSG_TEXTRENDERBASE_DRAWSTATE(DrawState)
  // UIMSG_GETRENDERFONT(GetRenderFont)
  // UIMSG_QUERYINTERFACE(SimpleTextRender)
  // UIMSG_SERIALIZE(OnSerialize)
  // UI_END_MSG_MAP_CHAIN_PARENT(TextRenderBase)

  void OnSerialize(SerializeParam *pData);
  void DrawState(TEXTRENDERBASE_DRAWSTATE *pDrawStruct);
  IRenderFont *GetRenderFont() { return m_pRenderFont.get(); }

  void SetRenderFont(std::shared_ptr<IRenderFont>);
  void SetColor(Color *pColText);
  bool GetColor(Color &color);

  void LoadFont(const char *szFontId);
  const char *GetFontId();

protected:
  ISimpleTextRender *m_pISimpleTextRender;
  Color *m_pColorText;
  std::shared_ptr<IRenderFont> m_pRenderFont;

#ifdef EDITOR_MODE
  std::string m_strFontId;
#endif
};

// 根据皮肤颜色，自适应设置文本的颜色是白还是黑
class ContrastColorTextRender : public TextRenderBase {
public:
  ContrastColorTextRender(IContrastColorTextRender *p);
  ~ContrastColorTextRender();

  UI_DECLARE_TEXTRENDERBASE(ContrastColorTextRender,
                            XML_TEXTRENDER_TYPE_CONTRAST_COLOR,
                            TEXTRENDER_TYPE_CONTRASTCOLOR)

  // UI_BEGIN_MSG_MAP()
  // UIMSG_TEXTRENDERBASE_DRAWSTATE(DrawState)
  // UIMSG_GETRENDERFONT(GetRenderFont)
  // UIMSG_QUERYINTERFACE(ContrastColorTextRender)
  // UIMSG_HANDLER_EX(UI_MSG_SKINTEXTURECHANGED, OnSkinTextureChanged)
  // UIMSG_SERIALIZE(OnSerialize)
  // UI_END_MSG_MAP_CHAIN_PARENT(TextRenderBase)

public:
  void SetRenderFont(std::shared_ptr<IRenderFont> );

protected:
  void OnSerialize(SerializeParam *pData);
  void DrawState(TEXTRENDERBASE_DRAWSTATE *pDrawStruct);
  IRenderFont *GetRenderFont() { return m_pRenderFont.get(); }
  int OnSkinTextureChanged(unsigned int, int, int);

  void LoadFont(const char *szFontId) { m_pRenderFont = _LoadFont(szFontId); }
  const char *GetFontId() { return _SaveFont(m_pRenderFont.get()); }
  void LoadColor(const char *szColorId) {
    _LoadColor(szColorId, m_pColorText);
  }
  const char *GetColorId() { return _SaveColor(m_pColorText); }

private:
  IContrastColorTextRender *m_pIContrastColorTextRender;

  Color *m_pColorText;
  std::shared_ptr<IRenderFont> m_pRenderFont;

  bool m_bNeedCalcColor;
};

class ContrastColorListTextRender : public TextRenderBase {
public:
  ContrastColorListTextRender(IContrastColorListTextRender *p);
  ~ContrastColorListTextRender();

  UI_DECLARE_TEXTRENDERBASE(ContrastColorListTextRender,
                            XML_TEXTRENDER_TYPE_CONTRASTCOLORLIST,
                            TEXTRENDER_TYPE_CONTRASTCOLORLIST)

  // UI_BEGIN_MSG_MAP()
  // UIMSG_TEXTRENDERBASE_DRAWSTATE(DrawState)
  // UIMSG_GETRENDERFONT(GetRenderFont)
  // UIMSG_HANDLER_EX(UI_MSG_SKINTEXTURECHANGED, OnSkinTextureChanged)
  // UIMSG_QUERYINTERFACE(ContrastColorListTextRender)
  // UIMSG_SERIALIZE(OnSerialize)
  // UI_END_MSG_MAP_CHAIN_PARENT(TextRenderBase)

  void OnSerialize(SerializeParam *pData);
  void DrawState(TEXTRENDERBASE_DRAWSTATE *pDrawStruct);
  int OnSkinTextureChanged(unsigned int, int, int);

  IRenderFont *GetRenderFont();
  void SetRenderFont(std::shared_ptr<IRenderFont>);

  void SetCount(int nCount);
  int GetCount();

  void LoadFont(const char *szFontId) { m_pRenderFont = _LoadFont(szFontId); }
  const char *GetFontId() { return _SaveFont(m_pRenderFont.get()); }

protected:
  void Clear();

private:
  IContrastColorListTextRender *m_pIContrastColorListTextRender;

  std::vector<Color *> m_vTextColor;
  std::shared_ptr<IRenderFont> m_pRenderFont;
  int m_nCount;
};

//////////////////////////////////////////////////////////////////////////

class ColorListTextRender : public TextRenderBase {
public:
  ColorListTextRender(IColorListTextRender *p);
  ~ColorListTextRender();

  UI_DECLARE_TEXTRENDERBASE(ColorListTextRender, XML_TEXTRENDER_TYPE_COLORLIST,
                            TEXTRENDER_TYPE_COLORLIST)

  // UI_BEGIN_MSG_MAP()
  // UIMSG_TEXTRENDERBASE_DRAWSTATE(DrawState)
  // UIMSG_GETRENDERFONT(GetRenderFont)
  // UIMSG_QUERYINTERFACE(ColorListTextRender)
  // UIMSG_SERIALIZE(OnSerialize)
  // UI_END_MSG_MAP_CHAIN_PARENT(TextRenderBase)

  void OnSerialize(SerializeParam *pData);
  void DrawState(TEXTRENDERBASE_DRAWSTATE *pDrawStruct);

  IRenderFont *GetRenderFont();
  void SetRenderFont(std::shared_ptr<IRenderFont>);

  void LoadFont(const char *szFontId);
  const char *GetFontId();

  void SetCount(int nCount);
  int GetCount();

  void LoadColor(const char *szText);
  const char *GetColor();

  void SetColor(int nIndex, Color col);

protected:
  void Clear();

private:
  IColorListTextRender *m_pIColorListTextRender;

  std::vector<Color*> m_vTextColor;
  std::shared_ptr<IRenderFont> m_pRenderFont;
  int m_nCount;

#ifdef EDITOR_MODE
  std::string m_strFontId;
#endif
};
#if 0
	class FontListTextRender : public TextRenderBase
	{
	private:
		Color*  m_pTextColor;
		vector<IRenderFont*>  m_vTextFont;
	};
#endif

class FontColorListTextRender : public TextRenderBase {
public:
  FontColorListTextRender(IFontColorListTextRender *p);
  ~FontColorListTextRender();

  UI_DECLARE_TEXTRENDERBASE(FontColorListTextRender,
                            XML_TEXTRENDER_TYPE_FONTCOLORLIST,
                            TEXTRENDER_TYPE_FONTCOLORLIST)

  // UI_BEGIN_MSG_MAP()
  // UIMSG_TEXTRENDERBASE_DRAWSTATE(DrawState)
  // UIMSG_GETRENDERFONT(GetRenderFont)
  // UIMSG_QUERYINTERFACE(FontColorListTextRender)
  // UIMSG_SERIALIZE(OnSerialize)
  // UI_END_MSG_MAP_CHAIN_PARENT(TextRenderBase)

  void OnSerialize(SerializeParam *pData);
  void DrawState(TEXTRENDERBASE_DRAWSTATE *pDrawStruct);

  IRenderFont *GetRenderFont();
  void SetRenderFont(std::shared_ptr<IRenderFont>);

  void SetCount(int nCount);
  int GetCount();

  void LoadColor(const char *szText);
  const char *GetColor();
  void LoadFont(const char *szText);
  const char *GetFont();

  void SetColor(int nIndex, unsigned int color);
  void SetFont(int nIndex, std::shared_ptr<IRenderFont>);

protected:
  void Clear();

private:
  IFontColorListTextRender *m_pIFontColorListTextRender;

  std::vector<Color *> m_vTextColor;
  std::vector<std::shared_ptr<IRenderFont> > m_vTextFont;

  int m_nCount;
};

#if 0
//
//	主题背景按钮
//
class ThemeTextRenderBase : public TextRenderBase
{
public:
	ThemeTextRenderBase(IThemeTextRenderBase* p);
	~ThemeTextRenderBase();

	// UI_BEGIN_MSG_MAP()
	// 	MSG_WM_THEMECHANGED(OnThemeChanged)
  //       UIMSG_INITIALIZE(OnInit)
  //   UI_END_MSG_MAP_CHAIN_PARENT(TextRenderBase)

    void  OnInit();
    void  OnThemeChanged();
    void  Serialize(AttributeSerializerWrap* ps);

public:
    void  CreateTheme();
    void  SetNoTheme(bool b) { 
		m_bNoTheme = b; CreateTheme(); 
	}
	bool  GetNoTheme() { 
		return m_bNoTheme;
	}
    HTHEME  GetTHEME() {
		return m_hTheme; 
	}

protected:
    IThemeTextRenderBase*  m_pIThemeTextRenderBase;
	HTHEME  m_hTheme;
    bool    m_bNoTheme;  // 是否强制使用无主题的样式。
};
#endif
} // namespace ui