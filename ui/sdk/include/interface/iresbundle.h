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
struct IBundleSource;
struct IUIDocument;
struct II18nRes;

class ResourceBundle;
struct UIAPI IResourceBundle {
  IResourceBundle(ResourceBundle *);
  ResourceBundle *GetImpl();
  void SetParentSkinRes(IResourceBundle *);

  IApplication *GetUIApplication();
  IResourceManager &GetResourceManager();
  IColorManager &GetColorManager();
  IFontManager &GetFontManager();
  IStyleManager &GetStyleManager();
  ILayoutManager &GetLayoutManager();
  IFontRes &GetFontRes();
  IColorRes &GetColorRes();
  IStyleRes &GetStyleRes();
  II18nRes &GetI18nRes();

  IBundleSource *GetSource();

  unsigned int GetXmlDocCount();
  IUIDocument *GetXmlDoc(unsigned int nIndex);
  IUIDocument *GetXmlDocByName(const char *szName);

  const char *GetName();
  const char *GetPath();

  bool Load();
  bool ChangeSkinHLS(short h, short l, short s, int nFlag);

private:
  ResourceBundle *m_pImpl;
};

struct IBundleSource;
class ResourceManager;
struct UIAPI IResourceManager : public IRootInterface {
  IResourceManager(ResourceManager *);
  ResourceManager *GetImpl();

  IApplication *GetUIApplication();
  void ChangeSkinHLS(short h, short l, short s, int nFlag);
  bool Save(IResourceBundle *resource_bundle);

  unsigned int GetResourceCount();
  IResourceBundle *GetResourceByIndex(unsigned int i);
  IResourceBundle *GetResourceByName(const char *szName);

  void SetCurrentLanguage(const char *);
  const char *GetCurrentLanguage();

private:
  ResourceManager *m_pImpl;
};
} // namespace ui
#endif