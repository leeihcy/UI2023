// #include "colormanager.h"
#include "interface/iuires.h"
#include "interface/iresbundle.h"
// #include "colorres.h"
// #include "imagemanager.h"
// #include "../SkinParse/xml/xmlwrap.h"
// #include "fontmanager.h"
// #include "stylemanager.h"
// #include "layoutmanager.h"
#include "res_bundle.h"
#include "application/uiapplication.h"
#include "src/skin_parse/datasource/skindatasource.h"
#include "resource_manager.h"
// #include "i18nres.h"
// #include "../Base/Object/object.h"
#include "include/interface/ixmlwrap.h"

using namespace ui;

IColorManager::IColorManager(ColorManager* p) 
{
    m_pImpl = p;
}
ColorManager*  IColorManager::GetImpl()
{
    return m_pImpl;
}

IColorRes&  IColorManager::GetColorRes()
{
	return m_pImpl->GetColorRes().GetIColorRes(); 
}


IColorRes::IColorRes(ColorRes* p)
{ 
	m_pImpl = p; 
}
ColorRes*  IColorRes::GetImpl() 
{ 
	return m_pImpl;
}

long  IColorRes::GetColorCount() 
{
	return m_pImpl->GetColorCount();
}
IColorResItem*  IColorRes::GetColorResItem(long lIndex)
{ 
	return m_pImpl->GetColorResItem(lIndex); 
}
void  IColorRes::GetColor(const wchar_t* szColorId, Color** pp) 
{ 
	return m_pImpl->GetColor(szColorId, pp); 
}
const wchar_t*  IColorRes::GetColorId(Color* p)
{
	return m_pImpl->GetColorId(p);
}

//////////////////////////////////////////////////////////////////////////

IColorResItem::IColorResItem(ColorResItem* p) 
{
	m_pImpl = p; 
}
ColorResItem*  IColorResItem::GetImpl() 
{
	return m_pImpl;
}

const wchar_t* IColorResItem::GetId() 
{ 
	return m_pImpl->GetId();
}
const wchar_t* IColorResItem::GetColorString()
{ 
	return m_pImpl->GetColorString(); 
}


//////////////////////////////////////////////////////////////////////////


IImageManager::IImageManager(ImageManager* p) 
{ 
    m_pImpl = p; 
}
ImageManager*  IImageManager::GetImpl() 
{
    return m_pImpl; 
}

IImageRes&   IImageManager::GetImageRes() 
{
    return m_pImpl->GetImageRes().GetIImageRes();
}
#if defined(OS_WIN)
ICursorRes*  IImageManager::GetCursorRes()
{
    return m_pImpl->GetCursorRes().GetICursorRes();
}
IGifRes*     IImageManager::GetGifRes() 
{ 
    return m_pImpl->GetGifRes().GetIGifRes(); 
}
#endif
IUIElement*  IImageManager::GetImageXmlElem(const wchar_t* szId)  
{ 
    UIElement* p = m_pImpl->GetImageXmlElem(szId); 
    if (p)
        return p->GetIUIElement();
    return nullptr;
}

IImageResItem*  IImageManager::InsertImageItem(IMAGE_ITEM_TYPE eType, const wchar_t* szID, const wchar_t* szPath) 
{ 
    return m_pImpl->InsertImageItem(eType, szID, szPath);
}
bool  IImageManager::ModifyImageItem(const wchar_t* szID, const wchar_t* szPath)
{ 
    return m_pImpl->ModifyImageItem(szID, szPath);
}
bool  IImageManager::RemoveImageItem(const wchar_t* szID)
{ 
    return m_pImpl->RemoveImageItem(szID);
}

bool  IImageManager::ModifyImageItemInRunTime(const wchar_t* szID, const wchar_t* szPath) 
{ 
    return m_pImpl->ModifyImageItemInRunTime(szID, szPath); 
}
bool  IImageManager::ModifyImageItemAlpha(const wchar_t* szID, byte nAlphaPercent) 
{ 
    return m_pImpl->ModifyImageItemAlpha(szID, nAlphaPercent); 
}

//////////////////////////////////////////////////////////////////////////

IFontManager::IFontManager(FontManager* p)
{ 
    m_pImpl = p; 
} 
FontManager*  IFontManager::GetImpl()
{
    return m_pImpl;
}

IFontRes&  IFontManager::GetFontRes() 
{ 
    return m_pImpl->GetFontRes().GetIFontRes(); 
}


//////////////////////////////////////////////////////////////////////////

IStyleManager::IStyleManager(StyleManager* p)
{ 
    m_pImpl = p;
}
StyleManager*  IStyleManager::GetImpl()
{ 
    return m_pImpl;
}

IStyleRes&  IStyleManager::GetStyleRes() 
{
    return m_pImpl->GetStyleRes().GetIStyleRes(); 
}

IUIElement*  IStyleManager::GetStyleXmlElem(const wchar_t* szId)
{
    UIElement* p = m_pImpl->GetStyleXmlElem(szId);
    if (p)
        return p->GetIUIElement();

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

ILayoutManager::ILayoutManager(LayoutManager* p) 
{ 
    m_pImpl = p; 
}
LayoutManager*  ILayoutManager::GetImpl() 
{
    return m_pImpl; 
}

bool  ILayoutManager::LoadWindowNodeList(ILayoutWindowNodeList** pp) 
{ 
    return m_pImpl->LoadWindowNodeList(pp); 
}

IObject*  ILayoutManager::LoadPluginLayout(
            const wchar_t* szWndId, IObject* pParent, IMessage* pNotifyTarget)
{
#if 0 // TODO:
    Object* pObj = m_pImpl->LoadPluginLayout(
            szWndId, 
            pParent?pParent->GetImpl():nullptr,
            pNotifyTarget);

    if (!pObj)
        return nullptr;

    return pObj->GetIObject();
#else
    UIASSERT(false);
    return nullptr;
#endif
}
IObject*  ILayoutManager::ParseElement(IUIElement* pUIElement, IObject* pParent)
{
#if 0
	if (!pUIElement)
		return nullptr;

    Object* pObj = m_pImpl->ParseElement(
            pUIElement->GetImpl(),
            pParent ? pParent->GetImpl():nullptr);

    if (!pObj)
        return nullptr;

    return pObj->GetIObject();
#else
    UIASSERT(0);
    return nullptr;
#endif
}

bool  ILayoutManager::FindWindowElement(const wchar_t* szTagName, const wchar_t* szId, IUIElement** ppElem)
{
    UIElementProxy p = m_pImpl->FindWindowElement(szTagName, szId);
    if (!p)
        return false;

    if (ppElem)
    {
        *ppElem = p->GetIUIElement();
        p->AddRef();
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////

IImageRes::IImageRes(ImageRes* p) 
{ 
    m_pImpl = p; 
}
ImageRes*  IImageRes::GetImpl()   
{ 
    return m_pImpl; 
}

long  IImageRes::GetImageCount() 
{ 
    return m_pImpl->GetImageCount(); 
}
IImageResItem*  IImageRes::GetImageResItem(long lIndex) 
{ 
    return m_pImpl->GetImageResItem(lIndex);
}
IImageResItem*  IImageRes::GetImageResItem(const wchar_t* szID) 
{ 
    return m_pImpl->GetImageResItem(szID); 
}
bool  IImageRes::ModifyImage(const wchar_t* szId, const wchar_t* szPath)
{ 
    return m_pImpl->ModifyImage(szId, szPath);
}
bool  IImageRes::GetBitmap(const wchar_t* szImageID, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, IRenderBitmap** ppRenderBitmap)
{ 
    return m_pImpl->GetBitmap(szImageID, eRenderType, ppRenderBitmap);
}
#if defined(OS_WIN)
HBITMAP  IImageRes::LoadBitmap(const wchar_t* szId)  
{ 
    return m_pImpl->LoadBitmap(szId);
}
#endif
const wchar_t*  IImageRes::GetRenderBitmapId(IRenderBitmap* pBitmap)
{
    return m_pImpl->GetRenderBitmapId(pBitmap);
}
//////////////////////////////////////////////////////////////////////////

IImageResItem::IImageResItem(ImageResItem* p) 
{ 
    m_pImpl = p;
}
ImageResItem*  IImageResItem::GetImpl() 
{ 
    return m_pImpl; 
}

void  IImageResItem::SetId(const wchar_t* szText)
{ 
    m_pImpl->SetId(szText); 
}
void  IImageResItem::SetPath(const wchar_t* szText) 
{
    m_pImpl->SetPath(szText); 
}
const wchar_t*  IImageResItem::GetId() 
{ 
    return m_pImpl->GetId(); 
}
const wchar_t*  IImageResItem::GetPath() 
{ 
    return m_pImpl->GetPath();
}
bool  IImageResItem::ModifyImage(const wchar_t*  szPath)
{ 
    return m_pImpl->ModifyImage(szPath); }
bool  IImageResItem::GetUseSkinHLS() 
{
    return m_pImpl->GetUseSkinHLS(); 
}
bool  IImageResItem::GetNeedAntiAliasing()
{
    return m_pImpl->GetNeedAntiAliasing();
}
void  IImageResItem::SetUseSkinHLS(bool b) 
{
    return m_pImpl->SetUseSkinHLS(b);
}
void  IImageResItem::SetNeedAntiAliasing(bool b)
{ 
    return m_pImpl->SetNeedAntiAliasing(b);
}
IMAGE_ITEM_TYPE  IImageResItem::GetImageItemType()
{
    return m_pImpl->GetImageItemType();
}
// void  IImageResItem::SetImageItemType(IMAGE_ITEM_TYPE e)
// {
//     m_pImpl->SetImageItemType(e);
// }

//////////////////////////////////////////////////////////////////////////

IImageListResItem::IImageListResItem(ImageListResItem* p):IImageResItem(static_cast<ImageResItem*>(p))
{
    m_pImpl = p; 
}
ImageListResItem*  IImageListResItem::GetImpl() { return m_pImpl; }

IMAGELIST_LAYOUT_TYPE  IImageListResItem::GetLayoutType()
{
    return m_pImpl->GetLayoutType();
}
int  IImageListResItem::GetItemCount()
{
    return m_pImpl->GetItemCount();
}
void  IImageListResItem::SetLayoutType(IMAGELIST_LAYOUT_TYPE e)
{
    m_pImpl->SetLayoutType(e);
}
void  IImageListResItem::SetItemCount(int n)
{
    m_pImpl->SetItemCount(n);
}

//////////////////////////////////////////////////////////////////////////

IImageIconResItem::IImageIconResItem(ImageIconResItem* p):IImageResItem(static_cast<ImageResItem*>(p))
{
    m_pImpl = p; 
}
ImageIconResItem*  IImageIconResItem::GetImpl() { return m_pImpl; }


//////////////////////////////////////////////////////////////////////////

#if defined(OS_WIN)
ICursorRes::ICursorRes(CursorRes* p) { m_pImpl = p; }
CursorRes*  ICursorRes::GetImpl() { return m_pImpl; }

void ICursorRes::GetCursor(const wchar_t*  szCursorID, IUICursor** pp) 
{ 
    m_pImpl->GetCursor(szCursorID, pp); 
}
const wchar_t*  ICursorRes::GetCursorId(IUICursor* p)
{
    return m_pImpl->GetCursorId(p); 
}

ICursorResItem::ICursorResItem(CursorResItem* p) { m_pImpl = p; }
CursorResItem*  ICursorResItem::GetImpl()  { return m_pImpl; }
#endif
//////////////////////////////////////////////////////////////////////////

IFontResItem::IFontResItem(FontResItem* p) { m_pImpl = p; }
FontResItem*  IFontResItem::GetImpl(){ return m_pImpl; }

const wchar_t*  IFontResItem::GetId()  { return m_pImpl->GetId(); }
#if defined(OS_WIN)
LOGFONT*  IFontResItem::GetLogFont() { return m_pImpl->GetLogFont(); }
#endif
//////////////////////////////////////////////////////////////////////////

#if defined(OS_WIN)
IGifRes::IGifRes(GifRes* p) { m_pImpl = p; }
GifRes*  IGifRes::GetImpl()  { return m_pImpl; }

long  IGifRes::GetGifImage(const wchar_t*  szId, IGifImage** ppGifImage) 
{
	IGifImage* p = m_pImpl->GetGifImage(szId); 
	if (ppGifImage)
		*ppGifImage = p;

	return 0;
}

IGifResItem::IGifResItem(GifResItem* p) { m_pImpl = p; }
GifResItem*  IGifResItem::GetImpl()  { return m_pImpl; }
#endif
//////////////////////////////////////////////////////////////////////////


IFontRes::IFontRes(FontRes* p) 
{
    m_pImpl = p;
}
FontRes*  IFontRes::GetImpl() 
{
    return m_pImpl; 
}

long  IFontRes::GetFontCount()
{ 
    return m_pImpl->GetFontCount();
}
bool  IFontRes::GetFontResItem(long lIndex, IFontResItem** ppResItem) 
{ 
    return m_pImpl->GetFontResItem(lIndex, ppResItem); 
}
bool  IFontRes::GetFont(const wchar_t* szFontId, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, IRenderFont** ppOut) 
{
    return m_pImpl->GetFont(szFontId, eRenderType, ppOut); 
}
bool  IFontRes::GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, IRenderFont** ppFont)
{
    return m_pImpl->GetDefaultFont(eRenderType, ppFont); 
}
const wchar_t*  IFontRes::GetDefaultFontId()
{
    return m_pImpl->GetDefaultFontId();
}
const wchar_t*  IFontRes::GetRenderFontId(IRenderFont* pFont)
{
    return m_pImpl->GetRenderFontId(pFont);
}


//////////////////////////////////////////////////////////////////////////

IStyleResItem::IStyleResItem(StyleResItem* p) 
{ 
    m_pImpl = p;
}
StyleResItem*  IStyleResItem::GetImpl()   
{
    return m_pImpl;
}

STYLE_SELECTOR_TYPE  IStyleResItem::GetSelectorType() 
{ 
    return m_pImpl->GetSelectorType(); 
}
const wchar_t*  IStyleResItem::GetId()      
{
    return m_pImpl->GetId(); 
}
const wchar_t*  IStyleResItem::GetInherits()
{
    return m_pImpl->GetInherits(); 
}
const wchar_t*  IStyleResItem::GetAttribute(const wchar_t* szKey)
{
	return m_pImpl->GetAttribute(szKey);
}
// void  IStyleResItem::SetInherits(const wchar_t* sz)
// {
// 	return m_pImpl->SetInherits(sz);
// }
// void  IStyleResItem::SetSelectorType(STYLE_SELECTOR_TYPE type)
// {
// 	m_pImpl->SetSelectorType(type);
// }
// void  IStyleResItem::SetId(const wchar_t* szId)
// {
// 	m_pImpl->SetId(szId);
// }
// void  IStyleResItem::SetAttribute(const wchar_t* key, const wchar_t* value)
// {
// 	m_pImpl->SetAttribute(key, value);
// }
// 
// void  IStyleResItem::RemoveAttribute(const wchar_t* key)
// {
// 	m_pImpl->RemoveAttribute(key);
// }


void  IStyleResItem::GetAttributeMap(IMapAttribute** ppMapAttrib)
{ 
    m_pImpl->GetAttributeMap(ppMapAttrib);
}


void  IStyleResItem::SetXmlElement(IUIElement* p)
{
    m_pImpl->SetXmlElement(p ? p->GetImpl():nullptr);
}
IUIElement*  IStyleResItem::GetXmlElement()
{
    UIElement* p = m_pImpl->GetXmlElement();
    if (p)
        return p->GetIUIElement();

    return nullptr;
}
bool  IStyleResItem::GetXmlElement2(IUIElement** pp)
{
    UIElement* pp2 = nullptr;
    if (!m_pImpl->GetXmlElement2(&pp2))
        return false;

    *pp = pp2->GetIUIElement();
    return true;
}

//////////////////////////////////////////////////////////////////////////

IStyleRes::IStyleRes(StyleRes* p)
{ 
    m_pImpl = p;
}
StyleRes*  IStyleRes::GetImpl()   
{ 
    return m_pImpl;
}

IStyleResItem* IStyleRes::Add(STYLE_SELECTOR_TYPE type, const wchar_t* szId, const wchar_t* szInherit)
{
    StyleResItem* p = m_pImpl->Add(type, szId, szInherit);
    if (!p)
        return nullptr;

    return p->GetIStyleResItem();
}

IStyleResItem*  IStyleRes::Insert(STYLE_SELECTOR_TYPE type, const wchar_t* szId, long lPos)
{
	StyleResItem* p = m_pImpl->Insert(type, szId, lPos);
	if (!p)
		return nullptr;

	return p->GetIStyleResItem();
}

bool  IStyleRes::Modify(IStyleResItem* pItem, STYLE_SELECTOR_TYPE type, const wchar_t* szId, const wchar_t* szInherit)
{
	if (!pItem)
		return false;

	return m_pImpl->Modify(pItem->GetImpl(), type, szId, szInherit);
}

long  IStyleRes::GetItemPos(IStyleResItem* p)
{
	if (!p)
		return -1;

	return m_pImpl->GetItemPos(p->GetImpl());	
}

bool  IStyleRes::Remove(STYLE_SELECTOR_TYPE type, const wchar_t* szId)
{
    return m_pImpl->Remove(type, szId);
}

bool  IStyleRes::Remove(IStyleResItem* pItem)
{
	if (!pItem)
		return false;

	return m_pImpl->Remove(pItem->GetImpl());
}

bool  IStyleRes::AddAttribute(IStyleResItem* pItem, const wchar_t* szKey, const wchar_t* szValue)
{
	if (!pItem)
		return false;

	return m_pImpl->AddAttribute(pItem->GetImpl(), szKey, szValue);
}
bool  IStyleRes::ModifyAttribute(IStyleResItem* pItem, const wchar_t* szKey, const wchar_t* szValue)
{
	if (!pItem)
		return false;

	return m_pImpl->ModifyAttribute(pItem->GetImpl(), szKey, szValue);
}
bool  IStyleRes::RemoveAttribute(IStyleResItem* pItem, const wchar_t* szKey)
{
	if (!pItem)
		return false;

	return m_pImpl->RemoveAttribute(pItem->GetImpl(), szKey);
}

long  IStyleRes::GetStyleCount()
{ 
    return m_pImpl->GetCount(); 
}

IStyleResItem*  IStyleRes::GetItem(long lIndex) 
{
    StyleResItem* p = m_pImpl->GetItem(lIndex); 
    if (!p)
        return nullptr;

    return p->GetIStyleResItem();
}

IStyleResItem*  IStyleRes::FindItem(STYLE_SELECTOR_TYPE type, const wchar_t* szId)
{ 
    StyleResItem* p = m_pImpl->GetItem(type, szId); 
    if (!p)
        return nullptr;

    return p->GetIStyleResItem();
}


bool  IStyleRes::LoadStyle(
        const wchar_t* szTagName, 
        const wchar_t* szStyleClass,
        const wchar_t* szID, 
        IMapAttribute* pMapStyle)
{ 
    return m_pImpl->LoadStyle(szTagName, szStyleClass, szID, pMapStyle); 
}

bool  IStyleRes::UnloadStyle(
        const wchar_t* szTagName,
        const wchar_t* szStyleClass,
        const wchar_t* szID,
        IListAttribute* pListAttribte)
{
    return m_pImpl->UnloadStyle(szTagName, szStyleClass, szID, pListAttribte);
}

//////////////////////////////////////////////////////////////////////////


IResBundle::IResBundle(ResBundle* p)
{
    m_pImpl = p;
}
ResBundle*  IResBundle::GetImpl()
{
    return m_pImpl;
}

void  IResBundle::SetParentSkinRes(IResBundle* p)
{
	m_pImpl->SetParentSkinRes(p ? p->GetImpl() : nullptr);
}

IUIApplication*  IResBundle::GetUIApplication()  { return m_pImpl->GetUIApplication()->GetIUIApplication(); }
IResourceManager&    IResBundle::GetResourceManager()    { return m_pImpl->GetSkinMgr().GetIResourceManager(); }
IImageManager&   IResBundle::GetImageManager()   { return m_pImpl->GetImageManager().GetIImageManager(); }
IColorManager&   IResBundle::GetColorManager()   { return m_pImpl->GetColorManager().GetIColorManager(); }
IFontManager&    IResBundle::GetFontManager()    { return m_pImpl->GetFontManager().GetIFontManager(); }
IStyleManager&   IResBundle::GetStyleManager()   { return m_pImpl->GetStyleManager().GetIStyleManager(); }
ILayoutManager&  IResBundle::GetLayoutManager()  { return m_pImpl->GetLayoutManager().GetILayoutManager(); }

IImageRes&  IResBundle::GetImageRes()            { return m_pImpl->GetImageRes().GetIImageRes(); }
IFontRes&   IResBundle::GetFontRes()             { return m_pImpl->GetFontRes().GetIFontRes(); }
IColorRes&  IResBundle::GetColorRes()            { return m_pImpl->GetColorRes().GetIColorRes(); }
IStyleRes&  IResBundle::GetStyleRes()            { return m_pImpl->GetStyleRes().GetIStyleRes(); }
II18nRes&   IResBundle::GetI18nRes()             
{ 
	return m_pImpl->GetI18nRes().GetII18nRes(); 
}

const wchar_t*  IResBundle::GetName()              
{ 
    return m_pImpl->GetName();
}
const wchar_t*  IResBundle::GetPath()              
{ 
    return m_pImpl->GetPath();
}

bool  IResBundle::Load()                        
{ 
    return m_pImpl->Load();
}
bool  IResBundle::ChangeSkinHLS(short h, short l, short s, int nFlag) 
{ 
    return m_pImpl->ChangeSkinHLS(h,l,s,nFlag);
}
ISkinDataSource*  IResBundle::GetDataSource()    
{
    SkinDataSource* p = m_pImpl->GetDataSource(); 
	if (!p)
		return nullptr;

	return p->GetISkinDataSource();
}
unsigned int  IResBundle::GetXmlDocCount()               
{ 
    return m_pImpl->GetXmlDocCount();
}

IUIDocument*  IResBundle::GetXmlDoc(unsigned int nIndex)
{
	UIDocument* p = m_pImpl->GetXmlDoc(nIndex);
	if (p)
		return p->GetIUIDocument();
	return nullptr;
}
IUIDocument*  IResBundle::GetXmlDocByName(const wchar_t* szName)
{
	UIDocument* p = m_pImpl->GetXmlDocByName(szName);
	if (p)
		return p->GetIUIDocument();
	return nullptr;
}

IResourceManager::IResourceManager(ResourceManager* p)
{
    m_pImpl = p;
}
ResourceManager*  IResourceManager::GetImpl()
{
    return m_pImpl;
}

IUIApplication*  IResourceManager::GetUIApplication() 
{
    return m_pImpl->GetUIApplication()->GetIUIApplication();
}

void  IResourceManager::ChangeSkinHLS(short h, short l, short s, int nFlag) 
{
    m_pImpl->ChangeSkinHLS(h,l,s,nFlag);
}

bool  IResourceManager::Save(IResBundle* pSkinRes) 
{
    if (nullptr == pSkinRes)
        return m_pImpl->Save(nullptr);
    else
        return m_pImpl->Save(pSkinRes->GetImpl()); 
}

IResBundle*  IResourceManager::GetSkinResByName(const wchar_t* szName)
{
	ResBundle* p = m_pImpl->GetSkinResByName(szName);
	if (p)
		return p->GetIResBundle();

	return nullptr;
}

uint  IResourceManager::GetSkinResCount()
{
	return m_pImpl->GetSkinResCount();
}
IResBundle*  IResourceManager::GetSkinResByIndex(uint i)
{
	ResBundle* p = m_pImpl->GetSkinResByIndex(i);
	if (p)
		return p->GetIResBundle();

	return nullptr;
}

void  IResourceManager::SetCurrentLanguage(const wchar_t* szLang)
{
    m_pImpl->SetCurrentLanguage(szLang);
}
const wchar_t*  IResourceManager::GetCurrentLanguage()
{
    return m_pImpl->GetCurrentLanguage();
}

//////////////////////////////////////////////////////////////////////////

II18nRes::II18nRes(I18nRes* p)
{
	m_pImpl = p;
}
const wchar_t*  II18nRes::MapConfigValue(const wchar_t* config)
{
    return m_pImpl->MapConfigValue(config);
}