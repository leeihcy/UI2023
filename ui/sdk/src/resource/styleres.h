#pragma once
#include "include/inc.h"

namespace ui {
class Resource;
struct IStyleRes;

// 1. 控件设置一个class样式：
//    styleclass="imagebtn;aaa"
//
// 2. Tag 样式
//       <CustomWindow font="normal" transparentrgn.type="4corner"
//       transparentrgn.9region="5"/>
//
// 3. Id 样式
//       <id id="imagebtn" xxx="yyy"/>
//
// 4. class 样式
//       <class id="imagebtn" xxx="yyy"/>
//

struct IStyleResItem;
struct UIElement;

class StyleResItem {
public:
  StyleResItem();
  ~StyleResItem();
  IStyleResItem *GetIStyleResItem();

  const char *GetId() { return m_strID.c_str(); }
  void SetId(const char *szId) {
    if (szId)
      m_strID = szId;
    else
      m_strID.clear();
  }

  STYLE_SELECTOR_TYPE GetSelectorType() { return m_eSelectorType; }
  void SetSelectorType(STYLE_SELECTOR_TYPE type) { m_eSelectorType = type; }

  const char *GetInherits() { return m_strInherits.c_str(); }
  void SetInherits(const char *sz);
  int GetInheritCount();
  bool GetInheritItem(int i, std::string &str);
  bool RemoveInheritItem(const char *sz);

  void SetXmlElement(UIElement *p);
  UIElement *GetXmlElement();
  bool GetXmlElement2(UIElement **pp);

  void SetAttributeMap(IMapAttribute *pMapAttrib);
  IMapAttribute *GetAttributeMap();
  void SetAttribute(const char *key, const char *value);
  const char *GetAttribute(const char *key);
  bool ModifyAttribute(const char *key, const char *value);
  bool RemoveAttribute(const char *key);

  bool InheritAttribute(const char *key, const char *value);
  bool InheritMyAttributesToAnother(StyleResItem *pChild);
  bool Apply(IMapAttribute *pMapAttrib, bool bOverwrite);

private:
  IStyleResItem *m_pIStyleResItem;

  STYLE_SELECTOR_TYPE m_eSelectorType;
  std::string m_strID;              // 该选择器的ID
  std::string m_strInherits;        // 继承列表
  IMapAttribute *m_pMapAttrib; // 该选择器的属性集

  // 非持久数据
  std::vector<std::string> m_vInheritList; // 继承列表，运行中解释m_strInherits

  // editor数据
  UIElement *m_pXmlElement;
};

class StyleRes {
public:
  StyleRes(Resource *p);
  ~StyleRes();
  IStyleRes &GetIStyleRes();
  void Clear();

public:
  StyleResItem *Add(STYLE_SELECTOR_TYPE type, const char *szId,
                    const char *szInherit);
  StyleResItem *Insert(STYLE_SELECTOR_TYPE type, const char *szId,
                       long lPos);
  bool Modify(StyleResItem *pItem, STYLE_SELECTOR_TYPE type,
              const char *szId, const char *szInherit);
  bool Remove(STYLE_SELECTOR_TYPE type, const char *szId);

  bool AddAttribute(StyleResItem *pItem, const char *szKey,
                    const char *szValue);
  bool ModifyAttribute(StyleResItem *pItem, const char *szKey,
                       const char *szValue);
  bool RemoveAttribute(StyleResItem *pItem, const char *szKey);

  bool Add(StyleResItem *pNewItem);
  bool Remove(StyleResItem *pNewItem);

  long GetCount();
  StyleResItem *GetItem(long nIndex);
  long GetItemPos(StyleResItem *p);
  StyleResItem *GetItem(STYLE_SELECTOR_TYPE type, const char *szID);

  bool LoadStyle(const char *szTagName, const char *szStyleClass,
                 const char *szID, IMapAttribute *pMapAttribute);
  bool UnloadStyle(const char *szTagName, const char *szStyleClass,
                   const char *szID, IListAttribute *pListAttribte);

private:
  IStyleRes *m_pIStyleRes;

  Resource *m_pSkinRes;
  std::vector<StyleResItem *> m_vStyles;
};
} // namespace ui