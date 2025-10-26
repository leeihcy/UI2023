#include "include/interface/iuires.h"
#include "include/interface/iresbundle.h"
#include "res_bundle.h"
#include "resource_manager.h"
#include "src/resource/imageres.h"
#include "src/application/uiapplication.h"
#include "src/parser/datasource/bundle_source.h"
#include "include/interface/ixmlwrap.h"
#include <memory>

using namespace ui;

IColorManager::IColorManager(ColorManager *p) { m_pImpl = p; }
ColorManager *IColorManager::GetImpl() { return m_pImpl; }

IColorRes &IColorManager::GetColorRes() {
  return m_pImpl->GetColorRes().GetIColorRes();
}

IColorRes::IColorRes(ColorRes *p) { m_pImpl = p; }
ColorRes *IColorRes::GetImpl() { return m_pImpl; }

long IColorRes::GetCount() { return m_pImpl->GetCount(); }
// IColorResItem*  IColorRes::GetItem(long lIndex)
// {
// 	return m_pImpl->GetItem(lIndex);
// }
Color IColorRes::GetColor(const char *szColorId, bool *exist) {
  return m_pImpl->GetColor(szColorId, exist);
}
const char *IColorRes::GetColorId(Color *p) { return m_pImpl->GetColorId(p); }

//////////////////////////////////////////////////////////////////////////

IColorResItem::IColorResItem(ColorResItem *p) { m_pImpl = p; }
ColorResItem *IColorResItem::GetImpl() { return m_pImpl; }

const char *IColorResItem::GetId() { return m_pImpl->GetId(); }
const char *IColorResItem::GetColorString() {
  return m_pImpl->GetColorString();
}

//////////////////////////////////////////////////////////////////////////

IStyleManager::IStyleManager(StyleManager *p) { m_pImpl = p; }
StyleManager *IStyleManager::GetImpl() { return m_pImpl; }

IStyleRes &IStyleManager::GetStyleRes() {
  return m_pImpl->GetStyleRes().GetIStyleRes();
}

IUIElement *IStyleManager::GetStyleXmlElem(const char *szId) {
  UIElement *p = m_pImpl->GetStyleXmlElem(szId);
  if (p)
    return p->GetIUIElement();

  return nullptr;
}

//////////////////////////////////////////////////////////////////////////

ILayoutManager::ILayoutManager(LayoutManager *p) { m_pImpl = p; }
LayoutManager *ILayoutManager::GetImpl() { return m_pImpl; }

bool ILayoutManager::LoadWindowNodeList(ILayoutWindowNodeList **pp) {
  return m_pImpl->LoadWindowNodeList(pp);
}

IObject *ILayoutManager::LoadPluginLayout(const char *szWndId, IObject *pParent,
                                          IMessage *pNotifyTarget) {
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
IObject *ILayoutManager::ParseElement(IUIElement *pUIElement,
                                      IObject *pParent) {
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

bool ILayoutManager::FindWindowElement(const char *szTagName, const char *szId,
                                       IUIElement **ppElem) {
  UIElementProxy p = m_pImpl->FindWindowElement(szTagName, szId);
  if (!p)
    return false;

  if (ppElem) {
    *ppElem = p->GetIUIElement();
    p->AddRef();
  }
  return true;
}
//////////////////////////////////////////////////////////////////////////

IImageRes::IImageRes(ImageRes *p) { m_pImpl = p; }
ImageRes *IImageRes::GetImpl() { return m_pImpl; }

long IImageRes::GetImageCount() { return m_pImpl->GetImageCount(); }
IImageResItem *IImageRes::GetImageResItem(long lIndex) {
  return m_pImpl->GetImageResItem(lIndex);
}
IImageResItem *IImageRes::GetImageResItem(const char *szID) {
  return m_pImpl->GetImageResItem(szID);
}
bool IImageRes::ModifyImage(const char *szId, const char *szPath) {
  return m_pImpl->ModifyImage(szId, szPath);
}
std::shared_ptr<IRenderBitmap>
IImageRes::GetBitmap(const char *szImageID,
                     GRAPHICS_RENDER_LIBRARY_TYPE eRenderType) {
  return m_pImpl->GetBitmap(szImageID, eRenderType);
}
#if 0 // defined(OS_WIN)
HBITMAP  IImageRes::LoadBitmap(const char* szId)  
{ 
    return m_pImpl->LoadBitmap(szId);
}
#endif
const char *IImageRes::GetRenderBitmapId(IRenderBitmap *pBitmap) {
  return m_pImpl->GetRenderBitmapId(pBitmap);
}
//////////////////////////////////////////////////////////////////////////

IImageResItem::IImageResItem(ImageResItem *p) { m_pImpl = p; }
ImageResItem *IImageResItem::GetImpl() { return m_pImpl; }

void IImageResItem::SetId(const char *szText) { m_pImpl->SetId(szText); }
void IImageResItem::SetPath(const char *szText) { m_pImpl->SetPath(szText); }
const char *IImageResItem::GetId() { return m_pImpl->GetId(); }
const char *IImageResItem::GetPath() { return m_pImpl->GetPath(); }
bool IImageResItem::ModifyImage(const char *szPath) {
  return m_pImpl->ModifyImage(szPath);
}
bool IImageResItem::GetUseSkinHLS() { return m_pImpl->GetUseSkinHLS(); }
bool IImageResItem::GetNeedAntiAliasing() {
  return m_pImpl->GetNeedAntiAliasing();
}
void IImageResItem::SetUseSkinHLS(bool b) { return m_pImpl->SetUseSkinHLS(b); }
void IImageResItem::SetNeedAntiAliasing(bool b) {
  return m_pImpl->SetNeedAntiAliasing(b);
}
IMAGE_ITEM_TYPE IImageResItem::GetImageItemType() {
  return m_pImpl->GetImageItemType();
}
// void  IImageResItem::SetImageItemType(IMAGE_ITEM_TYPE e)
// {
//     m_pImpl->SetImageItemType(e);
// }

//////////////////////////////////////////////////////////////////////////

IImageListResItem::IImageListResItem(ImageListResItem *p)
    : IImageResItem(static_cast<ImageResItem *>(p)) {
  m_pImpl = p;
}
ImageListResItem *IImageListResItem::GetImpl() { return m_pImpl; }

IMAGELIST_LAYOUT_TYPE IImageListResItem::GetLayoutType() {
  return m_pImpl->GetLayoutType();
}
int IImageListResItem::GetItemCount() { return m_pImpl->GetItemCount(); }
void IImageListResItem::SetLayoutType(IMAGELIST_LAYOUT_TYPE e) {
  m_pImpl->SetLayoutType(e);
}
void IImageListResItem::SetItemCount(int n) { m_pImpl->SetItemCount(n); }

//////////////////////////////////////////////////////////////////////////

IImageIconResItem::IImageIconResItem(ImageIconResItem *p)
    : IImageResItem(static_cast<ImageResItem *>(p)) {
  m_pImpl = p;
}
ImageIconResItem *IImageIconResItem::GetImpl() { return m_pImpl; }

//////////////////////////////////////////////////////////////////////////

#if 0 // defined(OS_WIN)
ICursorRes::ICursorRes(CursorRes* p) { m_pImpl = p; }
CursorRes*  ICursorRes::GetImpl() { return m_pImpl; }

void ICursorRes::GetCursor(const char*  szCursorID, IUICursor** pp) 
{ 
    m_pImpl->GetCursor(szCursorID, pp); 
}
const char*  ICursorRes::GetCursorId(IUICursor* p)
{
    return m_pImpl->GetCursorId(p); 
}

ICursorResItem::ICursorResItem(CursorResItem* p) { m_pImpl = p; }
CursorResItem*  ICursorResItem::GetImpl()  { return m_pImpl; }
#endif
//////////////////////////////////////////////////////////////////////////
#if 0
IFontResItem::IFontResItem(FontResItem *p) { m_pImpl = p; }
FontResItem *IFontResItem::GetImpl() { return m_pImpl; }

const char *IFontResItem::GetId() { return m_pImpl->GetId(); }
LOGFONT*  IFontResItem::GetLogFont() { return m_pImpl->GetLogFont(); }
#endif
//////////////////////////////////////////////////////////////////////////

#if 0 // defined(OS_WIN)
IGifRes::IGifRes(GifRes* p) { m_pImpl = p; }
GifRes*  IGifRes::GetImpl()  { return m_pImpl; }

long  IGifRes::GetGifImage(const char*  szId, IGifImage** ppGifImage) 
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
#if 0
IFontRes::IFontRes(FontRes *p) { m_pImpl = p; }
FontRes *IFontRes::GetImpl() { return m_pImpl; }

long IFontRes::GetFontCount() { return m_pImpl->GetFontCount(); }
bool IFontRes::GetFontResItem(long lIndex, IFontResItem **ppResItem) {
  return m_pImpl->GetFontResItem(lIndex, ppResItem);
}
bool IFontRes::GetFont(const char *szFontId,
                       GRAPHICS_RENDER_LIBRARY_TYPE eRenderType,
                       IRenderFont **ppOut) {
  return m_pImpl->GetFont(szFontId, eRenderType, ppOut);
}
bool IFontRes::GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType,
                              IRenderFont **ppFont) {
  return m_pImpl->GetDefaultFont(eRenderType, ppFont);
}
const char *IFontRes::GetDefaultFontId() { return m_pImpl->GetDefaultFontId(); }
const char *IFontRes::GetRenderFontId(IRenderFont *pFont) {
  return m_pImpl->GetRenderFontId(pFont);
}
#endif
//////////////////////////////////////////////////////////////////////////

IStyleResItem::IStyleResItem(StyleResItem *p) { m_pImpl = p; }
StyleResItem *IStyleResItem::GetImpl() { return m_pImpl; }

STYLE_SELECTOR_TYPE IStyleResItem::GetSelectorType() {
  return m_pImpl->GetSelectorType();
}
const char *IStyleResItem::GetId() { return m_pImpl->GetId(); }
const char *IStyleResItem::GetInherits() { return m_pImpl->GetInherits(); }
const char *IStyleResItem::GetAttribute(const char *szKey) {
  return m_pImpl->GetAttribute(szKey);
}
// void  IStyleResItem::SetInherits(const char* sz)
// {
// 	return m_pImpl->SetInherits(sz);
// }
// void  IStyleResItem::SetSelectorType(STYLE_SELECTOR_TYPE type)
// {
// 	m_pImpl->SetSelectorType(type);
// }
// void  IStyleResItem::SetId(const char* szId)
// {
// 	m_pImpl->SetId(szId);
// }
// void  IStyleResItem::SetAttribute(const char* key, const char* value)
// {
// 	m_pImpl->SetAttribute(key, value);
// }
//
// void  IStyleResItem::RemoveAttribute(const char* key)
// {
// 	m_pImpl->RemoveAttribute(key);
// }

std::shared_ptr<IAttributeMap> IStyleResItem::GetAttributeMap() {
  return m_pImpl->GetAttributeMap();
}

void IStyleResItem::SetXmlElement(IUIElement *p) {
  m_pImpl->SetXmlElement(p ? p->GetImpl() : nullptr);
}
IUIElement *IStyleResItem::GetXmlElement() {
  UIElement *p = m_pImpl->GetXmlElement();
  if (p)
    return p->GetIUIElement();

  return nullptr;
}
bool IStyleResItem::GetXmlElement2(IUIElement **pp) {
  UIElement *pp2 = nullptr;
  if (!m_pImpl->GetXmlElement2(&pp2))
    return false;

  *pp = pp2->GetIUIElement();
  return true;
}

//////////////////////////////////////////////////////////////////////////

IStyleRes::IStyleRes(StyleRes *p) { m_pImpl = p; }
StyleRes *IStyleRes::GetImpl() { return m_pImpl; }

IStyleResItem *IStyleRes::Add(STYLE_SELECTOR_TYPE type, const char *szId,
                              const char *szInherit) {
  StyleResItem *p = m_pImpl->Add(type, szId, szInherit);
  if (!p)
    return nullptr;

  return p->GetIStyleResItem();
}

IStyleResItem *IStyleRes::Insert(STYLE_SELECTOR_TYPE type, const char *szId,
                                 long lPos) {
  StyleResItem *p = m_pImpl->Insert(type, szId, lPos);
  if (!p)
    return nullptr;

  return p->GetIStyleResItem();
}

bool IStyleRes::Modify(IStyleResItem *pItem, STYLE_SELECTOR_TYPE type,
                       const char *szId, const char *szInherit) {
  if (!pItem)
    return false;

  return m_pImpl->Modify(pItem->GetImpl(), type, szId, szInherit);
}

long IStyleRes::GetItemPos(IStyleResItem *p) {
  if (!p)
    return -1;

  return m_pImpl->GetItemPos(p->GetImpl());
}

bool IStyleRes::Remove(STYLE_SELECTOR_TYPE type, const char *szId) {
  return m_pImpl->Remove(type, szId);
}

bool IStyleRes::Remove(IStyleResItem *pItem) {
  if (!pItem)
    return false;

  return m_pImpl->Remove(pItem->GetImpl());
}

bool IStyleRes::AddAttribute(IStyleResItem *pItem, const char *szKey,
                             const char *szValue) {
  if (!pItem)
    return false;

  return m_pImpl->AddAttribute(pItem->GetImpl(), szKey, szValue);
}
bool IStyleRes::ModifyAttribute(IStyleResItem *pItem, const char *szKey,
                                const char *szValue) {
  if (!pItem)
    return false;

  return m_pImpl->ModifyAttribute(pItem->GetImpl(), szKey, szValue);
}
bool IStyleRes::RemoveAttribute(IStyleResItem *pItem, const char *szKey) {
  if (!pItem)
    return false;

  return m_pImpl->RemoveAttribute(pItem->GetImpl(), szKey);
}

long IStyleRes::GetStyleCount() { return m_pImpl->GetCount(); }

IStyleResItem *IStyleRes::GetItem(long lIndex) {
  StyleResItem *p = m_pImpl->GetItem(lIndex);
  if (!p)
    return nullptr;

  return p->GetIStyleResItem();
}

IStyleResItem *IStyleRes::FindItem(STYLE_SELECTOR_TYPE type, const char *szId) {
  StyleResItem *p = m_pImpl->GetItem(type, szId);
  if (!p)
    return nullptr;

  return p->GetIStyleResItem();
}

bool IStyleRes::LoadStyle(const char *szTagName, const char *szStyleClass,
                          const char *szID, IAttributeMap *pMapStyle) {
  return m_pImpl->LoadStyle(szTagName, szStyleClass, szID, pMapStyle);
}

bool IStyleRes::UnloadStyle(const char *szTagName, const char *szStyleClass,
                            const char *szID, IAttributeList *pListAttribte) {
  return m_pImpl->UnloadStyle(szTagName, szStyleClass, szID, pListAttribte);
}

//////////////////////////////////////////////////////////////////////////

IResourceBundle::IResourceBundle(ResourceBundle *p) { m_pImpl = p; }
ResourceBundle *IResourceBundle::GetImpl() { return m_pImpl; }

void IResourceBundle::SetParentSkinRes(IResourceBundle *p) {
  m_pImpl->SetParentSkinRes(p ? p->GetImpl() : nullptr);
}

IApplication *IResourceBundle::GetUIApplication() {
  return m_pImpl->GetUIApplication()->GetIUIApplication();
}
IResourceManager &IResourceBundle::GetResourceManager() {
  return m_pImpl->GetSkinMgr().GetIResourceManager();
}
IColorManager &IResourceBundle::GetColorManager() {
  return m_pImpl->GetColorManager().GetIColorManager();
}
IStyleManager &IResourceBundle::GetStyleManager() {
  return m_pImpl->GetStyleManager().GetIStyleManager();
}
ILayoutManager &IResourceBundle::GetLayoutManager() {
  return m_pImpl->GetLayoutManager().GetILayoutManager();
}

IColorRes &IResourceBundle::GetColorRes() {
  return m_pImpl->GetColorRes().GetIColorRes();
}
IStyleRes &IResourceBundle::GetStyleRes() {
  return m_pImpl->GetStyleRes().GetIStyleRes();
}
II18nRes &IResourceBundle::GetI18nRes() {
  return m_pImpl->GetI18nRes().GetII18nRes();
}

const char *IResourceBundle::GetName() { return m_pImpl->GetName(); }
const char *IResourceBundle::GetPath() { return m_pImpl->GetPath(); }

bool IResourceBundle::Load() { return m_pImpl->Load(); }
bool IResourceBundle::ChangeSkinHLS(short h, short l, short s, int nFlag) {
  return m_pImpl->ChangeSkinHLS(h, l, s, nFlag);
}
IBundleSource *IResourceBundle::GetSource() {
  BundleSource *p = m_pImpl->GetSource();
  if (!p)
    return nullptr;

  return p->GetIBundleSource();
}

unsigned int IResourceBundle::GetXmlDocCount() { return m_pImpl->GetXmlDocCount(); }

IUIDocument *IResourceBundle::GetXmlDoc(unsigned int nIndex) {
  UIDocument *p = m_pImpl->GetXmlDoc(nIndex);
  if (p)
    return p->GetIUIDocument();
  return nullptr;
}
IUIDocument *IResourceBundle::GetXmlDocByName(const char *szName) {
  UIDocument *p = m_pImpl->GetXmlDocByName(szName);
  if (p)
    return p->GetIUIDocument();
  return nullptr;
}

IResourceManager::IResourceManager(ResourceManager *p) { m_pImpl = p; }
ResourceManager *IResourceManager::GetImpl() { return m_pImpl; }

IApplication *IResourceManager::GetUIApplication() {
  return m_pImpl->GetUIApplication()->GetIUIApplication();
}

void IResourceManager::ChangeSkinHLS(short h, short l, short s, int nFlag) {
  m_pImpl->ChangeSkinHLS(h, l, s, nFlag);
}

bool IResourceManager::Save(IResourceBundle *resource_bundle) {
  if (nullptr == resource_bundle)
    return m_pImpl->Save(nullptr);
  else
    return m_pImpl->Save(resource_bundle->GetImpl());
}

IResourceBundle *IResourceManager::GetResourceByName(const char *szName) {
  ResourceBundle *p = m_pImpl->GetResourceByName(szName);
  if (p)
    return p->GetIResource();

  return nullptr;
}

uint IResourceManager::GetResourceCount() {
  return m_pImpl->GetResourceCount();
}
IResourceBundle *IResourceManager::GetResourceByIndex(uint i) {
  ResourceBundle *p = m_pImpl->GetResourceByIndex(i);
  if (p)
    return p->GetIResource();

  return nullptr;
}

void IResourceManager::SetCurrentLanguage(const char *szLang) {
  m_pImpl->SetCurrentLanguage(szLang);
}
const char *IResourceManager::GetCurrentLanguage() {
  return m_pImpl->GetCurrentLanguage();
}

//////////////////////////////////////////////////////////////////////////

II18nRes::II18nRes(I18nRes *p) { m_pImpl = p; }
const char *II18nRes::MapConfigValue(const char *config) {
  return m_pImpl->MapConfigValue(config);
}