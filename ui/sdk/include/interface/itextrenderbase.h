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

  void SetObject(IObject *pObject);
  IObject *GetObject();

  void SetType(const TEXTRENDER_TYPE &nType);
  TEXTRENDER_TYPE GetType();
  void SetTextAlignment(int nDrawFlag);
  int GetTextAlignment();
  // void SetTextEffect(TEXT_EFFECT);
  // TEXT_EFFECT GetTextEffect();
  // void SetDrawTextParam(long w, long l);

  Size GetDesiredSize(const char *szText, unsigned int nLimitWidth = 0);
  void DrawState(IRenderTarget *pRenderTarget, const Rect *prc, int nState,
                 const char *szText, int nDrawTextFlag = -1);
  void Init();
  void CheckSkinTextureChanged();

  void Serialize(SerializeParam *pData);

protected:
  ITextRenderBase();
  UI_DECLARE_INTERFACE(TextRenderBase);
};

struct AttributeSerializerWrap;
class ThemeTextRenderBase;

// struct UIAPI IThemeTextRenderBase : public ITextRenderBase
// {
// 	IThemeTextRenderBase() {};
//
//     const char*  GetThemeName();
//     HTHEME  GetTHEME();
// 	void  Serialize(AttributeSerializerWrap* ps);
//
// 	UI_DECLARE_INTERFACE(ThemeTextRenderBase);
// };

class SimpleTextRender;
struct UIAPI ISimpleTextRender : public ITextRenderBase {
  // void SetColor(Color color);
  // Color GetColor();

  UI_DECLARE_INTERFACE(SimpleTextRender);
};

#if 0
class ContrastColorTextRender;
struct UIAPI IContrastColorTextRender : public ITextRenderBase {
  UI_DECLARE_INTERFACE(ContrastColorTextRender)
};

class ContrastColorListTextRender;
struct UIAPI IContrastColorListTextRender : public ITextRenderBase {
  UI_DECLARE_INTERFACE(ContrastColorListTextRender);
};

class FontColorListTextRender;
struct UIAPI IFontColorListTextRender : public ITextRenderBase {
  void SetCount(int nCount);
  void SetColor(int nIndex, unsigned int color);
  void SetFont(int nIndex, std::shared_ptr<IRenderFont> );


  UI_DECLARE_INTERFACE(FontColorListTextRender);
};
#endif

struct ITextRenderBaseMeta : public IMeta {
  virtual std::shared_ptr<ITextRenderBase> CreateShared(ui::IResource *p) = 0;
};

template <class Ixx>
struct TextRenderBaseMetaImpl : public MetaImpl<Ixx, ITextRenderBaseMeta> {
  using This = MetaImpl<Ixx, ITextRenderBaseMeta>;

  std::shared_ptr<ITextRenderBase> CreateShared(IResource *resource) override {
    Ixx *p = This::create(resource);
    return std::shared_ptr<Ixx>(p, This::destroy);
  }
};

} // namespace ui

#endif // _UI_ITEXTRENDERBASE_H_