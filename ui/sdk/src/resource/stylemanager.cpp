#include "stylemanager.h"
#include "include/interface/imapattr.h"
#include "include/interface/iuires.h"
#include "include/interface/ixmlwrap.h"
#include "res_bundle.h"
#include "src/application/uiapplication.h"
#include "src/parser/xml/xmlwrap.h"

namespace ui {
#define MAX_STRING_LEN 256

StyleManager::StyleManager(ResourceBundle& o) : m_style_res(o), m_resource_bundle(o) {
  m_pIStyleManager = nullptr;
}

StyleManager::~StyleManager(void) {
  this->Clear();
  SAFE_DELETE(m_pIStyleManager);

  _MyListItem *pItem = m_listUIElement.GetFirst();
  while (pItem) {
    (**pItem).pXmlElement->Release();
    pItem = pItem->GetNext();
  }
  m_listUIElement.Clear();
}

IStyleManager &StyleManager::GetIStyleManager() {
  if (!m_pIStyleManager)
    m_pIStyleManager = new IStyleManager(this);

  return *m_pIStyleManager;
}

void StyleManager::Clear() { m_style_res.Clear(); }

int StyleManager::GetStyleCount() { return m_style_res.GetCount(); }

StyleRes &StyleManager::GetStyleRes() { return m_style_res; }

//
// 废弃，只保留LoadStyle，精简对外接口
//
// 从xml中将一个结点的所有属性读取出来后，解析其styleclass属性
// bool  StyleManager::ParseStyle(IAttributeMap* attribute_map)
// {
//     if (!attribute_map)
//         return false;
//
//     // 避免pMapAttrib->GetAttr返回临时变量。
//     std::string strId, strStyleClass, strName;
//     {
//         const char*  szId = attribute_map->GetAttr(XML_ID, false);
//         if (szId)
//             strId = szId;
//
//         const char*  szStyleClass = attribute_map->GetAttr(XML_STYLECLASS,
//         false); if (szStyleClass)
//             strStyleClass = szStyleClass;
//
// 		const char*  szName = attribute_map->GetTag();
// 		if (szName)
// 			strName = szName;
//     }
//
//     return this->LoadStyle(
//                 strName.c_str(),
//                 strStyleClass.c_str(),
//                 strId.c_str(),
//                 attribute_map);
// }

// 将pListAttrib中已属于style的属性删除掉，用于保存到xml中
// bool  StyleManager::ReverseParseStyle(IAttributeList* attribute_list)
// {
//     if (!attribute_list)
//         return false;
//
//     std::string strId, strStyleClass, strName;  //
//     避免pMapAttrib->GetAttr返回临时变量。
//     {
//         const char*  szId = attribute_list->GetAttr(XML_ID);
//         if (szId)
//             strId = szId;
//
//         const char*  szStyleClass = attribute_list->GetAttr(XML_STYLECLASS);
//         if (szStyleClass)
//             strStyleClass = szStyleClass;
//
//         const char*  szName = attribute_list->GetTag();
//         if (szName)
//             strName = szName;
//     }
//
//     return m_style_res.UnloadStyle(
//         strId.c_str(), strStyleClass.c_str(), strName.c_str(), attribute_list);
// }

//
// 解析一个继承字符串所属的样式类型，如将#button解析为 id选择类型，id=button
//
bool StyleManager::ParseInheritString(const std::string &strInherit,
                                      STYLE_SELECTOR_TYPE &eStyletype,
                                      char *szStyleName) {
  if (strInherit.length() <= 0 || nullptr == szStyleName)
    return false;

  if (strInherit[0] == STYLE_ID_PREFIX) {
    eStyletype = STYLE_SELECTOR_TYPE_ID;
    strncpy(szStyleName, strInherit.substr(1, strInherit.length() - 1).c_str(),
            MAX_STRING_LEN - 1);
  } else if (strInherit[0] == STYLE_CLASS_PREFIX) {
    eStyletype = STYLE_SELECTOR_TYPE_CLASS;
    strncpy(szStyleName, strInherit.substr(1, strInherit.length() - 1).c_str(),
            MAX_STRING_LEN - 1);
  } else {
    eStyletype = STYLE_SELECTOR_TYPE_TAG;
    strncpy(szStyleName, strInherit.c_str(), MAX_STRING_LEN - 1);
  }
  return true;
}
bool StyleManager::MakeInheritString(const STYLE_SELECTOR_TYPE &eStyletype,
                                     const std::string &strStypeName,
                                     char *szInherit) {
  if (strStypeName.length() <= 0 || nullptr == szInherit)
    return false;

  if (eStyletype == STYLE_SELECTOR_TYPE_ID) {
    strcpy(szInherit, " ");
    szInherit[0] = STYLE_ID_PREFIX;
    strcat(szInherit, strStypeName.c_str());
  } else if (eStyletype == STYLE_SELECTOR_TYPE_CLASS) {
    strcpy(szInherit, " ");
    szInherit[0] = STYLE_CLASS_PREFIX;
    strcat(szInherit, strStypeName.c_str());
  } else {
    strcpy(szInherit, strStypeName.c_str());
  }
  return true;
}

//
//	递归解析一个CPojo_StyleItem的继承数据
//
//	Parameter
//		pTreeItem
//			[in]
//要解析的CStyleItem，pTreeItem代表它的继承依赖树，是树的叶子结点。
//					如果自己重复出现在这个树中，表示出现了死锁继承，无法解析。
//
//		pStyleRes
//			[in]	当前所有的StyleItem列表
//
//	Return
//		解析成功返回TRUE，失败返回FALSE。只要树中有一个返回FALSE，这棵树全部返回FALSE。
//
bool StyleManager::parse_inherit(tree<StyleResItem *> *pTreeItem,
                                 StyleRes *pStyleRes) {
  if (nullptr == pTreeItem || nullptr == pTreeItem->data)
    return false;

  StyleResItem *pStyleItem = pTreeItem->data;

  // 判断pTreeItem是否有继承关系
  int nSize = pStyleItem->GetInheritCount();
  if (0 == nSize)
    return true;

  // 判断当前这个pTreeItem中整棵树中是不是重复出现了，如果重复出现了则表示死循环了
  {
    tree<StyleResItem *> *pParentItem = pTreeItem;
    do {
      pParentItem = pParentItem->parent;
      if (pParentItem && pParentItem->data == pStyleItem)
        return false;
    } while (pParentItem != nullptr);
  }

  for (int i = nSize - 1; i >= 0; i--) {
    std::string strInherit;
    if (false == pStyleItem->GetInheritItem(i, strInherit))
      continue;

    STYLE_SELECTOR_TYPE type = STYLE_SELECTOR_TYPE_TAG;
    char szStyleName[MAX_STRING_LEN] = "";
    ParseInheritString(strInherit, type, szStyleName);

    StyleResItem *pInheritItem = pStyleRes->GetItem(type, szStyleName);
    if (nullptr == pInheritItem) {
      UI_LOG_WARN("get parent style item failed. type=%d, name=%s", type,
                  szStyleName);
      continue;
    }

    tree<StyleResItem *> t;
    t.parent = pTreeItem;
    t.data = pInheritItem;

    if (false == this->parse_inherit(&t, pStyleRes)) {
      return false;
    }

    // 解析成功，继承它的属性
    pInheritItem->InheritMyAttributesToAnother(pStyleItem);
    pStyleItem->RemoveInheritItem(strInherit.c_str());
  }

  return true;
}

int StyleManager::UIParseStyleTagCallback(IUIElement *element,
                                          IResourceBundle *resource_bundle) {
  IStyleManager &pStyleMgr = resource_bundle->GetStyleManager();
  return pStyleMgr.GetImpl()->ParseNewElement(element->GetImpl());
}

int StyleManager::ParseNewElement(UIElement *element) {
  if (!element)
    return 1;

  StyleTagElementInfo info;
  info.pXmlElement = element;
  info.pXmlElement->AddRef();

  std::string bstrId = element->GetAttrib(XML_ID);
  if (!bstrId.empty()) {
    info.strId = bstrId;
  }

  m_listUIElement.Add(info);

  // 遍历其子元素
  UIElementProxy childElement = element->FirstChild();
  while (childElement) {
    this->onNewChild(childElement.get());
    childElement = childElement->NextElement();
  }

  // 在所有的ITEM加载完后，开始解析inherit关系
  // TODO:
  // 2014.4.16,在初始化中就解析继承列表，会造成运行时修改父样式属性，子样式无法更新吧。以后想想怎么修改

  int nSize = m_style_res.GetCount();
  for (int i = 0; i < nSize; i++) {
    StyleResItem *pItem = m_style_res.GetItem(i);
    tree<StyleResItem *> t;
    t.data = pItem;

    if (false == this->parse_inherit(&t, &m_style_res)) {
      UI_LOG_WARN(
          "CXmlStyleParse::load_from_file parse_inherit failed. style item=%s",
          pItem->GetId());
    }
  }

  return 0;
}

void StyleManager::onNewChild(UIElement *element) {
  std::string tag_name = element->GetTagName();
  if (tag_name == XML_STYLE_FONTFACE) {
    onNewFontFace(element);
    return;
  }

  //	加载所有属性
  std::shared_ptr<IAttributeMap> attribute_map = UICreateIMapAttribute();
  element->GetAttribList(attribute_map.get());

  StyleResItem *pStyleItem = new StyleResItem;
  pStyleItem->SetAttributeMap(attribute_map);
  pStyleItem->SetXmlElement(element);

  do {
    // 解析  SELECTOR_TPYE, ID
    if (tag_name == XML_STYLE_SELECTOR_TYPE_ID) {
      pStyleItem->SetSelectorType(STYLE_SELECTOR_TYPE_ID);

      std::string strID;
      const char *szText = attribute_map->GetAttr(XML_ID, true);
      if (nullptr == szText) {
        UI_LOG_WARN(L"Can't find the %s attribute of %s", XML_ID,
                    tag_name.c_str());
        delete pStyleItem;
        break;
      }
      strID = szText;
      pStyleItem->SetId(strID.c_str());
    } else if (tag_name == XML_STYLE_SELECTOR_TYPE_CLASS) {
      pStyleItem->SetSelectorType(STYLE_SELECTOR_TYPE_CLASS);

      std::string strID;
      const char *szText = attribute_map->GetAttr(XML_ID, true);
      if (nullptr == szText) {
        UI_LOG_WARN(L"Can't find the %s attribute of %s", XML_ID,
                    tag_name.c_str());
        delete pStyleItem;
        break;
      }
      strID = szText;
      pStyleItem->SetId(strID.c_str());
    } else {
      pStyleItem->SetSelectorType(STYLE_SELECTOR_TYPE_TAG);
      pStyleItem->SetId(tag_name.c_str());
    }

    // 解析 inherit 属性
    const char *szText = attribute_map->GetAttr(
        XML_STYLE_INHERIT, true); // 不是一个属性，它是需要被扩展的，删除它
    if (szText)
      pStyleItem->SetInherits(szText);

    if (false == m_style_res.Add(pStyleItem)) {
      delete pStyleItem;
      pStyleItem = nullptr;
    }
  } while (0);
}

void StyleManager::onNewFontFace(UIElement *element) {
  std::shared_ptr<IAttributeMap> attribute_map = UICreateIMapAttribute();
  element->GetAttribList(attribute_map.get());

  FontFace font_face;
  const char* text = attribute_map->GetAttr(XML_FONT_FAMILY, false);
  if (!text) {
    return;
  }
  font_face.font_family = text;

  text = attribute_map->GetAttr(XML_SRC, false);
  if (!text) {
    return;
  }
  font_face.src = text;

  m_resource_bundle.GetFontRes().AddFontFace(font_face);
}

UIElement *StyleManager::GetStyleXmlElem(const char *szId) {
  if (0 == m_listUIElement.GetCount())
    return nullptr;

  if (!szId) {
    return m_listUIElement.GetFirst()->m_data.pXmlElement;
  }

  _MyListItem *pItem = m_listUIElement.GetFirst();
  while (pItem) {
    if ((**pItem).strId == szId) {
      return (**pItem).pXmlElement;
    }
    pItem = pItem->GetNext();
  }
  return nullptr;
}

//////////////////////////////////////////////////////////////////////////
void StyleManager::OnStyleAdd(StyleResItem *p) {
  IUIEditor *pUIEditor = m_resource_bundle.GetUIApplication()->GetUIEditorPtr();
  if (pUIEditor) {
    pUIEditor->OnStyleChanged(m_resource_bundle.GetIResource(), p->GetIStyleResItem(),
                              Editor_Add);
  }
}
void StyleManager::OnStyleRemove(StyleResItem *p) {
  IUIEditor *pUIEditor = m_resource_bundle.GetUIApplication()->GetUIEditorPtr();
  if (pUIEditor) {
    pUIEditor->OnStyleChanged(m_resource_bundle.GetIResource(), p->GetIStyleResItem(),
                              Editor_Remove);
  }
}
void StyleManager::OnStlyeModify(StyleResItem *p) {
  IUIEditor *pUIEditor = m_resource_bundle.GetUIApplication()->GetUIEditorPtr();
  if (pUIEditor) {
    pUIEditor->OnStyleChanged(m_resource_bundle.GetIResource(), p->GetIStyleResItem(),
                              Editor_Modify);
  }
}
void StyleManager::OnStyleAttributeAdd(StyleResItem *p, const char *szKey) {
  IUIEditor *pUIEditor = m_resource_bundle.GetUIApplication()->GetUIEditorPtr();
  if (pUIEditor) {
    pUIEditor->OnStyleAttributeChanged(
        m_resource_bundle.GetIResource(), p->GetIStyleResItem(), szKey, Editor_Add);
  }
}
void StyleManager::OnStyleAttributeRemove(StyleResItem *p, const char *szKey) {
  IUIEditor *pUIEditor = m_resource_bundle.GetUIApplication()->GetUIEditorPtr();
  if (pUIEditor) {
    pUIEditor->OnStyleAttributeChanged(m_resource_bundle.GetIResource(),
                                       p->GetIStyleResItem(), szKey,
                                       Editor_Remove);
  }
}
void StyleManager::OnStyleAttributeModify(StyleResItem *p, const char *szKey) {
  IUIEditor *pUIEditor = m_resource_bundle.GetUIApplication()->GetUIEditorPtr();
  if (pUIEditor) {
    pUIEditor->OnStyleAttributeChanged(m_resource_bundle.GetIResource(),
                                       p->GetIStyleResItem(), szKey,
                                       Editor_Modify);
  }
}
} // namespace ui