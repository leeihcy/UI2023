#include "stdafx.h"
#include "skinparseengine.h"
#include "Inc\Interface\ixmlwrap.h"
#include "Inc\Interface\iskindatasource.h"
#include "Src\Resource\skinres.h"
#include "Inc\Interface\iuires.h"
#include "xml\pugixmlwrap\pugixmlwrap.h"
#include "datasource\skindatasource.h"
#include "..\Base\Application\uiapplication.h"

using namespace UI;

SkinParseEngine::SkinParseEngine(SkinRes* pSkinRes)
{   
	UIASSERT(pSkinRes);
    m_pSkinRes = pSkinRes;
	m_pUIApplication = m_pSkinRes->GetUIApplication();
}

SkinParseEngine::~SkinParseEngine()
{
}

bool  SkinParseEngine::Parse(
			SkinDataSource* pDataSource, 
			LPCTSTR szXmlFile)
{
#if 0
	1. 查找 <skin> root element，如果没有找到则直接返回，表示这不是一个合法文件
		2. 遍历 <skin> 的child element

		2.1 获取tagName，例如image
		2.2 获取image标签对应的解析器 IImageParse
		2.3 调用IImageParse的NewElement
		2.4 IImageParse自己去负责遍历子结点
#endif
	if (!pDataSource || !szXmlFile)
		return false;

    UIDocument*  pUIDocument = nullptr;
    CreateXmlDocument(XML_ENGINE_DEFAULT, &pUIDocument);
	if (!pDataSource->Load_UIDocument(pUIDocument, szXmlFile))
	{
		SAFE_RELEASE(pUIDocument);
		return false;
	}
	pUIDocument->SetSkinPath(szXmlFile);	

	do 
	{
		UIElementProxy rootElem = pUIDocument->FindElem(XML_SKIN);
		if (!rootElem)
		{
			UI_LOG_ERROR(_T("Cannot find root element: %s"), XML_SKIN);
			break;
		}

		m_pSkinRes->OnNewUIDocument(pUIDocument);

		UIElementProxy childElem = rootElem->FirstChild();
		while (childElem)
		{
			this->NewChild(childElem.get());
			childElem = childElem->NextElement();
		}
	}
	while (0);

	SAFE_RELEASE(pUIDocument);	
	return true;
}

void  SkinParseEngine::NewChild(UIElement* pElement)
{
	if (!pElement)
		return;

    pfnParseSkinTag func;
    if (!m_pUIApplication->GetSkinTagParseFunc(
				(LPCWSTR)pElement->GetTagName(), &func))
	{
        return;
	}

    func(pElement->GetIUIElement(), m_pSkinRes->GetISkinRes());
}


HRESULT  SkinParseEngine::UIParseIncludeTagCallback(IUIElement* pElement, ISkinRes* pSkinRes)
{
    if (nullptr == pElement || nullptr == pSkinRes)
        return E_FAIL;

    LPCTSTR szData = pElement->GetData();
    if (!szData)
		return E_FAIL;

    SkinParseEngine  parse(pSkinRes->GetImpl());
	SkinDataSource*  pDataSource = pSkinRes->GetImpl()->GetDataSource();

    if (!parse.Parse(pDataSource, szData))
        return E_FAIL;

    return S_OK;
}

namespace UI
{
bool  CreateXmlDocument(XML_ENGINE e, UIDocument** pp)
{
	if (!pp)
		return false;

	*pp = nullptr;

    switch (e)
    {
//     case MSXML:
//         {
//             return MsXmlDocument::CreateInstance(pp);
//         }
//         break;

    case PUGIXML:
    case XML_ENGINE_DEFAULT:
        {
            UIDocument* p = new PugiXmlDocument;
			p->AddRef();
			*pp = p;
			return true;
        }
        break;
    }

    return false;
}
}