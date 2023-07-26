#pragma once
#include "res_bundle.h"
#include <string>
#include <vector>

/*
 *  2015.7.19
 *
 *  1. 废弃了类似TTPlayer形式的多皮肤功能，同步维护所有的皮肤成本太大，
 *     不利于软件的更新，更好的方式是一套皮肤，多个配置文件。
 *
 *  2. 扩展支持插件模式的多皮肤包，每个插件允许的自己的皮肤包，包肤包也
 *     可以有继承关系。
 * 
 *  2023.7.26
 *  重命名
 *  SkinManager ==> ResourceManager
 *  SkinRes ==> ResBundle 
 * 
 */
namespace ui {
struct IResourceManager;
class UIApplication;
class IUISkinDescParse;

class ResourceManager {
public:
  ResourceManager();
  ~ResourceManager();
  void Destroy();

public:
  IResourceManager &GetIResourceManager();
  UIApplication *GetUIApplication();
  void SetUIApplication(UIApplication *pUIApp);

  ResBundle& RootBundle() { return m_root_bundle; }

  ResBundle *LoadResBundle(const wchar_t *szPath);
  ResBundle *LoadResBundle(long hInstance, int resId = -1);
  ResBundle *GetDefaultSkinRes();
  ResBundle *GetSkinResByName(const wchar_t *szName);
  unsigned int GetSkinResCount();
  ResBundle *GetSkinResByIndex(unsigned int i);

  bool Save(ResBundle *pSkinRes = nullptr);
  void ChangeSkinHLS(short h, short l, short s, int nFlag);

  const wchar_t *GetCurrentLanguage();
  void SetCurrentLanguage(const wchar_t *);

private:
  IResourceManager *m_pIResourceManager = nullptr;
  UIApplication *m_pUIApplication = nullptr;

  // 数据持久层
  bool m_bDirty = false;

  // 默认带一个资源包，作为其它资源包的根结点，
  // 在这个包中的资源，可以被所有其他包访问到。
  // 这个包只能通过接口的方式进行修改，没有落地文件。
  ResBundle m_root_bundle;

  // 动态加载的其他资源包
  std::vector<ResBundle *> m_resoures;

  // TODO: 运行过程中如何切换语言
  std::wstring m_strLanguage; // 当前使用的语言
};
} // namespace ui
