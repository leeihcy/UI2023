#pragma once
#include "src/graphics/font/font.h"
#include <memory>
#include "include/interface/itextrenderbase.h"
#include "include/macro/uimsg.h"
#include "src/object/message.h"

namespace ui {
class AttributeSerializer;
class Application;
class Object;
struct TEXTRENDERBASE_DRAWSTATE;
struct IFontRes;
class ResourceBundle;

class TextRenderBase : public Message {
public:
  TextRenderBase(ITextRenderBase *p);
  ~TextRenderBase();

  void onRouteMessage(ui::Msg* msg);
  void Serialize(AttributeSerializer *ps);
  Size GetDesiredSize(const char* text, unsigned int limit_width = 0);
  
public:
  void SetTextAlignment(int nDrawFlag);
  int GetTextAlignment() { return m_nDrawTextFlag; }
  // void SetTextEffect(TEXT_EFFECT e) { m_eDrawTextEffect = e; }
  // TEXT_EFFECT GetTextEffect() { return m_eDrawTextEffect; }
  // void SetDrawTextParam(int w, int l) {
  //   m_wparamDrawText = w;
  //   m_lparamDrawText = l;
  // }

  void SetObject(Object *pObject) { m_pObject = pObject; }
  Object *GetObject() { return m_pObject; }

  void SetTextRenderType(const TEXTRENDER_TYPE &nType) {
    m_nTextRenderType = nType;
  }
  TEXTRENDER_TYPE GetTextRenderType() { return m_nTextRenderType; }

  IColorRes *GetSkinColorRes();
  ResourceBundle *GetResource();

  bool IsThemeRender();
  void CheckSkinTextureChanged();

  void _LoadColor(const char *szColorId, Color *&pColor);
  const char *_SaveColor(Color *&pColor);

  // void LoadHaloColor(const char *szColorId);
  // const char *GetHaloColorId();

protected:
  ITextRenderBase *m_pITextRenderBase;
  TEXTRENDER_TYPE m_nTextRenderType; // 自己的类型

  ResourceBundle*  m_resouce = nullptr;

  Object *m_pObject;                 // 绑定的对象，要绘制谁的文字
  int m_nDrawTextFlag;               // DrawText的flag标识

  // TEXT_EFFECT m_eDrawTextEffect;
  // Color *m_pColorTextBkgnd;
  // int m_wparamDrawText;
  // int m_lparamDrawText;

};

class SimpleTextRender : public TextRenderBase {
public:
  SimpleTextRender(ISimpleTextRender *p);
  ~SimpleTextRender();

  void onRouteMessage(ui::Msg *msg);

protected:
  void OnSerialize(SerializeParam *pData);
  void DrawState(TEXTRENDERBASE_DRAWSTATE *pDrawStruct);
  void onGetDesiredSize(GetTextDesiredSizeMessage* msg);

  void LoadColor(const char *szText);
  const char *GetColor();

public:
  void SetColor(Color color);
  void SetColor(Color color, int index);

protected:
  ISimpleTextRender *m_pISimpleTextRender;
  DrawTextParam m_draw_text_param;

  std::vector<Color> m_vTextColor;
};

#if 0
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

  // void LoadFont(const char *szFontId) { m_pRenderFont = _LoadFont(szFontId); }
  // const char *GetFontId() { return _SaveFont(m_pRenderFont.get()); }
  // void LoadColor(const char *szColorId) {
  //   _LoadColor(szColorId, m_pColorText);
  // }
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

  // void LoadFont(const char *szFontId) { m_pRenderFont = _LoadFont(szFontId); }
  // const char *GetFontId() { return _SaveFont(m_pRenderFont.get()); }

protected:
  void Clear();

private:
  IContrastColorListTextRender *m_pIContrastColorListTextRender;

  std::vector<Color *> m_vTextColor;
  std::shared_ptr<IRenderFont> m_pRenderFont;
  int m_nCount;
};

//////////////////////////////////////////////////////////////////////////

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
  // void LoadFont(const char *szText);
  // const char *GetFont();

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