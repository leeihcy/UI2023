#include "stylemanager.h"
#include "include/interface/imapattr.h"
#include "include/interface/iuires.h"
#include "include/interface/ixmlwrap.h"
#include "res_bundle.h"
#include "src/application/uiapplication.h"
#include "src/skin_parse/xml/xmlwrap.h"

namespace ui {
#define MAX_STRING_LEN 256

StyleManager::StyleManager(Resource *p) : m_StyleRes(p) {
  m_pIStyleManager = nullptr;
  m_pSkinRes = p;
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

void StyleManager::Clear() { m_StyleRes.Clear(); }

int StyleManager::GetStyleCount() { return m_StyleRes.GetCount(); }

StyleRes &StyleManager::GetStyleRes() { return m_StyleRes; }

//
// 废弃，只保留LoadStyle，精简对外接口
//
// 从xml中将一个结点的所有属性读取出来后，解析其styleclass属性
// bool  StyleManager::ParseStyle(IMapAttribute* pMapAttrib)
// {
//     if (!pMapAttrib)
//         return false;
//
//     // 避免pMapAttrib->GetAttr返回临时变量。
//     std::string strId, strStyleClass, strName;
//     {
//         const char*  szId = pMapAttrib->GetAttr(XML_ID, false);
//         if (szId)
//             strId = szId;
//
//         const char*  szStyleClass = pMapAttrib->GetAttr(XML_STYLECLASS,
//         false); if (szStyleClass)
//             strStyleClass = szStyleClass;
//
// 		const char*  szName = pMapAttrib->GetTag();
// 		if (szName)
// 			strName = szName;
//     }
//
//     return this->LoadStyle(
//                 strName.c_str(),
//                 strStyleClass.c_str(),
//                 strId.c_str(),
//                 pMapAttrib);
// }

// 将pListAttrib中已属于style的属性删除掉，用于保存到xml中
// bool  StyleManager::ReverseParseStyle(IListAttribute* pListAttrib)
// {
//     if (!pListAttrib)
//         return false;
//
//     std::string strId, strStyleClass, strName;  //
//     避免pMapAttrib->GetAttr返回临时变量。
//     {
//         const char*  szId = pListAttrib->GetAttr(XML_ID);
//         if (szId)
//             strId = szId;
//
//         const char*  szStyleClass = pListAttrib->GetAttr(XML_STYLECLASS);
//         if (szStyleClass)
//             strStyleClass = szStyleClass;
//
//         const char*  szName = pListAttrib->GetTag();
//         if (szName)
//             strName = szName;
//     }
//
//     return m_StyleRes.UnloadStyle(
//         strId.c_str(), strStyleClass.c_str(), strName.c_str(), pListAttrib);
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

int StyleManager::UIParseStyleTagCallback(IUIElement *pElem,
                                          IResource *pSkinRes) {
  IStyleManager &pStyleMgr = pSkinRes->GetStyleManager();
  return pStyleMgr.GetImpl()->ParseNewElement(pElem->GetImpl());
}

int StyleManager::ParseNewElement(UIElement *pElem) {
  if (!pElem)
    return 1;

  StyleTagElementInfo info;
  info.pXmlElement = pElem;
  info.pXmlElement->AddRef();

  std::string bstrId = pElem->GetAttrib(XML_ID);
  if (!bstrId.empty()) {
    info.strId = bstrId;
  }

  m_listUIElement.Add(info);

  // 遍历其子元素
  UIElementProxy childElement = pElem->FirstChild();
  while (childElement) {
    this->OnNewChild(childElement.get());
    childElement = childElement->NextElement();
  }

  // 在所有的ITEM加载完后，开始解析inherit关系
  // TODO:
  // 2014.4.16,在初始化中就解析继承列表，会造成运行时修改父样式属性，子样式无法更新吧。以后想想怎么修改

  int nSize = m_StyleRes.GetCount();
  for (int i = 0; i < nSize; i++) {
    StyleResItem *pItem = m_StyleRes.GetItem(i);
    tree<StyleResItem *> t;
    t.data = pItem;

    if (false == this->parse_inherit(&t, &m_StyleRes)) {
      UI_LOG_WARN(
          "CXmlStyleParse::load_from_file parse_inherit failed. style item=%s",
          pItem->GetId());
    }
  }

  return 0;
}

void StyleManager::OnNewChild(UIElement *pElem) {
  std::string bstrTagName = pElem->GetTagName();

  //	加载所有属性
  std::shared_ptr<IMapAttribute> pMapAttrib = UICreateIMapAttribute();
  pElem->GetAttribList(pMapAttrib.get());

  StyleResItem *pStyleItem = new StyleResItem;
  pStyleItem->SetAttributeMap(pMapAttrib);
  pStyleItem->SetXmlElement(pElem);

  do {
    // 解析  SELECTOR_TPYE, ID
    if (bstrTagName == XML_STYLE_SELECTOR_TYPE_ID) {
      pStyleItem->SetSelectorType(STYLE_SELECTOR_TYPE_ID);

      std::string strID;
      const char *szText = pMapAttrib->GetAttr(XML_ID, true);
      if (nullptr == szText) {
        UI_LOG_WARN(L"Can't find the %s attribute of %s", XML_ID,
                    bstrTagName.c_str());
        delete pStyleItem;
        break;
      }
      strID = szText;
      pStyleItem->SetId(strID.c_str());
    } else if (bstrTagName == XML_STYLE_SELECTOR_TYPE_CLASS) {
      pStyleItem->SetSelectorType(STYLE_SELECTOR_TYPE_CLASS);

      std::string strID;
      const char *szText = pMapAttrib->GetAttr(XML_ID, true);
      if (nullptr == szText) {
        UI_LOG_WARN(L"Can't find the %s attribute of %s", XML_ID,
                    bstrTagName.c_str());
        delete pStyleItem;
        break;
      }
      strID = szText;
      pStyleItem->SetId(strID.c_str());
    } else {
      pStyleItem->SetSelectorType(STYLE_SELECTOR_TYPE_TAG);
      pStyleItem->SetId(bstrTagName.c_str());
    }

    // 解析 inherit 属性
    const char *szText = pMapAttrib->GetAttr(
        XML_STYLE_INHERIT, true); // 不是一个属性，它是需要被扩展的，删除它
    if (szText)
      pStyleItem->SetInherits(szText);

    if (false == m_StyleRes.Add(pStyleItem)) {
      delete pStyleItem;
      pStyleItem = nullptr;
    }
  } while (0);
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
  IUIEditor *pUIEditor = m_pSkinRes->GetUIApplication()->GetUIEditorPtr();
  if (pUIEditor) {
    pUIEditor->OnStyleChanged(m_pSkinRes->GetIResource(), p->GetIStyleResItem(),
                              Editor_Add);
  }
}
void StyleManager::OnStyleRemove(StyleResItem *p) {
  IUIEditor *pUIEditor = m_pSkinRes->GetUIApplication()->GetUIEditorPtr();
  if (pUIEditor) {
    pUIEditor->OnStyleChanged(m_pSkinRes->GetIResource(), p->GetIStyleResItem(),
                              Editor_Remove);
  }
}
void StyleManager::OnStlyeModify(StyleResItem *p) {
  IUIEditor *pUIEditor = m_pSkinRes->GetUIApplication()->GetUIEditorPtr();
  if (pUIEditor) {
    pUIEditor->OnStyleChanged(m_pSkinRes->GetIResource(), p->GetIStyleResItem(),
                              Editor_Modify);
  }
}
void StyleManager::OnStyleAttributeAdd(StyleResItem *p, const char *szKey) {
  IUIEditor *pUIEditor = m_pSkinRes->GetUIApplication()->GetUIEditorPtr();
  if (pUIEditor) {
    pUIEditor->OnStyleAttributeChanged(
        m_pSkinRes->GetIResource(), p->GetIStyleResItem(), szKey, Editor_Add);
  }
}
void StyleManager::OnStyleAttributeRemove(StyleResItem *p, const char *szKey) {
  IUIEditor *pUIEditor = m_pSkinRes->GetUIApplication()->GetUIEditorPtr();
  if (pUIEditor) {
    pUIEditor->OnStyleAttributeChanged(m_pSkinRes->GetIResource(),
                                       p->GetIStyleResItem(), szKey,
                                       Editor_Remove);
  }
}
void StyleManager::OnStyleAttributeModify(StyleResItem *p, const char *szKey) {
  IUIEditor *pUIEditor = m_pSkinRes->GetUIApplication()->GetUIEditorPtr();
  if (pUIEditor) {
    pUIEditor->OnStyleAttributeChanged(m_pSkinRes->GetIResource(),
                                       p->GetIStyleResItem(), szKey,
                                       Editor_Modify);
  }
}
} // namespace ui