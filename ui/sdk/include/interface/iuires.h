#ifndef _IUIRES_H_
#define _IUIRES_H_

#include "sdk/include/macro/uidefine.h"
#include "sdk/include/macro/xmldefine.h"

namespace ui
{
    class ColorManager;
    class Color;
    struct IColorRes;
	struct IUIDocument;
	struct IRenderBitmap;
	struct IRenderFont;
    struct IUIElement;
    struct IMessage;
    struct IObject;

    struct UIAPI IColorManager : public IRootInterface
    {
        IColorManager(ColorManager* p);
        ColorManager*  GetImpl();

        IColorRes&  GetColorRes();

    private:
        ColorManager*  m_pImpl;
    };

	class ColorRes;
	struct IColorResItem;
	struct UIAPI IColorRes : public IRootInterface
	{
		IColorRes(ColorRes* p);
		ColorRes*  GetImpl();

		long  GetColorCount();
		IColorResItem*  GetColorResItem(long lIndex);
		void  GetColor(const wchar_t* szColorId, Color** pp);
		const wchar_t*  GetColorId(Color* p);

	private:
		ColorRes*  m_pImpl;
	};

	class  ColorResItem;
	struct UIAPI IColorResItem : public IRootInterface
	{
		IColorResItem(ColorResItem* p);
		ColorResItem*  GetImpl();

		const wchar_t* GetId();
		const wchar_t* GetColorString();
	private:
		ColorResItem*  m_pImpl;
	};

    class ImageResItem;
    struct UIAPI IImageResItem : public IRootInterface
    {
        IImageResItem(ImageResItem* p);
        ImageResItem*  GetImpl(); 

        void  SetId(const wchar_t*);
        void  SetPath(const wchar_t*);
        const wchar_t* GetId();
        const wchar_t* GetPath();
        bool  GetUseSkinHLS();
        bool  GetNeedAntiAliasing();
        void  SetUseSkinHLS(bool b);
        void  SetNeedAntiAliasing(bool b);
        bool  ModifyImage(const wchar_t* szPath);
        IMAGE_ITEM_TYPE  GetImageItemType();
        //        void  SetImageItemType(IMAGE_ITEM_TYPE e);

    private:
        ImageResItem*  m_pImpl;
    };
    class ImageListResItem;
    struct UIAPI IImageListResItem : public IImageResItem
    {
        IImageListResItem(ImageListResItem* p);
        ImageListResItem*  GetImpl(); 

        IMAGELIST_LAYOUT_TYPE  GetLayoutType();
        int  GetItemCount();
        void  SetLayoutType(IMAGELIST_LAYOUT_TYPE);
        void  SetItemCount(int);

    private:
        ImageListResItem*  m_pImpl;
    };

    class ImageIconResItem;
    struct UIAPI IImageIconResItem : public IImageResItem
    {
        IImageIconResItem(ImageIconResItem* p);
        ImageIconResItem*  GetImpl(); 

    private:
        ImageIconResItem*  m_pImpl;
    };

    class ImageRes;
    struct UIAPI IImageRes : public IRootInterface
    {
        IImageRes(ImageRes* p);
        ImageRes*  GetImpl();

        long  GetImageCount();
        IImageResItem*  GetImageResItem(long lIndex);
        IImageResItem*  GetImageResItem(const wchar_t* szId);
        bool  ModifyImage(const wchar_t* szId, const wchar_t* szPath);
        bool  GetBitmap(const wchar_t* szImageId, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, /*out*/ IRenderBitmap** ppRenderBitmap);
    #if defined(OS_WIN)
        HBITMAP  LoadBitmap(const wchar_t* szId);
    #endif
        const wchar_t*  GetRenderBitmapId(IRenderBitmap* pBitmap);

    private:
        ImageRes*  m_pImpl;
    };


    class CursorRes;
    struct IUICursor;
    struct UIAPI ICursorRes : public IRootInterface
    {
        ICursorRes(CursorRes* p);
        CursorRes*  GetImpl(); 

        void  GetCursor(const wchar_t* szCursorID, IUICursor** pp);
        const wchar_t*  GetCursorId(IUICursor* p);

    private:
        CursorRes*  m_pImpl;
    };

    class CursorResItem;
    struct UIAPI ICursorResItem : public IRootInterface
    {
        ICursorResItem(CursorResItem* p);
        CursorResItem*  GetImpl(); 

    private:
        CursorResItem*  m_pImpl;
    };

   
    class FontResItem;
    struct UIAPI IFontResItem : public IRootInterface
    {
        IFontResItem(FontResItem*);
        FontResItem*  GetImpl();

        const wchar_t*  GetId();
    #if defined(OS_WIN)
        LOGFONT*  GetLogFont();
    #endif
    private:
        FontResItem*  m_pImpl;
    };

    class GifRes;
    struct IGifImage;
    struct UIAPI IGifRes : public IRootInterface
    {
        IGifRes(GifRes* p);
        GifRes*  GetImpl();

        long  GetGifImage(const wchar_t* szId, IGifImage** ppGifImage);
    private:
        GifRes*  m_pImpl;
    };

    class GifResItem;
    struct UIAPI IGifResItem : public IRootInterface
    {
        IGifResItem(GifResItem* p);
        GifResItem*  GetImpl();

    private:
        GifResItem*  m_pImpl;
    };

    class FontRes;
    struct UIAPI IFontRes : public IRootInterface
    {
        IFontRes(FontRes* p);
        FontRes*  GetImpl();

        long  GetFontCount();
        bool  GetFontResItem(long lIndex, IFontResItem** ppResItem);
        bool  GetFont(const wchar_t* szFontId, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, /*__out*/ IRenderFont** ppOut);
        bool  GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, /*__out*/ IRenderFont** ppFont);
        const wchar_t*  GetDefaultFontId();
        const wchar_t*  GetRenderFontId(IRenderFont* pFont);
    private:
        FontRes*  m_pImpl;
    };

    class StyleResItem;
    struct UIAPI IStyleResItem : public IRootInterface
    {
        IStyleResItem(StyleResItem* p);
        StyleResItem*  GetImpl();

        STYLE_SELECTOR_TYPE  GetSelectorType();
        const wchar_t*  GetId();
        const wchar_t*  GetInherits();
		const wchar_t*  GetAttribute(const wchar_t* szKey);

	
		// 不提供。editor需要的话使用styleres::moidfy才会触发on moidify
// 		void  SetSelectorType(STYLE_SELECTOR_TYPE type);
// 		void  SetId(const wchar_t* szId);
// 		void  SetInherits(const wchar_t*);
// 		void  SetAttribute(const wchar_t* key, const wchar_t* value);
// 		void  RemoveAttribute(const wchar_t* key);

        void  GetAttributeMap(IMapAttribute** ppMapAttrib);

        void  SetXmlElement(IUIElement* p);
        IUIElement*  GetXmlElement();
        bool  GetXmlElement2(IUIElement** pp);

    private:
        StyleResItem*  m_pImpl;
    };

    class StyleRes;
    struct UIAPI IStyleRes : public IRootInterface
    {
        IStyleRes(StyleRes* p);
        StyleRes*  GetImpl();

        IStyleResItem*  Add(
                STYLE_SELECTOR_TYPE type, const wchar_t* szId, const wchar_t* szInherit);
		IStyleResItem*  Insert(
                STYLE_SELECTOR_TYPE type, const wchar_t* szId, long lPos);
		bool  Modify(
                IStyleResItem* pItem, 
                STYLE_SELECTOR_TYPE type,
                const wchar_t* szId, 
                const wchar_t* szInherit);
        bool  Remove(STYLE_SELECTOR_TYPE type, const wchar_t* szId);
		bool  Remove(IStyleResItem* pItem);

		bool  AddAttribute(
                IStyleResItem* pItem, const wchar_t* szKey, const wchar_t* szValue);
		bool  ModifyAttribute(
                IStyleResItem* pItem, const wchar_t* szKey, const wchar_t* szValue);
		bool  RemoveAttribute(IStyleResItem* pItem, const wchar_t* szKey);

        long  GetStyleCount();
        IStyleResItem*  GetItem(long lIndex);
		long  GetItemPos(IStyleResItem* p);

        IStyleResItem*  FindItem(STYLE_SELECTOR_TYPE type, const wchar_t* szId);

        bool  LoadStyle(
                const wchar_t* szTagName, 
                const wchar_t* szStyleClass, 
                const wchar_t* szID, 
                IMapAttribute* pMapStyle);
        bool  UnloadStyle(
                const wchar_t* szTagName,
                const wchar_t* szStyleClass,
                const wchar_t* szID, 
                IListAttribute* pListAttribte);

    private:
        StyleRes*  m_pImpl;
    };

    class ImageManager;
    struct UIAPI IImageManager : public IRootInterface	
    {
        IImageManager(ImageManager* p);
        ImageManager*  GetImpl();

        IImageRes&   GetImageRes();
    #if defined(OS_WIN)
        ICursorRes*  GetCursorRes();
        IGifRes*     GetGifRes();
    #endif
        IUIElement*  GetImageXmlElem(const wchar_t* szId);

        IImageResItem*  InsertImageItem(
                IMAGE_ITEM_TYPE eType, const wchar_t* szID, const wchar_t* szPath);
        bool  ModifyImageItem(const wchar_t* szID, const wchar_t* szPath);
        bool  RemoveImageItem(const wchar_t* szID);

        bool  ModifyImageItemInRunTime(const wchar_t* szID, const wchar_t* szPath);
        bool  ModifyImageItemAlpha(const wchar_t* szID, byte nAlphaPercent);
    private:
        ImageManager*  m_pImpl;
    };

    class FontManager;
    struct UIAPI IFontManager
    {
        IFontManager(FontManager* p);
        FontManager*  GetImpl();

        IFontRes&  GetFontRes();
    private:
        FontManager*  m_pImpl;
    };

    class StyleManager;
    struct UIAPI IStyleManager
    {
        IStyleManager(StyleManager*);
        StyleManager*  GetImpl();

        IStyleRes&  GetStyleRes();
        IUIElement*  GetStyleXmlElem(const wchar_t* szId);

    private:
        StyleManager*  m_pImpl;
    };

    struct ILayoutWindowNodeList
    {
        virtual void  Release() = 0;
        virtual unsigned int  GetCount() = 0;
        virtual const wchar_t*  GetWindowName(unsigned int index) = 0;
        virtual const wchar_t*  GetWindowId(unsigned int index) = 0;
        virtual const wchar_t*  GetWindowPath(unsigned int index) = 0;
    };

    class LayoutManager;
    struct UIAPI ILayoutManager
    {
        ILayoutManager(LayoutManager*);
        LayoutManager*  GetImpl();

        IObject*  LoadPluginLayout(
                    const wchar_t* szWndId, 
                    IObject* pParent, 
                    IMessage* pNotifyTarget);
        IObject*  ParseElement(IUIElement* pUIElement, IObject* pParent);

        bool  FindWindowElement(
                   const wchar_t* szTagName, const wchar_t* szId, IUIElement** ppElem);
        bool  LoadWindowNodeList(ILayoutWindowNodeList**);

    private:
        LayoutManager*  m_pImpl;
    };
 
	// 国际化 
	struct II18nCallback
	{
		virtual const wchar_t* Map(const wchar_t* key) = 0;
	};

	class I18nRes;
	struct UIAPI II18nRes
	{
		II18nRes(I18nRes*);
        const wchar_t*  MapConfigValue(const wchar_t*);

	private:
		I18nRes*  m_pImpl;
	};
}

#endif // _IUIRES_H_