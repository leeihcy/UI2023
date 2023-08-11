#include "include/inc.h"
#include "src/private_inc.h"
#include "textrender_factory.h"
#include "src/render/textrender/textrender.h"
#include "src/application/uiapplication.h"

namespace ui {
TextRenderFactory::TextRenderFactory(Application& app):m_app(app)
{

}
TextRenderFactory::~TextRenderFactory()
{
	Clear();
}

void  TextRenderFactory::Init()
{
	
#define REGISTER_UI_TEXTRENDERBASE2(classname) \
        this->RegisterUITextRenderBaseCreateData( \
                classname::GetXmlName(),          \
                classname::GetType(),             \
				(ui::pfnUICreateTextRenderBasePtr)ObjectCreator<I##classname>::CreateInstance2);

	REGISTER_UI_TEXTRENDERBASE2(SimpleTextRender)
	REGISTER_UI_TEXTRENDERBASE2(ColorListTextRender)
	REGISTER_UI_TEXTRENDERBASE2(FontColorListTextRender)
	REGISTER_UI_TEXTRENDERBASE2(ContrastColorTextRender)
	REGISTER_UI_TEXTRENDERBASE2(ContrastColorListTextRender)
}

void  TextRenderFactory::Clear()
{
	UITEXTRENDERBASE_CREATE_DATA::iterator iter = m_vecUITextRenderBaseCreateData.begin();
	for ( ; iter != m_vecUITextRenderBaseCreateData.end(); ++iter)
	{
		SAFE_DELETE(*iter);               
	}
	m_vecUITextRenderBaseCreateData.clear();     
}


bool  TextRenderFactory::RegisterUITextRenderBaseCreateData(
        const wchar_t* szName, 
        int nType, 
        pfnUICreateTextRenderBasePtr pfunc)
{
	if (nullptr == szName || nullptr == pfunc)
		return false;

	String strName(szName);

	UITEXTRENDERBASE_CREATE_INFO* pInfo = new UITEXTRENDERBASE_CREATE_INFO;
	pInfo->m_func = pfunc;
	pInfo->m_nRenderType = nType;
	pInfo->m_strName = strName;
	m_vecUITextRenderBaseCreateData.push_back(pInfo);

	//UI_LOG_DEBUG(_T("%s   @ 0x%08X"), szName,  pfunc);
	return true;
}

bool  TextRenderFactory::CreateTextRenderBaseByName(
        IResBundle* pSkinRes, 
        const wchar_t* bstrName, 
        IObject* pObject, 
        ITextRenderBase** ppOut)
{
    UIASSERT(pObject);
	if (!bstrName || !bstrName[0] || !pObject || !ppOut)
		return false;

	UITEXTRENDERBASE_CREATE_DATA::iterator iter = m_vecUITextRenderBaseCreateData.begin();
	for (; iter != m_vecUITextRenderBaseCreateData.end(); ++iter)
	{
		UITEXTRENDERBASE_CREATE_INFO* pData = *iter;
		if (nullptr == pData)
			continue;

		if (pData->m_strName != bstrName)
			continue;

		pData->m_func(pSkinRes, (void**)ppOut);
		if (*ppOut)
		{
			(*ppOut)->AddRef();
			(*ppOut)->SetObject(pObject);
			(*ppOut)->Init();
			(*ppOut)->SetType((TEXTRENDER_TYPE)pData->m_nRenderType);
			return true;
		}

		return false;
	}

	UI_LOG_WARN(_T("Create Failed. Name=%s"), bstrName);
	return false;
}
bool  TextRenderFactory::CreateTextRender(
        IResBundle* pSkinRes, 
        int nType, 
        IObject* pObject, 
        ITextRenderBase** ppOut)
{
    UIASSERT(pObject);
	if (nullptr == pObject || nullptr == ppOut)
		return false;

	UITEXTRENDERBASE_CREATE_DATA::iterator iter = m_vecUITextRenderBaseCreateData.begin();
	for (; iter != m_vecUITextRenderBaseCreateData.end(); ++iter)
	{
		UITEXTRENDERBASE_CREATE_INFO* pData = *iter;
		if (nullptr == pData)
			continue;

		if (pData->m_nRenderType != nType)
			continue;

		pData->m_func(pSkinRes, (void**)ppOut);
		if (*ppOut)
		{
			(*ppOut)->AddRef();
			(*ppOut)->SetObject(pObject);
			(*ppOut)->Init();
			(*ppOut)->SetType((TEXTRENDER_TYPE)nType);
			return true;
		}

		return false;
	}

	UI_LOG_WARN(_T("Create Failed. Type=%d"), nType);
	return false;
}

const wchar_t*  TextRenderFactory::GetTextRenderBaseName(int nType)
{
    UITEXTRENDERBASE_CREATE_DATA::iterator iter = m_vecUITextRenderBaseCreateData.begin();
	for (; iter != m_vecUITextRenderBaseCreateData.end(); ++iter)
	{
		UITEXTRENDERBASE_CREATE_INFO* pData = *iter;
		if (nullptr == pData)
			continue;

		if (pData->m_nRenderType != nType)
			continue;

		return pData->m_strName.c_str();
	}

	return nullptr;
}

void  TextRenderFactory::EnumTextRenderBaseName(
        pfnEnumTextRenderBaseNameCallback callback,
        long wParam, 
        long lParam)
{
	UITEXTRENDERBASE_CREATE_DATA::iterator iter = m_vecUITextRenderBaseCreateData.begin();
	for (; iter != m_vecUITextRenderBaseCreateData.end(); ++iter)
	{
		UITEXTRENDERBASE_CREATE_INFO* pData = *iter;
		if (nullptr == pData)
			continue;

		callback(pData->m_strName.c_str(), wParam, lParam);
	}
}

}