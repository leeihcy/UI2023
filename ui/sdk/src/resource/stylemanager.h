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
  StyleManager(Resource *p);
  ~StyleManager(void);

  StyleRes &GetStyleRes();
  IStyleManager &GetIStyleManager();

  UIElement *GetStyleXmlElem(const char *szId);

  // bool  ParseStyle(IMapAttribute* pMapAttrib);
  // bool  ReverseParseStyle(IListAttribute* pListAttrib);

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

  static int UIParseStyleTagCallback(IUIElement *, IResource *pSkinRes);

private:
  int ParseNewElement(UIElement *pElem);
  void OnNewChild(UIElement *pElem);

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
  StyleRes m_StyleRes;
  Resource *m_pSkinRes;
};

} // namespace ui