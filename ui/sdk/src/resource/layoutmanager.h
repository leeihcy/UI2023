#pragma once
#include "src/skin_parse/xml/xmlwrap.h"
#include "src/util/DataStruct/list.h"
#include <map>
#include <vector>

namespace ui {
class Object;
struct UIElement;
class Resource;
struct ILayoutWindowNodeList;
struct ILayoutManager;
struct IMessage;
struct IResource;

//  延迟加载的布局配置
//  一个xml配置中，允许配置多个相关联的窗口、自定义列表项
class LayoutConfigItem {
public:
  void SetPath(const wchar_t *szPath);
  const wchar_t *GetPath();

  void AddWindow(const wchar_t *szName);
  bool FindWindow(const wchar_t *szName);

  void AddListItem(const wchar_t *szName);
  bool FindListItem(const wchar_t *szName);

  bool Find(const wchar_t *szTagName, const wchar_t *szName);

  unsigned int GetWindowCount();
  const wchar_t *GetWindowName(unsigned int index);

private:
  std::wstring m_strPath;
  std::vector<std::wstring> m_vecWindow;
  std::vector<std::wstring> m_vecListItem;
};

// 新加载的子控件的notify设置为谁
#define NOTIFY_TARGET_ROOT (IMessage *)1 // 窗口（根对象）
#define NOTIFY_TARGET_NULL (IMessage *)0 // 不指定

class LayoutManager {
public:
  LayoutManager(Resource *);
  ~LayoutManager(void);

  ILayoutManager &GetILayoutManager();

  Object *LoadPluginLayout(const wchar_t *szId, Object *pNewParemt,
                           IMessage *pNotifyTarget = NOTIFY_TARGET_ROOT);

  UIElementProxy FindWindowElement(const wchar_t *szTagName,
                                   const wchar_t *szId);
  UIElementProxy FindListItemElement(const wchar_t *szId);

  enum PARSEFLAG {
    PARSEFLAG_LOADCHILD = 1,
    PARSEFLAG_RELOAD = 2,
  };
  Object *ParseElement(UIElement *pUIElement, Object *pParent,
                       IMessage *pNotifyTarget = NOTIFY_TARGET_ROOT,
                       int flags = PARSEFLAG_LOADCHILD);
  Object *ParseChildElement(UIElement *pParentElement, Object *pParent,
                            IMessage *pNotifyTarget = NOTIFY_TARGET_ROOT,
                            int flags = PARSEFLAG_LOADCHILD);

  bool ReLoadLayout(Object *pRootObj, const wchar_t *szNewLayoutId,
                    std::map<std::wstring, Object *> &listAllChild);
  void ReloadChildObjects(Object *pObjParent, UIElement *pObjElement,
                          std::map<std::wstring, Object *> &listAllChild);

  static int UIParseLayoutTagCallback(IUIElement *, IResource *pSkinRes);
  static int UIParseLayoutConfigTagCallback(IUIElement *, IResource *pSkinRes);

  // 编辑器专用函数
  bool LoadWindowNodeList(ILayoutWindowNodeList **pp);

private:
  void ParseNewElement(UIElement *pElem);
  void ParseLayoutConfigTag(UIElement *pElem);
  bool ParseLayoutConfigFileTag(UIElement *pElem);

  UIElementProxy find_element_from_cache(const wchar_t *szTagName,
                                         const wchar_t *szId);
  UIElementProxy load_window_by_id(const wchar_t *szTagName,
                                   const wchar_t *szId);
  UIElementProxy load_element_from_layout_config(const wchar_t *szTagName,
                                                 const wchar_t *szId);
  bool testUIElement(UIElement *pParentElem, const wchar_t *szTagName,
                     const wchar_t *szId);

private:
  ILayoutManager *m_pILayoutManager;
  UIList<UIElement *> m_listUIElement; // 已加载的布局项<layout>结点列表
  UIList<LayoutConfigItem *> m_listLayoutConfig; // 还未加载的布局项

  // 对象属性
  Resource *m_pSkinRes;
};

} // namespace ui