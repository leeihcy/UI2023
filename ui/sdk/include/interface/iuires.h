#ifndef _IUIRES_H_
#define _IUIRES_H_
#include "sdk/include/interface.h"

#include "sdk/include/macro/uidefine.h"
#include "sdk/include/macro/xmldefine.h"

namespace ui {
class ColorManager;
struct Color;
struct IColorRes;
struct IUIDocument;
struct IRenderBitmap;
struct IRenderFont;
struct IUIElement;
struct IMessage;
struct IObject;

struct UIAPI IColorManager : public IRootInterface {
  IColorManager(ColorManager *p);
  ColorManager *GetImpl();

  IColorRes &GetColorRes();

private:
  ColorManager *m_pImpl;
};

class ColorRes;
struct IColorResItem;
struct UIAPI IColorRes : public IRootInterface {
  IColorRes(ColorRes *p);
  ColorRes *GetImpl();

  long GetCount();
  IColorResItem *GetItem(long lIndex);
  Color GetColor(const char *szColorId, bool* exist=nullptr);
  const char *GetColorId(Color *p);

private:
  ColorRes *m_pImpl;
};

class ColorResItem;
struct UIAPI IColorResItem : public IRootInterface {
  IColorResItem(ColorResItem *p);
  ColorResItem *GetImpl();

  const char *GetId();
  const char *GetColorString();

private:
  ColorResItem *m_pImpl;
};

class ImageResItem;
struct UIAPI IImageResItem : public IRootInterface {
  IImageResItem(ImageResItem *p);
  ImageResItem *GetImpl();

  void SetId(const char *);
  void SetPath(const char *);
  const char *GetId();
  const char *GetPath();
  bool GetUseSkinHLS();
  bool GetNeedAntiAliasing();
  void SetUseSkinHLS(bool b);
  void SetNeedAntiAliasing(bool b);
  bool ModifyImage(const char *szPath);
  IMAGE_ITEM_TYPE GetImageItemType();
  //        void  SetImageItemType(IMAGE_ITEM_TYPE e);

private:
  ImageResItem *m_pImpl;
};
class ImageListResItem;
struct UIAPI IImageListResItem : public IImageResItem {
  IImageListResItem(ImageListResItem *p);
  ImageListResItem *GetImpl();

  IMAGELIST_LAYOUT_TYPE GetLayoutType();
  int GetItemCount();
  void SetLayoutType(IMAGELIST_LAYOUT_TYPE);
  void SetItemCount(int);

private:
  ImageListResItem *m_pImpl;
};

class ImageIconResItem;
struct UIAPI IImageIconResItem : public IImageResItem {
  IImageIconResItem(ImageIconResItem *p);
  ImageIconResItem *GetImpl();

private:
  ImageIconResItem *m_pImpl;
};

class ImageRes;
struct UIAPI IImageRes : public IRootInterface {
  IImageRes(ImageRes *p);
  ImageRes *GetImpl();

  long GetImageCount();
  IImageResItem *GetImageResItem(long lIndex);
  IImageResItem *GetImageResItem(const char *szId);
  bool ModifyImage(const char *szId, const char *szPath);
  std::shared_ptr<IRenderBitmap> GetBitmap(const char *szImageId,
                 GRAPHICS_RENDER_LIBRARY_TYPE eRenderType);
#if 0 // defined(OS_WIN)
  HBITMAP LoadBitmap(const char *szId);
#endif
  const char *GetRenderBitmapId(IRenderBitmap *pBitmap);

private:
  ImageRes *m_pImpl;
};

class CursorRes;
struct IUICursor;
struct UIAPI ICursorRes : public IRootInterface {
  ICursorRes(CursorRes *p);
  CursorRes *GetImpl();

  void GetCursor(const char *szCursorID, IUICursor **pp);
  const char *GetCursorId(IUICursor *p);

private:
  CursorRes *m_pImpl;
};

class CursorResItem;
struct UIAPI ICursorResItem : public IRootInterface {
  ICursorResItem(CursorResItem *p);
  CursorResItem *GetImpl();

private:
  CursorResItem *m_pImpl;
};

class FontResItem;
struct UIAPI IFontResItem : public IRootInterface {
  IFontResItem(FontResItem *);
  FontResItem *GetImpl();

  const char *GetId();
#if 0 // defined(OS_WIN)
  LOGFONT *GetLogFont();
#endif
private:
  FontResItem *m_pImpl;
};

class GifRes;
struct IGifImage;
struct UIAPI IGifRes : public IRootInterface {
  IGifRes(GifRes *p);
  GifRes *GetImpl();

  long GetGifImage(const char *szId, IGifImage **ppGifImage);

private:
  GifRes *m_pImpl;
};

class GifResItem;
struct UIAPI IGifResItem : public IRootInterface {
  IGifResItem(GifResItem *p);
  GifResItem *GetImpl();

private:
  GifResItem *m_pImpl;
};

#if 0
class FontRes;
struct UIAPI IFontRes : public IRootInterface {
  IFontRes(FontRes *p);
  FontRes *GetImpl();

  long GetFontCount();
  bool GetFontResItem(long lIndex, IFontResItem **ppResItem);
  bool GetFont(const char *szFontId,
               GRAPHICS_RENDER_LIBRARY_TYPE eRenderType,
               /*__out*/ IRenderFont **ppOut);
  bool GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType,
                      /*__out*/ IRenderFont **ppFont);
  const char *GetDefaultFontId();
  const char *GetRenderFontId(IRenderFont *pFont);

private:
  FontRes *m_pImpl;
};
#endif

class StyleResItem;
struct UIAPI IStyleResItem : public IRootInterface {
  IStyleResItem(StyleResItem *p);
  StyleResItem *GetImpl();

  STYLE_SELECTOR_TYPE GetSelectorType();
  const char *GetId();
  const char *GetInherits();
  const char *GetAttribute(const char *szKey);

  // 不提供。editor需要的话使用styleres::moidfy才会触发on moidify
  // 		void  SetSelectorType(STYLE_SELECTOR_TYPE type);
  // 		void  SetId(const char* szId);
  // 		void  SetInherits(const char*);
  // 		void  SetAttribute(const char* key, const char* value);
  // 		void  RemoveAttribute(const char* key);

  std::shared_ptr<IAttributeMap> GetAttributeMap();

  void SetXmlElement(IUIElement *p);
  IUIElement *GetXmlElement();
  bool GetXmlElement2(IUIElement **pp);

private:
  StyleResItem *m_pImpl;
};

class StyleRes;
struct UIAPI IStyleRes : public IRootInterface {
  IStyleRes(StyleRes *p);
  StyleRes *GetImpl();

  IStyleResItem *Add(STYLE_SELECTOR_TYPE type, const char *szId,
                     const char *szInherit);
  IStyleResItem *Insert(STYLE_SELECTOR_TYPE type, const char *szId,
                        long lPos);
  bool Modify(IStyleResItem *pItem, STYLE_SELECTOR_TYPE type,
              const char *szId, const char *szInherit);
  bool Remove(STYLE_SELECTOR_TYPE type, const char *szId);
  bool Remove(IStyleResItem *pItem);

  bool AddAttribute(IStyleResItem *pItem, const char *szKey,
                    const char *szValue);
  bool ModifyAttribute(IStyleResItem *pItem, const char *szKey,
                       const char *szValue);
  bool RemoveAttribute(IStyleResItem *pItem, const char *szKey);

  long GetStyleCount();
  IStyleResItem *GetItem(long lIndex);
  long GetItemPos(IStyleResItem *p);

  IStyleResItem *FindItem(STYLE_SELECTOR_TYPE type, const char *szId);

  bool LoadStyle(const char *szTagName, const char *szStyleClass,
                 const char *szID, IAttributeMap *pMapStyle);
  bool UnloadStyle(const char *szTagName, const char *szStyleClass,
                   const char *szID, IAttributeList *pListAttribte);

private:
  StyleRes *m_pImpl;
};

class ImageManager;
struct UIAPI IImageManager : public IRootInterface {
  IImageManager(ImageManager *p);
  ImageManager *GetImpl();

  IImageRes &GetImageRes();
#if 0 // defined(OS_WIN)
  ICursorRes *GetCursorRes();
  IGifRes *GetGifRes();
#endif
  IUIElement *GetImageXmlElem(const char *szId);

  IImageResItem *InsertImageItem(IMAGE_ITEM_TYPE eType, const char *szID,
                                 const char *szPath);
  bool ModifyImageItem(const char *szID, const char *szPath);
  bool RemoveImageItem(const char *szID);

  bool ModifyImageItemInRunTime(const char *szID, const char *szPath);
  bool ModifyImageItemAlpha(const char *szID, byte nAlphaPercent);

private:
  ImageManager *m_pImpl;
};

#if 0
class FontManager;
struct UIAPI IFontManager {
  IFontManager(FontManager *p);
  FontManager *GetImpl();

  IFontRes &GetFontRes();

private:
  FontManager *m_pImpl;
};
#endif

class StyleManager;
struct UIAPI IStyleManager {
  IStyleManager(StyleManager *);
  StyleManager *GetImpl();

  IStyleRes &GetStyleRes();
  IUIElement *GetStyleXmlElem(const char *szId);

private:
  StyleManager *m_pImpl;
};

struct ILayoutWindowNodeList {
  virtual void Release() = 0;
  virtual unsigned int GetCount() = 0;
  virtual const char *GetWindowName(unsigned int index) = 0;
  virtual const char *GetWindowId(unsigned int index) = 0;
  virtual const char *GetWindowPath(unsigned int index) = 0;
};

class LayoutManager;
struct UIAPI ILayoutManager {
  ILayoutManager(LayoutManager *);
  LayoutManager *GetImpl();

  IObject *LoadPluginLayout(const char *szWndId, IObject *pParent,
                            IMessage *pNotifyTarget);
  IObject *ParseElement(IUIElement *pUIElement, IObject *pParent);

  bool FindWindowElement(const char *szTagName, const char *szId,
                         IUIElement **ppElem);
  bool LoadWindowNodeList(ILayoutWindowNodeList **);

private:
  LayoutManager *m_pImpl;
};

// 国际化
struct II18nCallback {
  virtual const char *Map(const char *key) = 0;
};

class I18nRes;
struct UIAPI II18nRes {
  II18nRes(I18nRes *);
  const char *MapConfigValue(const char *);

private:
  I18nRes *m_pImpl;
};
} // namespace ui

#endif // _IUIRES_H_