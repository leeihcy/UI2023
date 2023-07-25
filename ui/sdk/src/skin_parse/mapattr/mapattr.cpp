#include "include/inc.h"
#include "mapattr.h"
#include "src/render/renderbase.h"
#include "src/render/textrender/textrender.h"
#include "include/interface/iuires.h"
#include "include/interface/iuiapplication.h"
#include "src/application/uiapplication.h"

namespace ui
{

CMapAttribute::CMapAttribute()
{
    m_lRef = 0;
    m_iterEnum = m_mapAttr.end();
}

int CMapAttribute::GetAttrCount()
{
	return (int)m_mapAttr.size();
}

CMapAttribute::~CMapAttribute()
{

}
CMapAttribute::CMapAttribute(const CMapAttribute& o)
{
    m_mapAttr = o.m_mapAttr;
}

// void  CMapAttribute::SetTag(const wchar_t* szKey)
// {
// 	if (szKey)
// 		m_strTag = szKey;
// 	else
// 		m_strTag.clear();
// }
// const wchar_t*  CMapAttribute::GetTag()
// {
// 	return m_strTag.c_str();
// }
bool  CMapAttribute::HasAttrib(const wchar_t* szKey)
{
    if (nullptr == szKey)
        return false;

    ATTRMAP::iterator iter = m_mapAttr.find(szKey);
    if (iter == m_mapAttr.end())
        return false;

    return true;
}
const wchar_t* CMapAttribute::GetAttr(const wchar_t* szKey, bool bErase)
{
    if (nullptr == szKey)
        return nullptr;

    ATTRMAP::iterator iter = m_mapAttr.find(szKey);
    if (iter == m_mapAttr.end())
        return nullptr;

    if (bErase)
    {
        String& str = GetTempBufferString();
        str = iter->second;
        m_mapAttr.erase(iter);
        return str.c_str();
    }
    return iter->second.c_str();
}

const wchar_t* CMapAttribute::GetAttr(const String& strKey, bool bErase)
{
    ATTRMAP::iterator iter = m_mapAttr.find(strKey);
    if (iter == m_mapAttr.end())
        return nullptr;

    if (bErase)
    {
        String& strTemp = GetTempBufferString();
        strTemp = iter->second;
        m_mapAttr.erase(iter);
        return strTemp.c_str();
    }
    return iter->second.c_str();
}
const wchar_t* CMapAttribute::GetAttr(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase)
{
    String strKey;
    if (szPrefix)
        strKey = szPrefix;
    strKey.append(szKey);

    return GetAttr(strKey.c_str(), bErase);
}

long CMapAttribute::GetAttr_bool(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase, bool* pbGet)
{
    String strKey;
    if (szPrefix)
        strKey = szPrefix;
    strKey.append(szKey);

    return GetAttr_bool(strKey.c_str(), bErase, pbGet);
}
long CMapAttribute::GetAttr_bool(const wchar_t* szKey, bool bErase, bool* pbGet)
{
    if (nullptr == szKey || nullptr == pbGet)
        return MAPATTR_RET_ERROR;

    ATTRMAP::iterator iter = m_mapAttr.find(szKey);
    if (iter == m_mapAttr.end())
        return MAPATTR_RET_NOT_EXIST;
    
    long lRet = MAPATTR_RET_OK;

    const wchar_t* szValue = iter->second.c_str();
    if (0 == wcscmp(szValue, XML_BOOL_VALUE_TRUE)  || 
        0 == wcscmp(szValue, XML_BOOL_VALUE_1)     ||
        0 == wcscmp(szValue, XML_BOOL_VALUE_YES))
    {
        *pbGet = true; 
    }
    else if (0 == wcscmp(szValue, XML_BOOL_VALUE_FALSE) ||
        0 == wcscmp(szValue, XML_BOOL_VALUE_0)      ||
        0 == wcscmp(szValue, XML_BOOL_VALUE_NO))
    {
        *pbGet = false;
    }
    else 
    {
        lRet = MAPATTR_RET_INVLID_VALUE;
    }

    if (bErase)
        m_mapAttr.erase(iter);

    return lRet;
}

long CMapAttribute::GetAttr_int(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase, int* pnGet)
{
    String strKey;
    if (szPrefix)
        strKey = szPrefix;
    strKey.append(szKey);

    return GetAttr_int(strKey.c_str(), bErase, pnGet);
}
long CMapAttribute::GetAttr_int(const wchar_t* szKey, bool bErase, int* pnGet)
{
    if (nullptr == szKey || nullptr == pnGet)
        return MAPATTR_RET_ERROR;

    ATTRMAP::iterator iter = m_mapAttr.find(szKey);
    if (iter == m_mapAttr.end())
        return MAPATTR_RET_NOT_EXIST;

    *pnGet = util::wtoi(iter->second.c_str());

    if (bErase)
        m_mapAttr.erase(iter);

    return MAPATTR_RET_OK;
}



long  CMapAttribute::GetAttr_intarray(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase, int* pIntArray, unsigned int nSize)
{
    String strKey;
    if (szPrefix)
        strKey = szPrefix;
    strKey.append(szKey);

    return GetAttr_intarray(strKey.c_str(), bErase, pIntArray, nSize);
}
long  CMapAttribute::GetAttr_intarray(const wchar_t* szKey, bool bErase, int* pIntArray, unsigned int nSize)
{
    if (nullptr == szKey || nullptr == pIntArray)
        return MAPATTR_RET_ERROR;

    ATTRMAP::iterator iter = m_mapAttr.find(szKey);
    if (iter == m_mapAttr.end())
        return MAPATTR_RET_NOT_EXIST;

    util::ISplitStringEnum* pEnum = nullptr;
    unsigned int nCount = util::SplitString(iter->second.c_str(), XML_SEPARATOR, &pEnum);
    if (nCount != nSize)
    {
        SAFE_RELEASE(pEnum);
        return MAPATTR_RET_INVLID_VALUE;
    }
    for (unsigned int i = 0; i < nCount; i++)
    {
        pIntArray[i] = util::wtoi(pEnum->GetText(i));
    }

    SAFE_RELEASE(pEnum);
    if (bErase)
        m_mapAttr.erase(iter);

    return MAPATTR_RET_OK;
}

long CMapAttribute::GetAttr_REGION4(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase, REGION4* prcGet)
{
    if (nullptr == szKey || nullptr == prcGet)
        return MAPATTR_RET_ERROR;

    String strKey;
    if (szPrefix)
        strKey = szPrefix;
    strKey.append(szKey);

    ATTRMAP::iterator iter = m_mapAttr.find(strKey.c_str());
    if (iter == m_mapAttr.end())
        return MAPATTR_RET_NOT_EXIST;

    long lRet = MAPATTR_RET_OK;
    if (false == util::TranslateRECT(iter->second.c_str(), prcGet))
        lRet = MAPATTR_RET_INVLID_VALUE;

    if (bErase)
        m_mapAttr.erase(iter);

    return lRet;
}

long  CMapAttribute::GetAttr_Image9Region(const wchar_t* szPrefix, const wchar_t* szKey, bool bErase, C9Region* pRegion)
{
    if (nullptr == szKey || nullptr == pRegion)
        return MAPATTR_RET_ERROR;

    String strKey;
    if (szPrefix)
        strKey = szPrefix;
    strKey.append(szKey);

    ATTRMAP::iterator iter = m_mapAttr.find(strKey.c_str());
    if (iter == m_mapAttr.end())
        return MAPATTR_RET_NOT_EXIST;

    long lRet = MAPATTR_RET_OK;
    if (false == util::TranslateImage9Region(iter->second.c_str(), pRegion))
        lRet = MAPATTR_RET_INVLID_VALUE;

    if (bErase)
        m_mapAttr.erase(iter);

    return lRet;
}

// long CMapAttribute::GetAttr_RenderBase(
//     const wchar_t* szPrefix,
//     const wchar_t* szKey, 
//     bool bErase,
//     IUIApplication* pUIApp, 
//     IObject* pBindObj, 
//     IRenderBase** ppGet)
// {
//     if (nullptr == szKey || nullptr == ppGet || nullptr == pUIApp)
//         return MAPATTR_RET_ERROR;
// 
//     LRESULT lRet = MAPATTR_RET_OK;
//     String strKey;
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
//         SERIALIZEDATA data = {0};
//         data.pUIApplication = pUIApp;
// 		data.pSkinRes = pBindObj?pBindObj->GetSkinRes():nullptr;
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
// long CMapAttribute::GetAttr_TextRenderBase(
//     const wchar_t* szPrefix, 
//     const wchar_t* szKey, 
//     bool bErase, 
//     ISkinRes* pUIApp,
//     IObject* pBindObj, 
//     ITextRenderBase** ppGet)
// {
//     if (nullptr == szKey || nullptr == ppGet)
//         return MAPATTR_RET_ERROR;
// 
//     LRESULT lRet = MAPATTR_RET_OK;
//     String strKey;
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
//         SERIALIZEDATA data = {0};
//         data.pUIApplication = pUIApp;
// 		data.pSkinRes = pBindObj ? pBindObj->GetSkinRes() : nullptr;
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
// long  CMapAttribute::GetAttr_Color(
//         const wchar_t* szPrefix,
//         const wchar_t* szKey, 
//         bool bErase, 
//         ISkinRes* pSkinRes,
//         Color** ppColor)
// {
//     UIASSERT(pSkinRes);
// 
//     if (nullptr == szKey || nullptr == ppColor || !pSkinRes)
//         return MAPATTR_RET_ERROR;
// 
//     LRESULT lRet = MAPATTR_RET_OK;
//     String strKey;
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

bool CMapAttribute::AddAttr(const wchar_t* szKey, const wchar_t* szValue) 
{
    if (nullptr == szKey)
        return false;

	if (szValue)
	{
		m_mapAttr[szKey] = szValue;
	}
	else
	{
		ATTRMAP::iterator iter = m_mapAttr.find(szKey);
		if (iter != m_mapAttr.end())
			iter->second.clear();
	}
    return true;
}
void CMapAttribute::AddAttr(const String& strKey, const String& strValue)
{
    m_mapAttr[strKey] = strValue;
}

bool  CMapAttribute::AddAttr_bool(const wchar_t* szKey, bool bValue)
{
	if (nullptr == szKey)
		return false;

	m_mapAttr[szKey] = bValue ? _T("1"):_T("0");
	return true;
}

bool  CMapAttribute::AddAttr_REGION4(const wchar_t* szKey, REGION4* pr)
{
	if (nullptr == szKey)
		return false;

	wchar szText[32] = {0};
	wprintf(szText, TEXT("%d,%d,%d,%d"), 
		pr->left, pr->top, pr->right, pr->bottom);

	m_mapAttr[szKey] = szText;
	return true;
}

bool  CMapAttribute::AddAttr_int(const wchar_t* szKey, int nValue)
{
	if (nullptr == szKey)
		return false;

	wchar szText[8] = {0};
	wprintf(szText, TEXT("%d"), nValue);

	m_mapAttr[szKey] = szText;
	return true;
}

// 使用前缀Prefix，从当前属性中抽取出相应的属性集放在ppMapAttribute中返回给外部
// 外部调用完之后，需要使用ppMapAttribute->Release释放内存
bool  CMapAttribute::ExtractMapAttrByPrefix(const wchar_t* szPrefix, bool bErase, /*out*/IMapAttribute** ppMapAttribute)
{
    if (nullptr == ppMapAttribute)
        return false;

    if (nullptr == szPrefix || 0 == wcslen(szPrefix))
    {
//         *ppMapAttribute = static_cast<IMapAttribute*>(this);
//         this->AddRef();
//         return true;
        return false;
    }

    ATTRMAP::iterator  iter = m_mapAttr.begin();
    ATTRMAP::iterator  iterEnd = m_mapAttr.end();

    int nPrifixLength = (int)wcslen(szPrefix);

    IMapAttribute*  pSubMapAttrib = nullptr;
    UICreateIMapAttribute(&pSubMapAttrib);

    for ( ; iter != iterEnd; )
    {
        wchar*  szKey = const_cast<wchar*>(iter->first.c_str());
        if (wcsstr(szKey, szPrefix) == szKey)
        {
            pSubMapAttrib->AddAttr(szKey + nPrifixLength, iter->second.c_str());

            if (bErase)
                m_mapAttr.erase(iter++);
            else
                ++iter;
        }
        else
        {
            ++iter;
        }
    }

    *ppMapAttribute = pSubMapAttrib;
    return true;
}

long  CMapAttribute::Release()
{
    UIASSERT(m_lRef > 0);
    --m_lRef;
    if (0 == m_lRef)
    {
        delete this;
        return 0;
    }
    return m_lRef;
}
long  CMapAttribute::AddRef()
{
    return ++ m_lRef;
}

void  CMapAttribute::CreateCopy(IMapAttribute** ppNewCopy)
{
    if (nullptr == ppNewCopy)
        return;

    CMapAttribute* p = new CMapAttribute;
    p->AddRef();
    *ppNewCopy = static_cast<IMapAttribute*>(p);
    p->m_mapAttr = m_mapAttr;
	//p->m_strTag = m_strTag;
}

// 将自己的属性拷贝给pDestMapAttrib，如果pDestMapAttrib中已经存在，则按钮bOverride参数判断是否覆盖
void  CMapAttribute::CopyTo(IMapAttribute* pDestMapAttrib, bool bOverride)
{
    if (nullptr == pDestMapAttrib)
        return;

    ATTRMAP::iterator  iter = m_mapAttr.begin();
    if (bOverride)
    {
        for (; iter != m_mapAttr.end(); iter++)
        {
            pDestMapAttrib->AddAttr(iter->first.c_str(), iter->second.c_str());
        }
    }
    else
    {
        for (; iter != m_mapAttr.end(); iter++)
        {
            if (pDestMapAttrib->HasAttrib(iter->first.c_str()))
                continue;

            pDestMapAttrib->AddAttr(iter->first.c_str(), iter->second.c_str());
        }
    }
}


void  CMapAttribute::BeginEnum()
{
    m_iterEnum = m_mapAttr.begin();
}
bool  CMapAttribute::EnumNext(const wchar** szKey, const wchar** szValue)
{
    if (nullptr == szKey || nullptr == szValue)
        return false;

    if (m_iterEnum == m_mapAttr.end())
        return false;

    *szKey = m_iterEnum->first.c_str();
    *szValue = m_iterEnum->second.c_str();

    m_iterEnum++;
    return true;
}
void  CMapAttribute::EndEnum()
{
    m_iterEnum = m_mapAttr.end();
}

long  UICreateIMapAttribute(IMapAttribute** ppOut)
{
    if (nullptr == ppOut)
        return -1; // E_INVALIDARG;

    CMapAttribute* p = new CMapAttribute;
    *ppOut = static_cast<IMapAttribute*>(p);
    p->AddRef();

    return 0;
}

long  UICreateIListAttribute(IListAttribute** ppOut)
{
    if (nullptr == ppOut)
        return -1; // E_INVALIDARG;

    CListAttribute* p = new CListAttribute;
    *ppOut = static_cast<IListAttribute*>(p);
    p->AddRef();

    return 0;
}

//////////////////////////////////////////////////////////////////////////

CListAttribute::CListAttribute()
{
    m_lRef = 0;
    m_pFirstItem = nullptr;
    m_pLastItem = nullptr;
    m_pEnum = nullptr;
}
CListAttribute::~CListAttribute()
{
    ListAttrItem* pItem = m_pFirstItem;
    while (pItem)
    {
        ListAttrItem*  pNext = pItem->pNext;
        SAFE_DELETE(pItem);
        pItem = pNext;
    }
    m_pFirstItem = nullptr;
    m_pLastItem = nullptr;
}


bool  CListAttribute::AddAttr(const wchar_t* szKey, const wchar_t* szValue)
{
    ListAttrItem* pItem = new ListAttrItem;
    if (szKey)
        pItem->strKey = szKey;
    if (szValue)
        pItem->strValue = szValue;

    if (nullptr == m_pLastItem)
    {
        m_pFirstItem = m_pLastItem = pItem;
    }
    else
    {
        m_pLastItem->pNext = pItem;
        pItem->pPrev = m_pLastItem;

        m_pLastItem = pItem;
    }

    return true;
}

const wchar_t*  CListAttribute::GetAttr(const wchar_t* szKey)
{
    ListAttrItem* pItem = FindItem(szKey);
    if (pItem)
        return pItem->strValue.c_str();

    return nullptr;
}

ListAttrItem*  CListAttribute::FindItem(const wchar_t* szKey)
{
    if (!szKey)
        return nullptr;

    ListAttrItem* pItem = m_pFirstItem;
    while (pItem)
    {
        if (pItem->strKey == szKey)
        {
            return pItem;
        }
        pItem = pItem->pNext;
    }

    return nullptr;
}


bool  CListAttribute::EraseAttr(const wchar_t* szKey)
{
    ListAttrItem* pItem = FindItem(szKey);
    if (!pItem)
        return false;

    if (pItem == m_pEnum)
    {
        m_pEnum = m_pEnum->pNext;
    }
    if (m_pFirstItem == pItem)
    {
        m_pFirstItem = pItem->pNext;
    }
    if (m_pLastItem == pItem)
    {
        m_pLastItem = pItem->pPrev;
    }

    if (pItem->pNext)
        pItem->pNext->pPrev = pItem->pPrev;
    if (pItem->pPrev)
        pItem->pPrev->pNext = pItem->pNext;

	SAFE_DELETE(pItem);
    return true;
}

// void  CListAttribute::SetTag(const wchar_t* szKey)
// {
// 	if (szKey)
// 		m_strTag = szKey;
// 	else
// 		m_strTag.clear();
// }
// 
// const wchar_t*  CListAttribute::GetTag()
// {
//     return m_strTag.c_str();
// }

void  CListAttribute::BeginEnum()
{
    m_pEnum = m_pFirstItem;
}
bool  CListAttribute::EnumNext(const wchar** szKey, const wchar** szValue)
{
    if (nullptr == m_pEnum)
        return false;

    if (szKey)
        *szKey = m_pEnum->strKey.c_str();
    if (szValue)
        *szValue = m_pEnum->strValue.c_str();

    m_pEnum = m_pEnum->pNext;
    return true;
}
void  CListAttribute::EndEnum()
{
    m_pEnum = nullptr;
}

long  CListAttribute::Release()
{
    UIASSERT(m_lRef > 0);
    --m_lRef;
    if (0 == m_lRef)
    {
        delete this;
        return 0;
    }
    return m_lRef;
}
long  CListAttribute::AddRef()
{
    return ++ m_lRef;
}

}


