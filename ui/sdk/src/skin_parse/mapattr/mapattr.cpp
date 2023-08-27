#include "mapattr.h"
#include "include/inc.h"
#include "include/interface/iuiapplication.h"
#include "include/interface/iuires.h"
#include "src/application/uiapplication.h"
#include "src/render/renderbase.h"
#include "src/render/textrender/textrender.h"

namespace ui {

CMapAttribute::CMapAttribute() { m_iterEnum = m_mapAttr.end(); }

int CMapAttribute::GetAttrCount() { return (int)m_mapAttr.size(); }

CMapAttribute::~CMapAttribute() {}
CMapAttribute::CMapAttribute(const CMapAttribute &o) {
  m_mapAttr = o.m_mapAttr;
}

// void  CMapAttribute::SetTag(const char* szKey)
// {
// 	if (szKey)
// 		m_strTag = szKey;
// 	else
// 		m_strTag.clear();
// }
// const char*  CMapAttribute::GetTag()
// {
// 	return m_strTag.c_str();
// }
bool CMapAttribute::HasAttrib(const char *szKey) {
  if (nullptr == szKey)
    return false;

  ATTRMAP::iterator iter = m_mapAttr.find(szKey);
  if (iter == m_mapAttr.end())
    return false;

  return true;
}
const char *CMapAttribute::GetAttr(const char *szKey, bool bErase) {
  if (nullptr == szKey)
    return nullptr;

  ATTRMAP::iterator iter = m_mapAttr.find(szKey);
  if (iter == m_mapAttr.end())
    return nullptr;

  if (bErase) {
    std::string &str = GetTempBufferString();
    str = iter->second;
    m_mapAttr.erase(iter);
    return str.c_str();
  }
  return iter->second.c_str();
}

const char *CMapAttribute::GetAttr(const std::string &strKey, bool bErase) {
  ATTRMAP::iterator iter = m_mapAttr.find(strKey);
  if (iter == m_mapAttr.end())
    return nullptr;

  if (bErase) {
    std::string &strTemp = GetTempBufferString();
    strTemp = iter->second;
    m_mapAttr.erase(iter);
    return strTemp.c_str();
  }
  return iter->second.c_str();
}
const char *CMapAttribute::GetAttr(const char *szPrefix,
                                      const char *szKey, bool bErase) {
  std::string strKey;
  if (szPrefix)
    strKey = szPrefix;
  strKey.append(szKey);

  return GetAttr(strKey.c_str(), bErase);
}

int CMapAttribute::GetAttr_bool(const char *szPrefix, const char *szKey,
                                 bool bErase, bool *pbGet) {
  std::string strKey;
  if (szPrefix)
    strKey = szPrefix;
  strKey.append(szKey);

  return GetAttr_bool(strKey.c_str(), bErase, pbGet);
}
int CMapAttribute::GetAttr_bool(const char *szKey, bool bErase,
                                 bool *pbGet) {
  if (nullptr == szKey || nullptr == pbGet)
    return MAPATTR_RET_ERROR;

  ATTRMAP::iterator iter = m_mapAttr.find(szKey);
  if (iter == m_mapAttr.end())
    return MAPATTR_RET_NOT_EXIST;

  int lRet = MAPATTR_RET_OK;

  const char *szValue = iter->second.c_str();
  if (0 == strcmp(szValue, XML_BOOL_VALUE_TRUE) ||
      0 == strcmp(szValue, XML_BOOL_VALUE_1) ||
      0 == strcmp(szValue, XML_BOOL_VALUE_YES)) {
    *pbGet = true;
  } else if (0 == strcmp(szValue, XML_BOOL_VALUE_FALSE) ||
             0 == strcmp(szValue, XML_BOOL_VALUE_0) ||
             0 == strcmp(szValue, XML_BOOL_VALUE_NO)) {
    *pbGet = false;
  } else {
    lRet = MAPATTR_RET_INVLID_VALUE;
  }

  if (bErase)
    m_mapAttr.erase(iter);

  return lRet;
}

int CMapAttribute::GetAttr_int(const char *szPrefix, const char *szKey,
                                bool bErase, int *pnGet) {
  std::string strKey;
  if (szPrefix)
    strKey = szPrefix;
  strKey.append(szKey);

  return GetAttr_int(strKey.c_str(), bErase, pnGet);
}
int CMapAttribute::GetAttr_int(const char *szKey, bool bErase, int *pnGet) {
  if (nullptr == szKey || nullptr == pnGet)
    return MAPATTR_RET_ERROR;

  ATTRMAP::iterator iter = m_mapAttr.find(szKey);
  if (iter == m_mapAttr.end())
    return MAPATTR_RET_NOT_EXIST;

  *pnGet = atoi(iter->second.c_str());

  if (bErase)
    m_mapAttr.erase(iter);

  return MAPATTR_RET_OK;
}

int CMapAttribute::GetAttr_intarray(const char *szPrefix,
                                     const char *szKey, bool bErase,
                                     int *pIntArray, unsigned int nSize) {
  std::string strKey;
  if (szPrefix)
    strKey = szPrefix;
  strKey.append(szKey);

  return GetAttr_intarray(strKey.c_str(), bErase, pIntArray, nSize);
}
int CMapAttribute::GetAttr_intarray(const char *szKey, bool bErase,
                                     int *pIntArray, unsigned int nSize) {
  if (nullptr == szKey || nullptr == pIntArray)
    return MAPATTR_RET_ERROR;

  ATTRMAP::iterator iter = m_mapAttr.find(szKey);
  if (iter == m_mapAttr.end())
    return MAPATTR_RET_NOT_EXIST;

  util::ISplitStringEnum *pEnum = nullptr;
  unsigned int nCount =
      util::SplitString(iter->second.c_str(), XML_SEPARATOR, &pEnum);
  if (nCount != nSize) {
    SAFE_RELEASE(pEnum);
    return MAPATTR_RET_INVLID_VALUE;
  }
  for (unsigned int i = 0; i < nCount; i++) {
    pIntArray[i] = atoi(pEnum->GetText(i));
  }

  SAFE_RELEASE(pEnum);
  if (bErase)
    m_mapAttr.erase(iter);

  return MAPATTR_RET_OK;
}

int CMapAttribute::GetAttr_REGION4(const char *szPrefix,
                                    const char *szKey, bool bErase,
                                    REGION4 *prcGet) {
  if (nullptr == szKey || nullptr == prcGet)
    return MAPATTR_RET_ERROR;

  std::string strKey;
  if (szPrefix)
    strKey = szPrefix;
  strKey.append(szKey);

  ATTRMAP::iterator iter = m_mapAttr.find(strKey.c_str());
  if (iter == m_mapAttr.end())
    return MAPATTR_RET_NOT_EXIST;

  int lRet = MAPATTR_RET_OK;
  if (false == util::TranslateRECT(iter->second.c_str(), prcGet))
    lRet = MAPATTR_RET_INVLID_VALUE;

  if (bErase)
    m_mapAttr.erase(iter);

  return lRet;
}

int CMapAttribute::GetAttr_Image9Region(const char *szPrefix,
                                         const char *szKey, bool bErase,
                                         C9Region *pRegion) {
  if (nullptr == szKey || nullptr == pRegion)
    return MAPATTR_RET_ERROR;

  std::string strKey;
  if (szPrefix)
    strKey = szPrefix;
  strKey.append(szKey);

  ATTRMAP::iterator iter = m_mapAttr.find(strKey.c_str());
  if (iter == m_mapAttr.end())
    return MAPATTR_RET_NOT_EXIST;

  int lRet = MAPATTR_RET_OK;
  if (false == util::TranslateImage9Region(iter->second.c_str(), pRegion))
    lRet = MAPATTR_RET_INVLID_VALUE;

  if (bErase)
    m_mapAttr.erase(iter);

  return lRet;
}

// int CMapAttribute::GetAttr_RenderBase(
//     const char* szPrefix,
//     const char* szKey,
//     bool bErase,
//     IApplication* pUIApp,
//     IObject* pBindObj,
//     IRenderBase** ppGet)
// {
//     if (nullptr == szKey || nullptr == ppGet || nullptr == pUIApp)
//         return MAPATTR_RET_ERROR;
//
//     LRESULT lRet = MAPATTR_RET_OK;
//     std::string strKey;
//     if (szPrefix)
//         strKey = szPrefix;
//     strKey.append(szKey);
//
//     ATTRMAP::iterator iter = m_mapAttr.find(strKey);
//     if (iter == m_mapAttr.end())
//         return MAPATTR_RET_NOT_EXIST;
//
//     SAFE_RELEASE(*ppGet);
//
//     pUIApp->GetImpl()->CreateRenderBaseByName(
// 		iter->second.c_str(), pBindObj, ppGet);
//     if (*ppGet)
//     {
//         SerializeParam data = {0};
//         data.pUIApplication = pUIApp;
// 		data.pSkinRes = pBindObj?pBindObj->GetResource():nullptr;
//         data.pMapAttrib = static_cast<IMapAttribute*>(this);
//         data.szPrefix = szPrefix;
//         data.nFlags = SERIALIZEFLAG_LOAD;
//         if (bErase)
//             data.nFlags |= SERIALIZEFLAG_LOAD_ERASEATTR;
//         (*ppGet)->Serialize(&data);
//     }
//     else
//     {
//         lRet = MAPATTR_RET_INVLID_VALUE;
//     }
//
//     if (bErase)
//         m_mapAttr.erase(iter);
//     return lRet;
// }
//
// int CMapAttribute::GetAttr_TextRenderBase(
//     const char* szPrefix,
//     const char* szKey,
//     bool bErase,
//     IResource* pUIApp,
//     IObject* pBindObj,
//     ITextRenderBase** ppGet)
// {
//     if (nullptr == szKey || nullptr == ppGet)
//         return MAPATTR_RET_ERROR;
//
//     LRESULT lRet = MAPATTR_RET_OK;
//     std::string strKey;
//     if (szPrefix)
//         strKey = szPrefix;
//     strKey.append(szKey);
//
//     ATTRMAP::iterator iter = m_mapAttr.find(strKey.c_str());
//     if (iter == m_mapAttr.end())
//         return MAPATTR_RET_NOT_EXIST;
//
//     SAFE_RELEASE(*ppGet);
//
//     pUIApp->CreateTextRenderBaseByName(
//             (BSTR)iter->second.c_str(), pBindObj, ppGet);
//     if (*ppGet)
//     {
//         SerializeParam data = {0};
//         data.pUIApplication = pUIApp;
// 		data.pSkinRes = pBindObj ? pBindObj->GetResource() : nullptr;
//         data.pMapAttrib = static_cast<IMapAttribute*>(this);
//         data.szPrefix = szPrefix;
//         data.nFlags = SERIALIZEFLAG_LOAD;
//         if (bErase)
//             data.nFlags |= SERIALIZEFLAG_LOAD_ERASEATTR;
//         (*ppGet)->Serialize(&data);
//     }
//     else
//     {
//         lRet = MAPATTR_RET_INVLID_VALUE;
//     }
//
//     if (bErase)
//         m_mapAttr.erase(iter);
//     return lRet;
// }
//
// int  CMapAttribute::GetAttr_Color(
//         const char* szPrefix,
//         const char* szKey,
//         bool bErase,
//         IResource* pSkinRes,
//         Color** ppColor)
// {
//     UIASSERT(pSkinRes);
//
//     if (nullptr == szKey || nullptr == ppColor || !pSkinRes)
//         return MAPATTR_RET_ERROR;
//
//     LRESULT lRet = MAPATTR_RET_OK;
//     std::string strKey;
//     if (szPrefix)
//         strKey = szPrefix;
//     strKey.append(szKey);
//
//     ATTRMAP::iterator iter = m_mapAttr.find(strKey.c_str());
//     if (iter == m_mapAttr.end())
//         return MAPATTR_RET_NOT_EXIST;
//
//     SAFE_RELEASE(*ppColor);
//
//     IColorRes* pColorRes = pSkinRes->GetColorRes();
//     pColorRes->GetColor(iter->second.c_str(), ppColor);
//
//     if (bErase)
//         m_mapAttr.erase(iter);
//
//     if (!(*ppColor))
//         lRet = MAPATTR_RET_ERROR;
//
//     return lRet;
// }

bool CMapAttribute::AddAttr(const char *szKey, const char *szValue) {
  if (nullptr == szKey)
    return false;

  if (szValue) {
    m_mapAttr[szKey] = szValue;
  } else {
    ATTRMAP::iterator iter = m_mapAttr.find(szKey);
    if (iter != m_mapAttr.end())
      iter->second.clear();
  }
  return true;
}
void CMapAttribute::AddAttr(const std::string &strKey, const std::string &strValue) {
  m_mapAttr[strKey] = strValue;
}

bool CMapAttribute::AddAttr_bool(const char *szKey, bool bValue) {
  if (nullptr == szKey)
    return false;

  m_mapAttr[szKey] = bValue ? "1" : "0";
  return true;
}

bool CMapAttribute::AddAttr_REGION4(const char *szKey, REGION4 *pr) {
  if (nullptr == szKey)
    return false;

  char szText[32] = {0};
  sprintf(szText, "%d,%d,%d,%d", pr->left, pr->top, pr->right,
          pr->bottom);

  m_mapAttr[szKey] = szText;
  return true;
}

bool CMapAttribute::AddAttr_int(const char *szKey, int nValue) {
  if (nullptr == szKey)
    return false;

  char szText[8] = {0};
  sprintf(szText, "%d", nValue);

  m_mapAttr[szKey] = szText;
  return true;
}

// 使用前缀Prefix，从当前属性中抽取出相应的属性集放在ppMapAttribute中返回给外部
// 外部调用完之后，需要使用ppMapAttribute->Release释放内存
bool CMapAttribute::ExtractMapAttrByPrefix(
    const char *szPrefix, bool bErase,
    /*out*/ IMapAttribute **ppMapAttribute) {
  if (nullptr == ppMapAttribute)
    return false;

  if (nullptr == szPrefix || 0 == strlen(szPrefix)) {
    //         *ppMapAttribute = static_cast<IMapAttribute*>(this);
    //         this->AddRef();
    //         return true;
    return false;
  }

  ATTRMAP::iterator iter = m_mapAttr.begin();
  ATTRMAP::iterator iterEnd = m_mapAttr.end();

  int nPrifixLength = (int)strlen(szPrefix);

  IMapAttribute *pSubMapAttrib = UICreateIMapAttribute();

  for (; iter != iterEnd;) {
    char *szKey = const_cast<char *>(iter->first.c_str());
    if (strstr(szKey, szPrefix) == szKey) {
      pSubMapAttrib->AddAttr(szKey + nPrifixLength, iter->second.c_str());

      if (bErase)
        m_mapAttr.erase(iter++);
      else
        ++iter;
    } else {
      ++iter;
    }
  }

  *ppMapAttribute = pSubMapAttrib;
  return true;
}

void CMapAttribute::Destroy() {
    delete this;
}

// 将自己的属性拷贝给pDestMapAttrib，如果pDestMapAttrib中已经存在，则按钮bOverride参数判断是否覆盖
void CMapAttribute::CopyTo(IMapAttribute *pDestMapAttrib, bool bOverride) {
  if (nullptr == pDestMapAttrib)
    return;

  ATTRMAP::iterator iter = m_mapAttr.begin();
  if (bOverride) {
    for (; iter != m_mapAttr.end(); iter++) {
      pDestMapAttrib->AddAttr(iter->first.c_str(), iter->second.c_str());
    }
  } else {
    for (; iter != m_mapAttr.end(); iter++) {
      if (pDestMapAttrib->HasAttrib(iter->first.c_str()))
        continue;

      pDestMapAttrib->AddAttr(iter->first.c_str(), iter->second.c_str());
    }
  }
}

void CMapAttribute::BeginEnum() { m_iterEnum = m_mapAttr.begin(); }
bool CMapAttribute::EnumNext(const char **szKey, const char **szValue) {
  if (nullptr == szKey || nullptr == szValue)
    return false;

  if (m_iterEnum == m_mapAttr.end())
    return false;

  *szKey = m_iterEnum->first.c_str();
  *szValue = m_iterEnum->second.c_str();

  m_iterEnum++;
  return true;
}
void CMapAttribute::EndEnum() { m_iterEnum = m_mapAttr.end(); }

IMapAttribute* UICreateIMapAttribute() {
  CMapAttribute *p = new CMapAttribute;
  return static_cast<IMapAttribute *>(p);
}

int UICreateIListAttribute(IListAttribute **ppOut) {
  if (nullptr == ppOut)
    return -1; // E_INVALIDARG;

  CListAttribute *p = new CListAttribute;
  *ppOut = static_cast<IListAttribute *>(p);
  p->AddRef();

  return 0;
}

//////////////////////////////////////////////////////////////////////////

CListAttribute::CListAttribute() {
  m_lRef = 0;
  m_pFirstItem = nullptr;
  m_pLastItem = nullptr;
  m_pEnum = nullptr;
}
CListAttribute::~CListAttribute() {
  ListAttrItem *pItem = m_pFirstItem;
  while (pItem) {
    ListAttrItem *pNext = pItem->pNext;
    SAFE_DELETE(pItem);
    pItem = pNext;
  }
  m_pFirstItem = nullptr;
  m_pLastItem = nullptr;
}

bool CListAttribute::AddAttr(const char *szKey, const char *szValue) {
  ListAttrItem *pItem = new ListAttrItem;
  if (szKey)
    pItem->strKey = szKey;
  if (szValue)
    pItem->strValue = szValue;

  if (nullptr == m_pLastItem) {
    m_pFirstItem = m_pLastItem = pItem;
  } else {
    m_pLastItem->pNext = pItem;
    pItem->pPrev = m_pLastItem;

    m_pLastItem = pItem;
  }

  return true;
}

const char *CListAttribute::GetAttr(const char *szKey) {
  ListAttrItem *pItem = FindItem(szKey);
  if (pItem)
    return pItem->strValue.c_str();

  return nullptr;
}

ListAttrItem *CListAttribute::FindItem(const char *szKey) {
  if (!szKey)
    return nullptr;

  ListAttrItem *pItem = m_pFirstItem;
  while (pItem) {
    if (pItem->strKey == szKey) {
      return pItem;
    }
    pItem = pItem->pNext;
  }

  return nullptr;
}

bool CListAttribute::EraseAttr(const char *szKey) {
  ListAttrItem *pItem = FindItem(szKey);
  if (!pItem)
    return false;

  if (pItem == m_pEnum) {
    m_pEnum = m_pEnum->pNext;
  }
  if (m_pFirstItem == pItem) {
    m_pFirstItem = pItem->pNext;
  }
  if (m_pLastItem == pItem) {
    m_pLastItem = pItem->pPrev;
  }

  if (pItem->pNext)
    pItem->pNext->pPrev = pItem->pPrev;
  if (pItem->pPrev)
    pItem->pPrev->pNext = pItem->pNext;

  SAFE_DELETE(pItem);
  return true;
}

// void  CListAttribute::SetTag(const char* szKey)
// {
// 	if (szKey)
// 		m_strTag = szKey;
// 	else
// 		m_strTag.clear();
// }
//
// const char*  CListAttribute::GetTag()
// {
//     return m_strTag.c_str();
// }

void CListAttribute::BeginEnum() { m_pEnum = m_pFirstItem; }
bool CListAttribute::EnumNext(const char **szKey, const char **szValue) {
  if (nullptr == m_pEnum)
    return false;

  if (szKey)
    *szKey = m_pEnum->strKey.c_str();
  if (szValue)
    *szValue = m_pEnum->strValue.c_str();

  m_pEnum = m_pEnum->pNext;
  return true;
}
void CListAttribute::EndEnum() { m_pEnum = nullptr; }

int CListAttribute::Release() {
  UIASSERT(m_lRef > 0);
  --m_lRef;
  if (0 == m_lRef) {
    delete this;
    return 0;
  }
  return m_lRef;
}
int CListAttribute::AddRef() { return ++m_lRef; }

} // namespace ui
