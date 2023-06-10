#ifndef _IUIRES_H_
#define _IUIRES_H_

namespace UI
{
    class ColorManager;
    class Color;
    interface IColorRes;
	interface IUIDocument;
	interface IRenderBitmap;
	interface IRenderFont;

    interface UIAPI IColorManager : public IRootInterface
    {
        IColorManager(ColorManager* p);
        ColorManager*  GetImpl();

        IColorRes&  GetColorRes();

    private:
        ColorManager*  m_pImpl;
    };

	class ColorRes;
	interface IColorResItem;
	interface UIAPI IColorRes : public IRootInterface
	{
		IColorRes(ColorRes* p);
		ColorRes*  GetImpl();

		long  GetColorCount();
		IColorResItem*  GetColorResItem(long lIndex);
		void  GetColor(LPCTSTR szColorId, Color** pp);
		LPCTSTR  GetColorId(Color* p);

	private:
		ColorRes*  m_pImpl;
	};

	class  ColorResItem;
	interface UIAPI IColorResItem : public IRootInterface
	{
		IColorResItem(ColorResItem* p);
		ColorResItem*  GetImpl();

		LPCTSTR GetId();
		LPCTSTR GetColorString();
	private:
		ColorResItem*  m_pImpl;
	};

    class ImageResItem;
    interface UIAPI IImageResItem : public IRootInterface
    {
        IImageResItem(ImageResItem* p);
        ImageResItem*  GetImpl(); 

        void  SetId(const TCHAR*);
        void  SetPath(const TCHAR*);
        LPCTSTR GetId();
        LPCTSTR GetPath();
        bool  GetUseSkinHLS();
        bool  GetNeedAntiAliasing();
        void  SetUseSkinHLS(bool b);
        void  SetNeedAntiAliasing(bool b);
        bool  ModifyImage(LPCTSTR szPath);
        IMAGE_ITEM_TYPE  GetImageItemType();
        //        void  SetImageItemType(IMAGE_ITEM_TYPE e);

    private:
        ImageResItem*  m_pImpl;
    };
    class ImageListResItem;
    interface UIAPI IImageListResItem : public IImageResItem
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
    interface UIAPI IImageIconResItem : public IImageResItem
    {
        IImageIconResItem(ImageIconResItem* p);
        ImageIconResItem*  GetImpl(); 

    private:
        ImageIconResItem*  m_pImpl;
    };

    class ImageRes;
    interface UIAPI IImageRes : public IRootInterface
    {
        IImageRes(ImageRes* p);
        ImageRes*  GetImpl();

        long  GetImageCount();
        IImageResItem*  GetImageResItem(long lIndex);
        IImageResItem*  GetImageResItem(LPCTSTR szId);
        bool  ModifyImage(LPCTSTR szId, LPCTSTR szPath);
        bool  GetBitmap(LPCTSTR szImageId, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderBitmap** ppRenderBitmap);
        HBITMAP  LoadBitmap(LPCTSTR szId);

        LPCTSTR  GetRenderBitmapId(IRenderBitmap* pBitmap);

    private:
        ImageRes*  m_pImpl;
    };


    class CursorRes;
    interface IUICursor;
    interface UIAPI ICursorRes : public IRootInterface
    {
        ICursorRes(CursorRes* p);
        CursorRes*  GetImpl(); 

        void  GetCursor(LPCTSTR szCursorID, IUICursor** pp);
        LPCTSTR  GetCursorId(IUICursor* p);

    private:
        CursorRes*  m_pImpl;
    };

    class CursorResItem;
    interface UIAPI ICursorResItem : public IRootInterface
    {
        ICursorResItem(CursorResItem* p);
        CursorResItem*  GetImpl(); 

    private:
        CursorResItem*  m_pImpl;
    };

   
    class FontResItem;
    interface UIAPI IFontResItem : public IRootInterface
    {
        IFontResItem(FontResItem*);
        FontResItem*  GetImpl();

        LPCTSTR  GetId();
        LOGFONT*  GetLogFont();
    private:
        FontResItem*  m_pImpl;
    };

    class GifRes;
    interface IGifImage;
    interface UIAPI IGifRes : public IRootInterface
    {
        IGifRes(GifRes* p);
        GifRes*  GetImpl();

        HRESULT  GetGifImage(LPCTSTR szId, IGifImage** ppGifImage);
    private:
        GifRes*  m_pImpl;
    };

    class GifResItem;
    interface UIAPI IGifResItem : public IRootInterface
    {
        IGifResItem(GifResItem* p);
        GifResItem*  GetImpl();

    private:
        GifResItem*  m_pImpl;
    };

    class FontRes;
    interface UIAPI IFontRes : public IRootInterface
    {
        IFontRes(FontRes* p);
        FontRes*  GetImpl();

        long  GetFontCount();
        bool  GetFontResItem(long lIndex, IFontResItem** ppResItem);
        bool  GetFont(LPCTSTR szFontId, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppOut);
        bool  GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppFont);
        LPCTSTR  GetDefaultFontId();
        LPCTSTR  GetRenderFontId(IRenderFont* pFont);
    private:
        FontRes*  m_pImpl;
    };

    class StyleResItem;
    interface UIAPI IStyleResItem : public IRootInterface
    {
        IStyleResItem(StyleResItem* p);
        StyleResItem*  GetImpl();

        STYLE_SELECTOR_TYPE  GetSelectorType();
        LPCTSTR  GetId();
        LPCTSTR  GetInherits();
		LPCTSTR  GetAttribute(LPCTSTR szKey);

	
		// 不提供。editor需要的话使用styleres::moidfy才会触发on moidify
// 		void  SetSelectorType(STYLE_SELECTOR_TYPE type);
// 		void  SetId(LPCTSTR szId);
// 		void  SetInherits(LPCTSTR);
// 		void  SetAttribute(LPCTSTR key, LPCTSTR value);
// 		void  RemoveAttribute(LPCTSTR key);

        void  GetAttributeMap(IMapAttribute** ppMapAttrib);

        void  SetXmlElement(IUIElement* p);
        IUIElement*  GetXmlElement();
        bool  GetXmlElement2(IUIElement** pp);

    private:
        StyleResItem*  m_pImpl;
    };

    class StyleRes;
    interface UIAPI IStyleRes : public IRootInterface
    {
        IStyleRes(StyleRes* p);
        StyleRes*  GetImpl();

        IStyleResItem*  Add(
                STYLE_SELECTOR_TYPE type, LPCTSTR szId, LPCTSTR szInherit);
		IStyleResItem*  Insert(
                STYLE_SELECTOR_TYPE type, LPCTSTR szId, long lPos);
		bool  Modify(
                IStyleResItem* pItem, 
                STYLE_SELECTOR_TYPE type,
                LPCTSTR szId, 
                LPCTSTR szInherit);
        bool  Remove(STYLE_SELECTOR_TYPE type, LPCTSTR szId);
		bool  Remove(IStyleResItem* pItem);

		bool  AddAttribute(
                IStyleResItem* pItem, LPCTSTR szKey, LPCTSTR szValue);
		bool  ModifyAttribute(
                IStyleResItem* pItem, LPCTSTR szKey, LPCTSTR szValue);
		bool  RemoveAttribute(IStyleResItem* pItem, LPCTSTR szKey);

        long  GetStyleCount();
        IStyleResItem*  GetItem(long lIndex);
		long  GetItemPos(IStyleResItem* p);

        IStyleResItem*  FindItem(STYLE_SELECTOR_TYPE type, LPCTSTR szId);

        bool  LoadStyle(
                LPCTSTR szTagName, 
                LPCTSTR szStyleClass, 
                LPCTSTR szID, 
                IMapAttribute* pMapStyle);
        bool  UnloadStyle(
                LPCTSTR szTagName,
                LPCTSTR szStyleClass,
                LPCTSTR szID, 
                IListAttribute* pListAttribte);

    private:
        StyleRes*  m_pImpl;
    };

    class ImageManager;
    interface UIAPI IImageManager : public IRootInterface	
    {
        IImageManager(ImageManager* p);
        ImageManager*  GetImpl();

        IImageRes&   GetImageRes();
        ICursorRes*  GetCursorRes();
        IGifRes*     GetGifRes();
        IUIElement*  GetImageXmlElem(LPCTSTR szId);

        IImageResItem*  InsertImageItem(
                IMAGE_ITEM_TYPE eType, LPCTSTR szID, LPCTSTR szPath);
        bool  ModifyImageItem(LPCTSTR szID, LPCTSTR szPath);
        bool  RemoveImageItem(LPCTSTR szID);

        bool  ModifyImageItemInRunTime(LPCTSTR szID, LPCTSTR szPath);
        bool  ModifyImageItemAlpha(LPCTSTR szID, byte nAlphaPercent);
    private:
        ImageManager*  m_pImpl;
    };

    class FontManager;
    interface UIAPI IFontManager
    {
        IFontManager(FontManager* p);
        FontManager*  GetImpl();

        IFontRes&  GetFontRes();
    private:
        FontManager*  m_pImpl;
    };

    class StyleManager;
    interface UIAPI IStyleManager
    {
        IStyleManager(StyleManager*);
        StyleManager*  GetImpl();

        IStyleRes&  GetStyleRes();
        IUIElement*  GetStyleXmlElem(LPCTSTR szId);

    private:
        StyleManager*  m_pImpl;
    };

    interface ILayoutWindowNodeList
    {
        virtual void  Release() PURE;
        virtual UINT  GetCount() PURE;
        virtual LPCTSTR  GetWindowName(UINT index) PURE;
        virtual LPCTSTR  GetWindowId(UINT index) PURE;
        virtual LPCTSTR  GetWindowPath(UINT index) PURE;
    };

    class LayoutManager;
    interface UIAPI ILayoutManager
    {
        ILayoutManager(LayoutManager*);
        LayoutManager*  GetImpl();

        IObject*  LoadPluginLayout(
                    LPCTSTR szWndId, 
                    IObject* pParent, 
                    IMessage* pNotifyTarget);
        IObject*  ParseElement(IUIElement* pUIElement, IObject* pParent);

        bool  FindWindowElement(
                   LPCTSTR szTagName, LPCTSTR szId, IUIElement** ppElem);
        bool  LoadWindowNodeList(ILayoutWindowNodeList**);

    private:
        LayoutManager*  m_pImpl;
    };
 
	// 国际化 
	interface II18nCallback
	{
		virtual LPCTSTR Map(LPCTSTR key) = 0;
	};

	class I18nRes;
	interface UIAPI II18nRes
	{
		II18nRes(I18nRes*);
        LPCTSTR  MapConfigValue(LPCTSTR);

	private:
		I18nRes*  m_pImpl;
	};
}

#endif // _IUIRES_H_