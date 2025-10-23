#pragma once
#include "src/util/DataStruct/list.h"
#include "src/util/DataStruct/tree.h"
#include "styleres.h"

namespace ui {
struct IStyleManager;
struct IUIElement;

class StyleTagElementInfo {
public:
  std::string strId;      // 模块名
  UIElement *pXmlElement; // xml结点
};

class StyleManager {
public:
  StyleManager(ResourceBundle&);
  ~StyleManager(void);

  StyleRes &GetStyleRes();
  IStyleManager &GetIStyleManager();

  UIElement *GetStyleXmlElem(const char *szId);

  // bool  ParseStyle(IAttributeMap* attribute_map);
  // bool  ReverseParseStyle(IAttributeList* attribute_list);

public:
  // Editor
  void OnStyleAdd(StyleResItem *p);
  void OnStyleRemove(StyleResItem *p);
  void OnStlyeModify(StyleResItem *p);
  void OnStyleAttributeAdd(StyleResItem *p, const char *szKey);
  void OnStyleAttributeRemove(StyleResItem *p, const char *szKey);
  void OnStyleAttributeModify(StyleResItem *p, const char *szKey);

public:
  void Clear();
  int GetStyleCount();
  //  bool     GetStyleItemInfo( int nIndex, IStyleResItem** ppStyleItemInfo );
  //  bool     GetStyleItemInfo( STYLE_SELECTOR_TYPE eType, const char*
  //  szSelectorID, IStyleResItem** ppStyleItemInfo );

  static int UIParseStyleTagCallback(IUIElement *, IResourceBundle *resource_bundle);

private:
  int ParseNewElement(UIElement *pElem);
  void onNewChild(UIElement *pElem);
  void onNewFontFace(UIElement *pElem);

  bool parse_inherit(tree<StyleResItem *> *pTreeItem, StyleRes *pStyleRes);
  bool MakeInheritString(const STYLE_SELECTOR_TYPE &eStyletype,
                         const std::string &strStypeName, char *szInherit);
  bool ParseInheritString(const std::string &strInherit,
                          STYLE_SELECTOR_TYPE &eStyletype, char *szStyleName);

private:
  // 数据持久层
  IStyleManager *m_pIStyleManager;

  typedef UIList<StyleTagElementInfo> _MyList;
  typedef UIListItem<StyleTagElementInfo> _MyListItem;
  _MyList m_listUIElement;

  // 对象属性
  StyleRes m_style_res;
  ResourceBundle& m_resource_bundle;
};

} // namespace ui