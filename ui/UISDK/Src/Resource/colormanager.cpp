#include "StdAfx.h"
#include "colormanager.h"
#include "Inc/Interface/iuires.h"
#include "Src/SkinParse/xml/xmlwrap.h"
#include "Inc/Interface/ixmlwrap.h"
#include "Inc/Interface/imapattr.h"

ColorManager::ColorManager(SkinRes* pSkinRes): m_resColor(pSkinRes)
{
    m_pIColorManager = nullptr;
	m_pSkinRes = pSkinRes;
}

ColorManager::~ColorManager(void)
{
	this->Clear();
    SAFE_DELETE(m_pIColorManager);

	_MyListItem* pItem = m_listUIElement.GetFirst();
	while (pItem)
	{
		(**pItem).pXmlElement->Release();
		pItem = pItem->GetNext();
	}
	m_listUIElement.Clear();
}

IColorManager&  ColorManager::GetIColorManager()
{
    if (!m_pIColorManager)
        m_pIColorManager = new IColorManager(this);

    return *m_pIColorManager;
}
/*
**	清除所有col资源
*/
void ColorManager::Clear()
{
	m_resColor.Clear();
}

int ColorManager::GetColorCount()
{
	return m_resColor.GetColorCount();
}

IColorResItem* ColorManager::GetColorItemInfo(int nIndex)
{
	ColorResItem* pItem = this->m_resColor.GetColorItem(nIndex);
	if (nullptr == pItem)
		return nullptr;

    return pItem->GetIColorResItem();
}


bool ColorManager::ChangeSkinHLS(short h, short l, short s, int nFlag)
{
	return m_resColor.ChangeSkinHLS(h,l,s,nFlag);
}

ColorRes&  ColorManager::GetColorRes()
{
	return m_resColor;
}


//////////////////////////////////////////////////////////////////////////

HRESULT  ColorManager::UIParseColorTagCallback(IUIElement* pElem, ISkinRes* pSkinRes)
{
    IColorManager&  pColorMgr = pSkinRes->GetColorManager();
    return pColorMgr.GetImpl()->ParseNewElement(pElem->GetImpl());
}

HRESULT  ColorManager::ParseNewElement(UIElement* pElem)
{
	ColorTagElementInfo info;
	info.pXmlElement = pElem;
	pElem->AddRef();

	CComBSTR bstrId;
	if (pElem->GetAttrib(XML_ID, &bstrId))
	{
		info.strId = bstrId;
	}

	m_listUIElement.Add(info);

    // 遍历其子元素
    UIElementProxy childElement = pElem->FirstChild();
    while (childElement)
    {
        this->OnNewChild(childElement.get());
        childElement = childElement->NextElement();
    }

    return S_OK;
}
void  ColorManager::OnNewChild(UIElement* pElem)
{
    //	加载所有属性
    IMapAttribute* pMapAttrib = nullptr;
    pElem->GetAttribList(&pMapAttrib);

    if (false == m_resColor.LoadItem(pMapAttrib, pElem->GetData()))
    {
        UI_LOG_WARN(_T("insert color failed."));
    }

    SAFE_RELEASE(pMapAttrib);
}