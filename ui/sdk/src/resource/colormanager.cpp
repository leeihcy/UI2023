#include "colormanager.h"
#include "include/interface/iuires.h"
#include "src/skin_parse/xml/xmlwrap.h"
#include "include/interface/ixmlwrap.h"
#include "include/interface/imapattr.h"
#include "include/util/log.h"

namespace ui
{
ColorManager::ColorManager(ResBundle* pSkinRes): m_resColor(pSkinRes)
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

long  ColorManager::UIParseColorTagCallback(IUIElement* pElem, IResBundle* pSkinRes)
{
    IColorManager&  pColorMgr = pSkinRes->GetColorManager();
    return pColorMgr.GetImpl()->ParseNewElement(pElem->GetImpl());
}

long  ColorManager::ParseNewElement(UIElement* pElem)
{
	ColorTagElementInfo info;
	info.pXmlElement = pElem;
	pElem->AddRef();

	std::wstring bstrId = pElem->GetAttrib(XML_ID);
	if (!bstrId.empty())
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

    return 0;
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
}