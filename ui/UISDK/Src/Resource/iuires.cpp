#include "stdafx.h"
#include "colormanager.h"
#include "Inc/Interface/iuires.h"
#include "colorres.h"
#include "imagemanager.h"
#include "../SkinParse/xml/xmlwrap.h"
#include "fontmanager.h"
#include "stylemanager.h"
#include "layoutmanager.h"
#include "Inc/Interface/ixmlwrap.h"
#include "skinres.h"
#include "../Base/Application/uiapplication.h"
#include "../SkinParse/datasource/skindatasource.h"
#include "skinmanager.h"
#include "i18nres.h"
#include "../Base/Object/object.h"



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
void  IColorRes::GetColor(LPCTSTR szColorId, Color** pp) 
{ 
	return m_pImpl->GetColor(szColorId, pp); 
}
LPCTSTR  IColorRes::GetColorId(Color* p)
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

LPCTSTR IColorResItem::GetId() 
{ 
	return m_pImpl->GetId();
}
LPCTSTR IColorResItem::GetColorString()
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
ICursorRes*  IImageManager::GetCursorRes()
{
    return m_pImpl->GetCursorRes().GetICursorRes();
}
IGifRes*     IImageManager::GetGifRes() 
{ 
    return m_pImpl->GetGifRes().GetIGifRes(); 
}
IUIElement*  IImageManager::GetImageXmlElem(LPCTSTR szId)  
{ 
    UIElement* p = m_pImpl->GetImageXmlElem(szId); 
    if (p)
        return p->GetIUIElement();
    return nullptr;
}

IImageResItem*  IImageManager::InsertImageItem(IMAGE_ITEM_TYPE eType, LPCTSTR szID, LPCTSTR szPath) 
{ 
    return m_pImpl->InsertImageItem(eType, szID, szPath);
}
bool  IImageManager::ModifyImageItem(LPCTSTR szID, LPCTSTR szPath)
{ 
    return m_pImpl->ModifyImageItem(szID, szPath);
}
bool  IImageManager::RemoveImageItem(LPCTSTR szID)
{ 
    return m_pImpl->RemoveImageItem(szID);
}

bool  IImageManager::ModifyImageItemInRunTime(LPCTSTR szID, LPCTSTR szPath) 
{ 
    return m_pImpl->ModifyImageItemInRunTime(szID, szPath); 
}
bool  IImageManager::ModifyImageItemAlpha(LPCTSTR szID, byte nAlphaPercent) 
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

IUIElement*  IStyleManager::GetStyleXmlElem(LPCTSTR szId)
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
            LPCTSTR szWndId, IObject* pParent, IMessage* pNotifyTarget)
{
    Object* pObj = m_pImpl->LoadPluginLayout(
            szWndId, 
            pParent?pParent->GetImpl():nullptr,
            pNotifyTarget);

    if (!pObj)
        return nullptr;

    return pObj->GetIObject();
}
IObject*  ILayoutManager::ParseElement(IUIElement* pUIElement, IObject* pParent)
{
	if (!pUIElement)
		return nullptr;

    Object* pObj = m_pImpl->ParseElement(
            pUIElement->GetImpl(),
            pParent ? pParent->GetImpl():nullptr);

    if (!pObj)
        return nullptr;

    return pObj->GetIObject();
}

bool  ILayoutManager::FindWindowElement(LPCTSTR szTagName, LPCTSTR szId, IUIElement** ppElem)
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
IImageResItem*  IImageRes::GetImageResItem(LPCTSTR szID) 
{ 
    return m_pImpl->GetImageResItem(szID); 
}
bool  IImageRes::ModifyImage(LPCTSTR szId, LPCTSTR szPath)
{ 
    return m_pImpl->ModifyImage(szId, szPath);
}
bool  IImageRes::GetBitmap(LPCTSTR szImageID, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderBitmap** ppRenderBitmap)
{ 
    return m_pImpl->GetBitmap(szImageID, eRenderType, ppRenderBitmap);
}
HBITMAP  IImageRes::LoadBitmap(LPCTSTR szId)  
{ 
    return m_pImpl->LoadBitmap(szId);
}
LPCTSTR  IImageRes::GetRenderBitmapId(IRenderBitmap* pBitmap)
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

void  IImageResItem::SetId(LPCTSTR szText)
{ 
    m_pImpl->SetId(szText); 
}
void  IImageResItem::SetPath(LPCTSTR szText) 
{
    m_pImpl->SetPath(szText); 
}
LPCTSTR  IImageResItem::GetId() 
{ 
    return m_pImpl->GetId(); 
}
LPCTSTR  IImageResItem::GetPath() 
{ 
    return m_pImpl->GetPath();
}
bool  IImageResItem::ModifyImage(LPCTSTR  szPath)
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


ICursorRes::ICursorRes(CursorRes* p) { m_pImpl = p; }
CursorRes*  ICursorRes::GetImpl() { return m_pImpl; }

void ICursorRes::GetCursor(LPCTSTR  szCursorID, IUICursor** pp) 
{ 
    m_pImpl->GetCursor(szCursorID, pp); 
}
LPCTSTR  ICursorRes::GetCursorId(IUICursor* p)
{
    return m_pImpl->GetCursorId(p); 
}

ICursorResItem::ICursorResItem(CursorResItem* p) { m_pImpl = p; }
CursorResItem*  ICursorResItem::GetImpl()  { return m_pImpl; }

//////////////////////////////////////////////////////////////////////////

IFontResItem::IFontResItem(FontResItem* p) { m_pImpl = p; }
FontResItem*  IFontResItem::GetImpl(){ return m_pImpl; }

LPCTSTR  IFontResItem::GetId()  { return m_pImpl->GetId(); }
LOGFONT*  IFontResItem::GetLogFont() { return m_pImpl->GetLogFont(); }

//////////////////////////////////////////////////////////////////////////

IGifRes::IGifRes(GifRes* p) { m_pImpl = p; }
GifRes*  IGifRes::GetImpl()  { return m_pImpl; }

HRESULT  IGifRes::GetGifImage(LPCTSTR  szId, __out IGifImage** ppGifImage) 
{
	IGifImage* p = m_pImpl->GetGifImage(szId); 
	if (ppGifImage)
		*ppGifImage = p;

	return S_OK;
}

IGifResItem::IGifResItem(GifResItem* p) { m_pImpl = p; }
GifResItem*  IGifResItem::GetImpl()  { return m_pImpl; }

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
bool  IFontRes::GetFont(LPCTSTR szFontId, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppOut) 
{
    return m_pImpl->GetFont(szFontId, eRenderType, ppOut); 
}
bool  IFontRes::GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppFont)
{
    return m_pImpl->GetDefaultFont(eRenderType, ppFont); 
}
LPCTSTR  IFontRes::GetDefaultFontId()
{
    return m_pImpl->GetDefaultFontId();
}
LPCTSTR  IFontRes::GetRenderFontId(IRenderFont* pFont)
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
LPCTSTR  IStyleResItem::GetId()      
{
    return m_pImpl->GetId(); 
}
LPCTSTR  IStyleResItem::GetInherits()
{
    return m_pImpl->GetInherits(); 
}
LPCTSTR  IStyleResItem::GetAttribute(LPCTSTR szKey)
{
	return m_pImpl->GetAttribute(szKey);
}
// void  IStyleResItem::SetInherits(LPCTSTR sz)
// {
// 	return m_pImpl->SetInherits(sz);
// }
// void  IStyleResItem::SetSelectorType(STYLE_SELECTOR_TYPE type)
// {
// 	m_pImpl->SetSelectorType(type);
// }
// void  IStyleResItem::SetId(LPCTSTR szId)
// {
// 	m_pImpl->SetId(szId);
// }
// void  IStyleResItem::SetAttribute(LPCTSTR key, LPCTSTR value)
// {
// 	m_pImpl->SetAttribute(key, value);
// }
// 
// void  IStyleResItem::RemoveAttribute(LPCTSTR key)
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

IStyleResItem* IStyleRes::Add(STYLE_SELECTOR_TYPE type, LPCTSTR szId, LPCTSTR szInherit)
{
    StyleResItem* p = m_pImpl->Add(type, szId, szInherit);
    if (!p)
        return nullptr;

    return p->GetIStyleResItem();
}

IStyleResItem*  IStyleRes::Insert(STYLE_SELECTOR_TYPE type, LPCTSTR szId, long lPos)
{
	StyleResItem* p = m_pImpl->Insert(type, szId, lPos);
	if (!p)
		return nullptr;

	return p->GetIStyleResItem();
}

bool  IStyleRes::Modify(IStyleResItem* pItem, STYLE_SELECTOR_TYPE type, LPCTSTR szId, LPCTSTR szInherit)
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

bool  IStyleRes::Remove(STYLE_SELECTOR_TYPE type, LPCTSTR szId)
{
    return m_pImpl->Remove(type, szId);
}

bool  IStyleRes::Remove(IStyleResItem* pItem)
{
	if (!pItem)
		return false;

	return m_pImpl->Remove(pItem->GetImpl());
}

bool  IStyleRes::AddAttribute(IStyleResItem* pItem, LPCTSTR szKey, LPCTSTR szValue)
{
	if (!pItem)
		return false;

	return m_pImpl->AddAttribute(pItem->GetImpl(), szKey, szValue);
}
bool  IStyleRes::ModifyAttribute(IStyleResItem* pItem, LPCTSTR szKey, LPCTSTR szValue)
{
	if (!pItem)
		return false;

	return m_pImpl->ModifyAttribute(pItem->GetImpl(), szKey, szValue);
}
bool  IStyleRes::RemoveAttribute(IStyleResItem* pItem, LPCTSTR szKey)
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

IStyleResItem*  IStyleRes::FindItem(STYLE_SELECTOR_TYPE type, LPCTSTR szId)
{ 
    StyleResItem* p = m_pImpl->GetItem(type, szId); 
    if (!p)
        return nullptr;

    return p->GetIStyleResItem();
}


bool  IStyleRes::LoadStyle(
        LPCTSTR szTagName, 
        LPCTSTR szStyleClass,
        LPCTSTR szID, 
        IMapAttribute* pMapStyle)
{ 
    return m_pImpl->LoadStyle(szTagName, szStyleClass, szID, pMapStyle); 
}

bool  IStyleRes::UnloadStyle(
        LPCTSTR szTagName,
        LPCTSTR szStyleClass,
        LPCTSTR szID,
        IListAttribute* pListAttribte)
{
    return m_pImpl->UnloadStyle(szTagName, szStyleClass, szID, pListAttribte);
}

//////////////////////////////////////////////////////////////////////////


ISkinRes::ISkinRes(SkinRes* p)
{
    m_pImpl = p;
}
SkinRes*  ISkinRes::GetImpl()
{
    return m_pImpl;
}

void  ISkinRes::SetParentSkinRes(ISkinRes* p)
{
	m_pImpl->SetParentSkinRes(p ? p->GetImpl() : nullptr);
}

IUIApplication*  ISkinRes::GetUIApplication()  { return m_pImpl->GetUIApplication()->GetIUIApplication(); }
ISkinManager&    ISkinRes::GetSkinManager()    { return m_pImpl->GetSkinMgr().GetISkinManager(); }
IImageManager&   ISkinRes::GetImageManager()   { return m_pImpl->GetImageManager().GetIImageManager(); }
IColorManager&   ISkinRes::GetColorManager()   { return m_pImpl->GetColorManager().GetIColorManager(); }
IFontManager&    ISkinRes::GetFontManager()    { return m_pImpl->GetFontManager().GetIFontManager(); }
IStyleManager&   ISkinRes::GetStyleManager()   { return m_pImpl->GetStyleManager().GetIStyleManager(); }
ILayoutManager&  ISkinRes::GetLayoutManager()  { return m_pImpl->GetLayoutManager().GetILayoutManager(); }

IImageRes&  ISkinRes::GetImageRes()            { return m_pImpl->GetImageRes().GetIImageRes(); }
IFontRes&   ISkinRes::GetFontRes()             { return m_pImpl->GetFontRes().GetIFontRes(); }
IColorRes&  ISkinRes::GetColorRes()            { return m_pImpl->GetColorRes().GetIColorRes(); }
IStyleRes&  ISkinRes::GetStyleRes()            { return m_pImpl->GetStyleRes().GetIStyleRes(); }
II18nRes&   ISkinRes::GetI18nRes()             
{ 
	return m_pImpl->GetI18nRes().GetII18nRes(); 
}

LPCTSTR  ISkinRes::GetName()              
{ 
    return m_pImpl->GetName();
}
LPCTSTR  ISkinRes::GetPath()              
{ 
    return m_pImpl->GetPath();
}

bool  ISkinRes::Load()                        
{ 
    return m_pImpl->Load();
}
bool  ISkinRes::ChangeSkinHLS(short h, short l, short s, int nFlag) 
{ 
    return m_pImpl->ChangeSkinHLS(h,l,s,nFlag);
}
ISkinDataSource*  ISkinRes::GetDataSource()    
{
    SkinDataSource* p = m_pImpl->GetDataSource(); 
	if (!p)
		return nullptr;

	return p->GetISkinDataSource();
}
UINT  ISkinRes::GetXmlDocCount()               
{ 
    return m_pImpl->GetXmlDocCount();
}

IUIDocument*  ISkinRes::GetXmlDoc(UINT nIndex)
{
	UIDocument* p = m_pImpl->GetXmlDoc(nIndex);
	if (p)
		return p->GetIUIDocument();
	return nullptr;
}
IUIDocument*  ISkinRes::GetXmlDocByName(LPCTSTR szName)
{
	UIDocument* p = m_pImpl->GetXmlDocByName(szName);
	if (p)
		return p->GetIUIDocument();
	return nullptr;
}

ISkinManager::ISkinManager(SkinManager* p)
{
    m_pImpl = p;
}
SkinManager*  ISkinManager::GetImpl()
{
    return m_pImpl;
}

IUIApplication*  ISkinManager::GetUIApplication() 
{
    return m_pImpl->GetUIApplication()->GetIUIApplication();
}

void  ISkinManager::ChangeSkinHLS(short h, short l, short s, int nFlag) 
{
    m_pImpl->ChangeSkinHLS(h,l,s,nFlag);
}

bool  ISkinManager::Save(ISkinRes* pSkinRes) 
{
    if (nullptr == pSkinRes)
        return m_pImpl->Save(nullptr);
    else
        return m_pImpl->Save(pSkinRes->GetImpl()); 
}

ISkinRes*  ISkinManager::GetSkinResByName(LPCTSTR szName)
{
	SkinRes* p = m_pImpl->GetSkinResByName(szName);
	if (p)
		return p->GetISkinRes();

	return nullptr;
}

uint  ISkinManager::GetSkinResCount()
{
	return m_pImpl->GetSkinResCount();
}
ISkinRes*  ISkinManager::GetSkinResByIndex(uint i)
{
	SkinRes* p = m_pImpl->GetSkinResByIndex(i);
	if (p)
		return p->GetISkinRes();

	return nullptr;
}

void  ISkinManager::SetCurrentLanguage(LPCTSTR szLang)
{
    m_pImpl->SetCurrentLanguage(szLang);
}
LPCTSTR  ISkinManager::GetCurrentLanguage()
{
    return m_pImpl->GetCurrentLanguage();
}

//////////////////////////////////////////////////////////////////////////

II18nRes::II18nRes(I18nRes* p)
{
	m_pImpl = p;
}
LPCTSTR  II18nRes::MapConfigValue(LPCTSTR config)
{
    return m_pImpl->MapConfigValue(config);
}