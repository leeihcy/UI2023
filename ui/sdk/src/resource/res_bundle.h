#ifndef _UI_SDK_SRC_RESOURCE_RESBUNDLE_H_
#define _UI_SDK_SRC_RESOURCE_RESBUNDLE_H_

#include "colormanager.h"
#include "i18nmanager.h"
#include "imagemanager.h"
#include "include/interface/ibundlesource.h"
#include "layoutmanager.h"
#include "src/graphics/font/font.h"
#include "stylemanager.h"
#include <memory>

namespace ui {
class Application;
class Object;
class ResourceManager;
struct UIDocument;
struct BundleSource;
struct SKIN_HLS_INFO;
struct IResourceManager;

enum SKIN_RES_LOAD_STATE {
  SKIN_RES_LOAD_STATE_NONE,   // 仅new ResourceBundle
  SKIN_RES_LOAD_STATE_LOADED, // 已加载
};

//
//	一个资源包。
//  可以是目录，或者是一个压缩包，或者是代码控制的资源。
//
class ResourceBundle {
public:
  ResourceBundle(ResourceManager &o);
  ~ResourceBundle();

  bool Load();
  bool Unload();
  bool Save();
  const char *GetName();
  const char *GetPath();
  void SetName(const char *);
  void SetPath(const char *);
  void CreateBundleSource(eBundleFormat eType);
  bool ChangeSkinHLS(short h, short l, short s, int nFlag);

  IResourceBundle *GetIResource();
  Application *GetUIApplication();
  IResourceManager &GetIResourceManager();

  ResourceManager &GetSkinMgr() { return m_mgrSkinRef; } //  内部调用
  ImageManager &GetImageManager() { return m_mgrImage; }
  ColorManager &GetColorManager() { return m_mgrColor; }
  StyleManager &GetStyleManager() { return m_mgrStyle; }
  LayoutManager &GetLayoutManager() { return m_mgrLayout; }
  SKIN_HLS_INFO *GetHLSInfo() { return m_pHLSInfo; }
  BundleSource *GetSource() { return m_source.get(); }
  I18nManager &GetI18nManager() { return m_mgrI18n; }
  FontRes& GetFontRes() { return m_fontres; }

  ImageRes &GetImageRes();
  ColorRes &GetColorRes();
  // FontRes &GetFontRes();
  StyleRes &GetStyleRes();
  I18nRes &GetI18nRes();

  unsigned int GetXmlDocCount() { return (unsigned int)m_listDoc.size(); }
  UIDocument *GetXmlDocByName(const char *szName);
  UIDocument *GetXmlDoc(unsigned int nIndex);

  void OnNewUIDocument(UIDocument *pDoc);

  void SetParentSkinRes(ResourceBundle *);
  ResourceBundle *GetParentSkinRes();

private:
  IResourceBundle *m_pISkinRes;
  ResourceManager &m_mgrSkinRef;

  // 父一级的资源，本地找不着时，向上查找
  // 用于插件资源中引用父一级的资源
  ResourceBundle *m_pParentSkinRes;

  std::string m_strSkinResName;
  std::unique_ptr<BundleSource> m_source;

  ColorManager m_mgrColor;
  ImageManager m_mgrImage;
  FontRes m_fontres;

  StyleManager m_mgrStyle;
  LayoutManager m_mgrLayout;
  I18nManager m_mgrI18n;

  SKIN_RES_LOAD_STATE m_eLoadState;
  SKIN_HLS_INFO *m_pHLSInfo;

  typedef std::vector<UIDocument *> _DocList;
  _DocList m_listDoc; // 各个xml文档指针
};

} // namespace ui

#endif