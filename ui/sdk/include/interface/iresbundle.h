#ifndef _UI_ISKINRES_H_
#define _UI_ISKINRES_H_

#include "sdk/include/macro/uidefine.h"
#include "sdk/include/common/signalslot/slot.h"

namespace ui {
struct IResourceManager;
struct IImageManager;
struct IImageManager;
struct IColorManager;
struct IFontManager;
struct IStyleManager;
struct ILayoutManager;
struct IImageRes;
struct IFontRes;
struct IColorRes;
struct IStyleRes;
struct ISkinDataSource;
struct IUIDocument;
struct II18nRes;

class Resource;
struct UIAPI IResource {
  IResource(Resource *);
  Resource *GetImpl();
  void SetParentSkinRes(IResource *);

  IApplication *GetUIApplication();
  IResourceManager &GetResourceManager();
  IImageManager &GetImageManager();
  IColorManager &GetColorManager();
  IFontManager &GetFontManager();
  IStyleManager &GetStyleManager();
  ILayoutManager &GetLayoutManager();
  IImageRes &GetImageRes();
  IFontRes &GetFontRes();
  IColorRes &GetColorRes();
  IStyleRes &GetStyleRes();
  II18nRes &GetI18nRes();

  ISkinDataSource *GetDataSource();

  unsigned int GetXmlDocCount();
  IUIDocument *GetXmlDoc(unsigned int nIndex);
  IUIDocument *GetXmlDocByName(const char *szName);

  const char *GetName();
  const char *GetPath();

  bool Load();
  bool ChangeSkinHLS(short h, short l, short s, int nFlag);

private:
  Resource *m_pImpl;
};

struct ISkinDataSource;
class ResourceManager;
struct UIAPI IResourceManager : public IRootInterface {
  IResourceManager(ResourceManager *);
  ResourceManager *GetImpl();

  IApplication *GetUIApplication();
  void ChangeSkinHLS(short h, short l, short s, int nFlag);
  bool Save(IResource *pSkinRes);

  unsigned int GetResourceCount();
  IResource *GetResourceByIndex(unsigned int i);
  IResource *GetResourceByName(const char *szName);

  void SetCurrentLanguage(const char *);
  const char *GetCurrentLanguage();

private:
  ResourceManager *m_pImpl;
};
} // namespace ui
#endif