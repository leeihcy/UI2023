#include "styleres.h"
#include "include/interface/imapattr.h"
#include "include/interface/iuires.h"
#include "include/interface/ixmlwrap.h"
#include "src/resource/res_bundle.h"

namespace ui {

StyleResItem::StyleResItem() {
  m_pIStyleResItem = nullptr;
  m_pMapAttrib = nullptr;
  m_pXmlElement = nullptr;
  m_eSelectorType = STYLE_SELECTOR_TYPE_TAG;
}
StyleResItem::~StyleResItem() {
  if (m_pMapAttrib) {
    m_pMapAttrib->Delete();
    m_pMapAttrib = nullptr;
  }
  SAFE_DELETE(m_pIStyleResItem);
  SAFE_RELEASE(m_pXmlElement);
}

IStyleResItem *StyleResItem::GetIStyleResItem() {
  if (nullptr == m_pIStyleResItem)
    m_pIStyleResItem = new IStyleResItem(this);

  return m_pIStyleResItem;
}

void StyleResItem::SetXmlElement(UIElement *p) {
  SAFE_RELEASE(m_pXmlElement);
  m_pXmlElement = p;
  if (m_pXmlElement)
    m_pXmlElement->AddRef();
}
UIElement *StyleResItem::GetXmlElement() { return m_pXmlElement; }
bool StyleResItem::GetXmlElement2(UIElement **pp) {
  if (!pp || !m_pXmlElement)
    return false;

  *pp = m_pXmlElement;
  m_pXmlElement->AddRef();
  return true;
}

void StyleResItem::SetInherits(const wchar_t *sz) {
  SETSTRING(m_strInherits, sz);
  m_vInheritList.clear();

  std::vector<String> vStrArray;
  UI_Split(m_strInherits, XML_MULTI_SEPARATOR, vStrArray);
  int nSize = (int)vStrArray.size();
  if (0 != nSize) {
    for (int i = 0; i < nSize; i++) {
      m_vInheritList.push_back(vStrArray[i]);
    }
  }
}
int StyleResItem::GetInheritCount() { return (int)m_vInheritList.size(); }

bool StyleResItem::GetInheritItem(int i, String &str) {
  if (i < 0 || i >= GetInheritCount())
    return false;

  str = m_vInheritList[i];
  return true;
}

//
// 仅从m_vInheritList中删除，不从m_strInherit中删除
//
bool StyleResItem::RemoveInheritItem(const wchar_t *sz) {
  if (!sz)
    return false;

  std::vector<String>::iterator iter = m_vInheritList.begin();
  for (; iter != m_vInheritList.end(); ++iter) {
    if (wcscmp(iter->c_str(), sz) == 0) {
      m_vInheritList.erase(iter);
      return true;
    }
  }

  return false;
}

void StyleResItem::SetAttributeMap(IMapAttribute *pMapAttrib) {
  if (m_pMapAttrib) {
    m_pMapAttrib->Delete();
    m_pMapAttrib = nullptr;
  }
  m_pMapAttrib = pMapAttrib;
}

IMapAttribute* StyleResItem::GetAttributeMap() {
  return m_pMapAttrib;
}

void StyleResItem::SetAttribute(const wchar_t *key, const wchar_t *value) {
  if (!m_pMapAttrib) {
   m_pMapAttrib = UICreateIMapAttribute();
  }

  m_pMapAttrib->AddAttr(key, value);
}

const wchar_t *StyleResItem::GetAttribute(const wchar_t *key) {
  if (!key || !m_pMapAttrib)
    return nullptr;

  if (0 == wcscmp(key, XML_ID))
    return m_strID.c_str();

  return m_pMapAttrib->GetAttr(key, false);
}

bool StyleResItem::ModifyAttribute(const wchar_t *key, const wchar_t *value) {
  if (!key || !m_pMapAttrib)
    return false;

  if (0 == wcscmp(key, XML_ID)) {
    SetId(value);
    return true;
  }

  if (!m_pMapAttrib->HasAttrib(key))
    return false;

  m_pMapAttrib->AddAttr(key, value);
  return true;
}

bool StyleResItem::RemoveAttribute(const wchar_t *key) {
  if (!m_pMapAttrib)
    return false;

  if (m_pMapAttrib->GetAttr(key, true))
    return true;

  return false;
}

bool StyleResItem::InheritAttribute(const wchar_t *key, const wchar_t *value) {
  // 继承一个属性值。如果自己已经定义了这个属性，则使用自身的值
  if (nullptr == m_pMapAttrib->GetAttr(key, false)) {
    m_pMapAttrib->AddAttr(key, value);
    return true;
  }

  return false;
}

//
// 将自己的属性继承给另一个item
//
//	如果pChild已经有这个属性了，则使用pChild的属性
//
bool StyleResItem::InheritMyAttributesToAnother(StyleResItem *pChild) {
  if (nullptr == pChild)
    return false;

  IMapAttribute *pMapAttrib = pChild->GetAttributeMap();
  if (pMapAttrib) {
    m_pMapAttrib->CopyTo(pMapAttrib, false);
  }

  return true;
}

//
//	将自己的属性应用到一个已有的map中，如果map中已有这个属性，则使用该map的属性
//
bool StyleResItem::Apply(IMapAttribute *pMapAttrib, bool bOverwrite) {
  if (!pMapAttrib || !m_pMapAttrib)
    return false;

  m_pMapAttrib->CopyTo(pMapAttrib, bOverwrite);
  return true;
}

//////////////////////////////////////////////////////////////////////////
//
//   StyleRes
//

StyleRes::StyleRes(ResBundle *p) {
  m_pIStyleRes = nullptr;
  m_pSkinRes = p;
}

StyleRes::~StyleRes() {
  this->Clear();
  SAFE_DELETE(m_pIStyleRes);
}
IStyleRes &StyleRes::GetIStyleRes() {
  if (!m_pIStyleRes)
    m_pIStyleRes = new IStyleRes(this);

  return *m_pIStyleRes;
}

void StyleRes::Clear() {
  std::vector<StyleResItem *>::iterator iter = m_vStyles.begin();
  for (; iter != m_vStyles.end(); ++iter) {
    StyleResItem *p = *iter;
    delete p;
    p = nullptr;
  }

  m_vStyles.clear();
}

StyleResItem *StyleRes::Add(STYLE_SELECTOR_TYPE type, const wchar_t *szId,
                            const wchar_t *szInherit) {
  StyleResItem *pStyleItem = new StyleResItem;
  pStyleItem->SetSelectorType(type);
  pStyleItem->SetId(szId);
  pStyleItem->SetInherits(szInherit);

  bool bRet = this->Add(pStyleItem);
  if (false == bRet) {
    delete pStyleItem;
    pStyleItem = nullptr;
    return nullptr;
  }

  return pStyleItem;
}

bool StyleRes::Add(StyleResItem *pNewItem) {
  if (nullptr == pNewItem)
    return false;

  StyleResItem *p =
      this->GetItem(pNewItem->GetSelectorType(), pNewItem->GetId());
  if (p) {
    UI_LOG_WARN(_T("StyleRes::Add failed, insert item=%s"), pNewItem->GetId());
    return false;
  }

  this->m_vStyles.push_back(pNewItem);
  m_pSkinRes->GetStyleManager().OnStyleAdd(pNewItem);
  return true;
}

StyleResItem *StyleRes::Insert(STYLE_SELECTOR_TYPE type, const wchar_t *szId,
                               long lPos) {
  if (!szId)
    return nullptr;

  if (this->GetItem(type, szId)) {
    UI_LOG_WARN(TEXT("StyleRes::Insert type=%d, id=%s Failed."), type, szId);
    return nullptr;
  }

  StyleResItem *pStyleItem = new StyleResItem;
  pStyleItem->SetSelectorType(type);
  pStyleItem->SetId(szId);

  std::vector<StyleResItem *>::iterator iter = m_vStyles.begin();
  iter += lPos;
  if (iter == m_vStyles.end())
    m_vStyles.push_back(pStyleItem);
  else
    m_vStyles.insert(iter, pStyleItem);

  m_pSkinRes->GetStyleManager().OnStyleAdd(pStyleItem);
  return pStyleItem;
}

bool StyleRes::Modify(StyleResItem *pItem, STYLE_SELECTOR_TYPE type,
                      const wchar_t *szId, const wchar_t *szInherit) {
  if (!pItem)
    return false;

  pItem->SetId(szId);
  pItem->SetSelectorType(type);
  pItem->SetInherits(szInherit);

  m_pSkinRes->GetStyleManager().OnStlyeModify(pItem);
  return true;
}

bool StyleRes::Remove(STYLE_SELECTOR_TYPE type, const wchar_t *szId) {
  std::vector<StyleResItem *>::iterator iter = m_vStyles.begin();
  for (; iter != m_vStyles.end(); ++iter) {
    StyleResItem *p = *iter;
    if (p->GetSelectorType() == type && 0 == wcscmp(p->GetId(), szId)) {
      m_pSkinRes->GetStyleManager().OnStyleRemove(p);
      delete p;
      p = nullptr;
      m_vStyles.erase(iter);
      return true;
    }
  }
  return false;
}

bool StyleRes::Remove(StyleResItem *pNewItem) {
  std::vector<StyleResItem *>::iterator iter =
      std::find(m_vStyles.begin(), m_vStyles.end(), pNewItem);
  if (iter == m_vStyles.end())
    return false;

  m_pSkinRes->GetStyleManager().OnStyleRemove(*iter);
  m_vStyles.erase(iter);
  delete pNewItem;
  return true;
}

bool StyleRes::AddAttribute(StyleResItem *pItem, const wchar_t *szKey,
                            const wchar_t *szValue) {
  if (!pItem)
    return false;

  pItem->SetAttribute(szKey, szValue);

  m_pSkinRes->GetStyleManager().OnStyleAttributeAdd(pItem, szKey);
  return true;
}

bool StyleRes::ModifyAttribute(StyleResItem *pItem, const wchar_t *szKey,
                               const wchar_t *szValue) {
  if (!pItem)
    return false;

  pItem->ModifyAttribute(szKey, szValue);

  m_pSkinRes->GetStyleManager().OnStyleAttributeModify(pItem, szKey);
  return true;
}

bool StyleRes::RemoveAttribute(StyleResItem *pItem, const wchar_t *szKey) {
  if (!pItem)
    return false;

  pItem->RemoveAttribute(szKey);

  m_pSkinRes->GetStyleManager().OnStyleAttributeRemove(pItem, szKey);
  return true;
}

long StyleRes::GetCount() { return (long)m_vStyles.size(); }

StyleResItem *StyleRes::GetItem(long nIndex) {
  if (nIndex < 0)
    return nullptr;
  if (nIndex >= (int)m_vStyles.size())
    return nullptr;

  return m_vStyles[nIndex];
}

StyleResItem *StyleRes::GetItem(STYLE_SELECTOR_TYPE type, const wchar_t *szId) {
  if (nullptr == szId)
    return nullptr;

  std::vector<StyleResItem *>::iterator iter = m_vStyles.begin();
  for (; iter != m_vStyles.end(); ++iter) {
    StyleResItem *p = *iter;
    if (p->GetSelectorType() == type && 0 == wcscmp(p->GetId(), szId)) {
      return p;
    }
  }

  ResBundle *pParentRes = m_pSkinRes->GetParentSkinRes();
  if (pParentRes) {
    return pParentRes->GetStyleRes().GetItem(type, szId);
  }

  return nullptr;
}

// 用于删除时保存当前位置
long StyleRes::GetItemPos(StyleResItem *p) {
  if (!p)
    return -1;

  std::vector<StyleResItem *>::iterator iter =
      std::find(m_vStyles.begin(), m_vStyles.end(), p);
  if (iter == m_vStyles.end())
    return -1;

  return (long)(iter - m_vStyles.begin());
}

// bool StyleRes::AddStyleAttribute(STYLE_SELECTOR_TYPE type, const wchar_t*
// szId, const wchar_t* szKey, const wchar_t* szValue)
// {
// 	StyleResItem* pItem = this->GetItem(type, szId);
// 	if (false == pItem)
// 		return false;
//
// 	return pItem->InsertAttribute(szKey, szValue);
// }
//
// bool StyleRes::ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const wchar_t*
// szId, const wchar_t* szKey, const wchar_t* szValue)
// {
// 	StyleResItem* pItem = this->GetItem(type, szId);
// 	if (false == pItem)
// 		return false;
//
// 	return pItem->ModifyAttribute(szKey, szValue);
// }
//
// bool StyleRes::RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const wchar_t*
// szId, const wchar_t* szKey)
// {
// 	StyleResItem* pItem = this->GetItem(type, szId);
// 	if (false == pItem)
// 		return false;
//
// 	return pItem->RemoveAttribute(szKey);
// }

//
//	返回匹配的样式
//
//	Parameter
//		strTagName
//			[in]	要匹配的标签名
//		strStyleClass
//			[in]	要匹配的class样式
//		strID
//			[in]	要匹配的id样式
//
//		mapStyle
//			[in/out] 将结果返回到该map中
//
//	Remark
//		优化级定义：
//			级别1. 在layout.xml中直接指定的属性
//			级别2. 通过ID匹配的属性
//			级别3. 通过CLASS匹配的属性
//			级别4. CLASS列表中，排后面的样式匹配的属性 >
//前面的样式匹配的属性 			级别5. 通过TAG匹配的属性
//
bool StyleRes::LoadStyle(const wchar_t *szTagName, const wchar_t *szStyleClass,
                         const wchar_t *szID, IMapAttribute *pMapStyle) {
  if (szID && wcslen(szID) > 0) {
    StyleResItem *pIDStyleItem = this->GetItem(STYLE_SELECTOR_TYPE_ID, szID);
    if (pIDStyleItem) {
      pIDStyleItem->Apply(pMapStyle, false);
    }
  }

  // strStyleClass可能有多个
  if (szStyleClass && wcslen(szStyleClass) > 0) {
    std::vector<String> vStrArray;
    UI_Split(szStyleClass, XML_MULTI_SEPARATOR, vStrArray);
    int nCount = (int)vStrArray.size();
    if (0 != nCount) {
      for (int i = 0; i < nCount; i++) {
        if (vStrArray[i].empty())
          continue;

        StyleResItem *pClassStyleItem =
            this->GetItem(STYLE_SELECTOR_TYPE_CLASS, vStrArray[i].c_str());
        if (pClassStyleItem) {
          pClassStyleItem->Apply(pMapStyle, false);
        }
      }
    }
  }

  if (szTagName && wcslen(szTagName) > 0) {
    StyleResItem *pTagStyleItem =
        this->GetItem(STYLE_SELECTOR_TYPE_TAG, szTagName);
    if (pTagStyleItem) {
      pTagStyleItem->Apply(pMapStyle, false);
    }
  }

  return true;
}

// 将pListAttribte中属于style的属性过滤掉
bool StyleRes::UnloadStyle(const wchar_t *szTagName,
                           const wchar_t *szStyleClass, const wchar_t *szID,
                           IListAttribute *pListAttribte) {
  // 先拿到所有的样式列表
  IMapAttribute *pStyleAttr = UICreateIMapAttribute();
  LoadStyle(szTagName, szStyleClass, szID, pStyleAttr);

  pListAttribte->BeginEnum();

  const wchar_t *szKey = nullptr;
  const wchar_t *szValue = nullptr;
  while (pListAttribte->EnumNext(&szKey, &szValue)) {
    const wchar_t *szStyleValue = pStyleAttr->GetAttr(nullptr, szKey, true);
    if (!szStyleValue)
      continue;

    // 该属性已包含于style中，不需要配置，删掉
    if (0 == wcscmp(szStyleValue, szValue)) {
      pListAttribte->EraseAttr(szKey);
      continue;
    }
    // 该属性与style不一致，必须配置，以覆盖style
    else {
    }
  }
  pListAttribte->EndEnum();

  // 还剩下一个style没有被删干净，则需要额外添加一些空值给控件，避免使用style中的样式
  pStyleAttr->BeginEnum();
  while (pStyleAttr->EnumNext(&szKey, &szValue)) {
    pListAttribte->AddAttr(szKey, L"");
  }
  pStyleAttr->EndEnum();

  if (pStyleAttr) {
    pStyleAttr->Delete();
    pStyleAttr = nullptr;
  }
  return true;
}

} // namespace ui