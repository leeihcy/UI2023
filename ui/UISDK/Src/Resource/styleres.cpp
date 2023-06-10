#include "stdafx.h"
#include "styleres.h"
#include "Src\Resource\skinres.h"
#include "Inc\Interface\ixmlwrap.h"
#include "Inc\Interface\iuires.h"
#include "Inc\Interface\imapattr.h"


using namespace UI;
	
StyleResItem::StyleResItem()
{
    m_pIStyleResItem = nullptr;
    m_pMapAttrib = nullptr;
	m_pXmlElement = nullptr;
	m_eSelectorType = STYLE_SELECTOR_TYPE_TAG;
}
StyleResItem::~StyleResItem()
{
    SAFE_RELEASE(m_pMapAttrib);
    SAFE_DELETE(m_pIStyleResItem);
	SAFE_RELEASE(m_pXmlElement);
}

IStyleResItem*  StyleResItem::GetIStyleResItem()
{
    if (nullptr == m_pIStyleResItem)
        m_pIStyleResItem = new IStyleResItem(this);

    return m_pIStyleResItem;
}

void  StyleResItem::SetXmlElement(UIElement* p)
{
	SAFE_RELEASE(m_pXmlElement);
	m_pXmlElement = p;
	if (m_pXmlElement)
		m_pXmlElement->AddRef();
}
UIElement*  StyleResItem::GetXmlElement()
{
	return m_pXmlElement;
}
bool  StyleResItem::GetXmlElement2(UIElement** pp)
{
	if (!pp || !m_pXmlElement)
		return false;

	*pp = m_pXmlElement;
	m_pXmlElement->AddRef();
	return true;
}

void  StyleResItem::SetInherits(LPCTSTR sz)
{
	SETSTRING(m_strInherits, sz);
	m_vInheritList.clear();

	vector<String>  vStrArray;
	UI_Split(m_strInherits, XML_MULTI_SEPARATOR, vStrArray);
	int nSize = (int)vStrArray.size();
	if (0 != nSize )
	{
		for(int i = 0; i < nSize; i++ )
		{
			m_vInheritList.push_back(vStrArray[i]);
		}
	}
}
int StyleResItem::GetInheritCount() 
{
	return (int) m_vInheritList.size(); 
}

bool StyleResItem::GetInheritItem(int i, String& str)
{
	if (i < 0 || i >= GetInheritCount() )
		return false;

	str = m_vInheritList[i];
	return true;
}

//
// 仅从m_vInheritList中删除，不从m_strInherit中删除
//
bool StyleResItem::RemoveInheritItem(LPCTSTR sz)
{
    if (!sz)
        return false;

	vector<String>::iterator iter = m_vInheritList.begin();
	for (; iter != m_vInheritList.end(); ++iter)
	{
		if (_tcscmp(iter->c_str(),sz) == 0)
		{
			m_vInheritList.erase(iter);
			return true;
		}
	}

	return false;
}

void  StyleResItem::SetAttributeMap(IMapAttribute* pMapAttrib)
{
    SAFE_RELEASE(m_pMapAttrib);
    m_pMapAttrib = pMapAttrib;
    if (m_pMapAttrib)
        m_pMapAttrib->AddRef();
}

void StyleResItem::GetAttributeMap(IMapAttribute** ppMapAttrib)
{
	if (nullptr == ppMapAttrib)
		return;

	*ppMapAttrib = m_pMapAttrib;
    if (m_pMapAttrib)
        m_pMapAttrib->AddRef();
}

void StyleResItem::SetAttribute(LPCTSTR key, LPCTSTR value)
{
    if (!m_pMapAttrib)
    {
        UICreateIMapAttribute(&m_pMapAttrib);
    }

    m_pMapAttrib->AddAttr(key, value);
}

LPCTSTR StyleResItem::GetAttribute(LPCTSTR key)
{
    if (!key || !m_pMapAttrib)
        return false;

	if (0 == _tcscmp(key, XML_ID))
		return m_strID.c_str();

    return m_pMapAttrib->GetAttr(key, false);
}

bool StyleResItem::ModifyAttribute(LPCTSTR key, LPCTSTR value)
{
	if (!key || !m_pMapAttrib)
		return false;

	if (0 == _tcscmp(key, XML_ID))
	{
		SetId(value);
		return true;
	}

	if (!m_pMapAttrib->HasAttrib(key))
		return false;

	m_pMapAttrib->AddAttr(key, value);
	return true;
}

bool StyleResItem::RemoveAttribute(LPCTSTR key)
{
	if (!m_pMapAttrib)
		return false;

	if (m_pMapAttrib->GetAttr(key, true))
		return true;

	return false;
}

bool StyleResItem::InheritAttribute(LPCTSTR key, LPCTSTR value)
{
    // 继承一个属性值。如果自己已经定义了这个属性，则使用自身的值
    if (nullptr == m_pMapAttrib->GetAttr(key, false))
    {
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
bool StyleResItem::InheritMyAttributesToAnother(StyleResItem* pChild)
{
	if (nullptr == pChild)
		return false;

    IMapAttribute*  pMapAttrib = nullptr;
    pChild->GetAttributeMap(&pMapAttrib);
    if (pMapAttrib)
    {
        m_pMapAttrib->CopyTo(pMapAttrib, false);
        SAFE_RELEASE(pMapAttrib);
    }
   
	return true;
}

//
//	将自己的属性应用到一个已有的map中，如果map中已有这个属性，则使用该map的属性
//
bool StyleResItem::Apply(IMapAttribute* pMapAttrib, bool bOverwrite)
{
    if (!pMapAttrib || !m_pMapAttrib)
        return false;

    m_pMapAttrib->CopyTo(pMapAttrib, bOverwrite);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 
//   StyleRes
//

StyleRes::StyleRes(SkinRes* p)
{
    m_pIStyleRes = nullptr;
	m_pSkinRes = p;
}

StyleRes::~StyleRes()
{
	this->Clear();
    SAFE_DELETE(m_pIStyleRes);
}
IStyleRes&  StyleRes::GetIStyleRes()
{
    if (!m_pIStyleRes)
        m_pIStyleRes = new IStyleRes(this);

    return *m_pIStyleRes;
}

void StyleRes::Clear()
{
	vector<StyleResItem*>::iterator  iter = m_vStyles.begin();
	for (; iter != m_vStyles.end(); ++iter)
	{
		StyleResItem* p = *iter;
		delete p;
		p = nullptr;
	}

	m_vStyles.clear();
}

StyleResItem*  StyleRes::Add(STYLE_SELECTOR_TYPE type, LPCTSTR szId, LPCTSTR szInherit)
{
	StyleResItem* pStyleItem = new StyleResItem;
	pStyleItem->SetSelectorType(type);
	pStyleItem->SetId(szId);
	pStyleItem->SetInherits(szInherit);

	bool bRet = this->Add(pStyleItem);
	if (false == bRet)
	{
		delete pStyleItem;
		pStyleItem = nullptr;
        return nullptr;
	}

	return pStyleItem;
}

bool StyleRes::Add(StyleResItem* pNewItem)
{
	if (nullptr == pNewItem)
		return false;

	StyleResItem* p = this->GetItem(pNewItem->GetSelectorType(), pNewItem->GetId());
	if (p)
	{
		UI_LOG_WARN(_T("StyleRes::Add failed, insert item=%s"), pNewItem->GetId());
		return false;
	}

	this->m_vStyles.push_back(pNewItem);  
	m_pSkinRes->GetStyleManager().OnStyleAdd(pNewItem);
	return true;
}

StyleResItem*  StyleRes::Insert(STYLE_SELECTOR_TYPE type, LPCTSTR szId, long lPos)
{
	if (!szId)
		return nullptr;

	if (this->GetItem(type, szId))
	{
		UI_LOG_WARN(TEXT("StyleRes::Insert type=%d, id=%s Failed."), type, szId);
		return nullptr;
	}

	StyleResItem* pStyleItem = new StyleResItem;
	pStyleItem->SetSelectorType(type);
	pStyleItem->SetId(szId);

	vector<StyleResItem*>::iterator iter = m_vStyles.begin();
	iter += lPos;
	if (iter == m_vStyles.end())
		m_vStyles.push_back(pStyleItem);
	else
		m_vStyles.insert(iter, pStyleItem);

	m_pSkinRes->GetStyleManager().OnStyleAdd(pStyleItem);
	return pStyleItem;
}

bool  StyleRes::Modify(StyleResItem* pItem, STYLE_SELECTOR_TYPE type, LPCTSTR szId, LPCTSTR szInherit)
{
	if (!pItem)
		return false;

	pItem->SetId(szId);
	pItem->SetSelectorType(type);
	pItem->SetInherits(szInherit);

	m_pSkinRes->GetStyleManager().OnStlyeModify(pItem);
	return true;
}

bool  StyleRes::Remove(STYLE_SELECTOR_TYPE type, LPCTSTR szId)
{
	vector<StyleResItem*>::iterator  iter = m_vStyles.begin();
	for ( ; iter != m_vStyles.end(); ++iter)
	{
		StyleResItem* p = *iter;
		if (p->GetSelectorType() == type && 0== _tcscmp(p->GetId(),szId))
		{
			m_pSkinRes->GetStyleManager().OnStyleRemove(p);
			delete p;
			p = nullptr;
			m_vStyles.erase(iter);
			return true;
		}
	}
	return false;
}

bool  StyleRes::Remove(StyleResItem* pNewItem)
{
	vector<StyleResItem*>::iterator  iter = std::find(m_vStyles.begin(), m_vStyles.end(), pNewItem);
	if (iter == m_vStyles.end())
		return false;

	m_pSkinRes->GetStyleManager().OnStyleRemove(*iter);
	m_vStyles.erase(iter);
	delete pNewItem;
	return true;
}

bool  StyleRes::AddAttribute(StyleResItem* pItem, LPCTSTR szKey, LPCTSTR szValue)
{
	if (!pItem)
		return false;

	pItem->SetAttribute(szKey, szValue);

	m_pSkinRes->GetStyleManager().OnStyleAttributeAdd(pItem, szKey);
	return true;
}

bool  StyleRes::ModifyAttribute(StyleResItem* pItem, LPCTSTR szKey, LPCTSTR szValue)
{
	if (!pItem)
		return false;

	pItem->ModifyAttribute(szKey, szValue);

	m_pSkinRes->GetStyleManager().OnStyleAttributeModify(pItem, szKey);
	return true;
}

bool  StyleRes::RemoveAttribute(StyleResItem* pItem, LPCTSTR szKey)
{
	if (!pItem)
		return false;

	pItem->RemoveAttribute(szKey);

	m_pSkinRes->GetStyleManager().OnStyleAttributeRemove(pItem, szKey);
	return true;
}

long  StyleRes::GetCount() 
{
	return (long)m_vStyles.size();
}


StyleResItem* StyleRes::GetItem(long nIndex )
{
	if (nIndex < 0)
		return nullptr;
	if (nIndex >= (int)m_vStyles.size())
		return nullptr;

	return m_vStyles[nIndex];
}

StyleResItem* StyleRes::GetItem(STYLE_SELECTOR_TYPE type, LPCTSTR szId)
{
	if (nullptr == szId)
		return nullptr;

	vector<StyleResItem*>::iterator  iter = m_vStyles.begin();
	for ( ; iter != m_vStyles.end(); ++iter)
	{
		StyleResItem* p = *iter;
		if (p->GetSelectorType() == type && 
            0 == _tcscmp(p->GetId(), szId))
		{
			return p;
		}
	}

    SkinRes* pParentRes = m_pSkinRes->GetParentSkinRes();
    if (pParentRes)
    {
        return pParentRes->GetStyleRes().GetItem(type, szId);
    }

	return nullptr;
}

// 用于删除时保存当前位置
long  StyleRes::GetItemPos(StyleResItem* p)
{
	if (!p)
		return -1;

	vector<StyleResItem*>::iterator iter = std::find(m_vStyles.begin(), m_vStyles.end(), p);
	if (iter == m_vStyles.end())
		return -1;

	return (long)(iter - m_vStyles.begin());
}

// bool StyleRes::AddStyleAttribute(STYLE_SELECTOR_TYPE type, LPCTSTR szId, LPCTSTR szKey, LPCTSTR szValue)
// {
// 	StyleResItem* pItem = this->GetItem(type, szId);
// 	if (false == pItem)
// 		return false;
// 
// 	return pItem->InsertAttribute(szKey, szValue);
// }
// 
// bool StyleRes::ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, LPCTSTR szId, LPCTSTR szKey, LPCTSTR szValue)
// {
// 	StyleResItem* pItem = this->GetItem(type, szId);
// 	if (false == pItem)
// 		return false;
// 
// 	return pItem->ModifyAttribute(szKey, szValue);
// }
// 
// bool StyleRes::RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, LPCTSTR szId, LPCTSTR szKey)
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
//			级别4. CLASS列表中，排后面的样式匹配的属性 > 前面的样式匹配的属性
//			级别5. 通过TAG匹配的属性
//
bool  StyleRes::LoadStyle(LPCTSTR szTagName, LPCTSTR szStyleClass, LPCTSTR szID, IMapAttribute* pMapStyle)
{
	if (szID && _tcslen(szID) > 0)
	{
		StyleResItem* pIDStyleItem = this->GetItem(STYLE_SELECTOR_TYPE_ID, szID);
		if (pIDStyleItem)
		{
			pIDStyleItem->Apply(pMapStyle, false);
		}
	}

	// strStyleClass可能有多个
	if (szStyleClass && _tcslen(szStyleClass) > 0)
	{
		vector<String>  vStrArray;
		UI_Split(szStyleClass, XML_MULTI_SEPARATOR, vStrArray); 
		int nCount = (int)vStrArray.size();
		if (0 != nCount)
		{
			for (int i = 0; i < nCount; i++)
			{
				if (vStrArray[i].empty())
					continue;

				StyleResItem* pClassStyleItem = this->GetItem(STYLE_SELECTOR_TYPE_CLASS, vStrArray[i].c_str());
				if (pClassStyleItem)
				{
					pClassStyleItem->Apply(pMapStyle, false);
				}
			}
		}
	}

	if (szTagName && _tcslen(szTagName) > 0)
	{
		StyleResItem* pTagStyleItem = this->GetItem(STYLE_SELECTOR_TYPE_TAG, szTagName);
		if (pTagStyleItem)
		{
			pTagStyleItem->Apply(pMapStyle, false);
		}
	}

	return true;
} 

// 将pListAttribte中属于style的属性过滤掉
bool  StyleRes::UnloadStyle(LPCTSTR szTagName, LPCTSTR szStyleClass, LPCTSTR szID, IListAttribute* pListAttribte)
{
    // 先拿到所有的样式列表
    IMapAttribute*  pStyleAttr = nullptr;
    UICreateIMapAttribute(&pStyleAttr);
    LoadStyle(szTagName, szStyleClass, szID, pStyleAttr);

    pListAttribte->BeginEnum();
    
    const TCHAR* szKey = nullptr;
    const TCHAR* szValue = nullptr;
    while (pListAttribte->EnumNext(&szKey, &szValue))
    {
        LPCTSTR szStyleValue = pStyleAttr->GetAttr(nullptr, szKey, true);
        if (!szStyleValue)
            continue;

        // 该属性已包含于style中，不需要配置，删掉
        if (0 == _tcscmp(szStyleValue, szValue))
        {
            pListAttribte->EraseAttr(szKey);
            continue;
        }
        // 该属性与style不一致，必须配置，以覆盖style
        else
        {

        }
    }
    pListAttribte->EndEnum();

    // 还剩下一个style没有被删干净，则需要额外添加一些空值给控件，避免使用style中的样式
    pStyleAttr->BeginEnum();
    while (pStyleAttr->EnumNext(&szKey, &szValue))
    {
        pListAttribte->AddAttr(szKey, L"");
    }
    pStyleAttr->EndEnum();


    SAFE_RELEASE(pStyleAttr);
    return true;

}