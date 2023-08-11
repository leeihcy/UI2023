#include "include/inc.h"
#include "renderbase.h"

#include "src/object/object.h"
#include "src/attribute/attribute.h"
#include "include/interface/iuires.h"
#include "src/resource/res_bundle.h"
#include "src/application/uiapplication.h"
#include "include/interface/renderlibrary.h"
// #include "src\UIObject\Window\windowbase.h"
// #include "src/Layer\windowrender.h"

#if defined(OS_WIN)
#pragma comment(lib, "uxtheme.lib")
#endif
using namespace ui;


RenderBase::RenderBase(IRenderBase* p) : Message(p)
{
    m_pIRenderBase = p;
	m_pObject = nullptr;
	m_nRenderType = RENDER_TYPE_NULL;
    m_pUIApplication = nullptr;
	m_lRef = 0;
}

long  RenderBase::AddRef()
{
	m_lRef++;
	return m_lRef;
}
long  RenderBase::Release()
{
	--m_lRef;
	if (0 >= m_lRef)
	{
		m_pIRenderBase->IMessage::Release();
		return 0;
	}

	return m_lRef;
}

ResBundle*  RenderBase::GetSkinRes()
{
    if (!m_pObject)
        return nullptr;

    return m_pObject->GetSkinRes();
}
ColorRes*  RenderBase::GetSkinColorRes()
{
    ResBundle* pSkinRes = GetSkinRes();
    if (!pSkinRes)
        return nullptr;

    return &pSkinRes->GetColorRes();
}

ImageRes*  RenderBase::GetSkinImageRes()
{
    ResBundle* pSkinRes = GetSkinRes();
    if (!pSkinRes)
        return nullptr;

    return &pSkinRes->GetImageRes();
}

void  RenderBase::CheckThemeChanged()
{
    if (this->IsThemeRender())
    {
        static_cast<IMessage*>(m_pIRenderBase)->SendMessage(WM_THEMECHANGED);
    }
}

void  RenderBase::_LoadColor(const wchar_t* szColorId, Color*& pColorRef)
{
	SAFE_RELEASE(pColorRef);
	if (!szColorId)
		return;

	ColorRes* pColorRes = GetSkinColorRes();
	if (!pColorRes)
		return;

	pColorRes->GetColor(szColorId, &pColorRef);
}
const wchar_t*  RenderBase::_GetColorId(Color*& pColorRef)
{
	if (!pColorRef)
		return nullptr;

	ColorRes* pColorRes = GetSkinColorRes();
	if (pColorRes)
	{
		const wchar_t* szId = pColorRes->GetColorId(pColorRef);
		if (szId)
			return szId;
	}

	wchar_t* szBuffer = GetTempBuffer();
	pColorRef->ToWebString(szBuffer);
	return szBuffer;
}

void  RenderBase::_LoadBitmap(const wchar_t* szBitmapId, IRenderBitmap*& pBitmapRef)
{
#if defined(OS_WIN)
	SAFE_RELEASE(pBitmapRef);
	if (!szBitmapId)
		return;


	if (m_pObject)
	{
		ResBundle* pSkinRes = m_pObject->GetSkinRes();
		if (!pSkinRes)
			return;

		GRAPHICS_RENDER_LIBRARY_TYPE eType = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
		WindowBase* pWnd = m_pObject->GetWindowObject();
		if (pWnd)
		{
			WindowRender* pRender = pWnd->GetWindowRender();
			if (pRender)
				eType = pRender->GetGraphicsRenderType();
		}

		pSkinRes->GetImageRes().GetBitmap(
			szBitmapId, 
			eType,
			&pBitmapRef);
	}
	else
	{
		ImageRes* pImageRes = GetSkinImageRes();
		if (!pImageRes)
			return;

		pImageRes->GetBitmap(szBitmapId, UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDI, &pBitmapRef); 
	}
#else
    UIASSERT(false);
#endif
}

const wchar_t*  RenderBase::_GetBitmapId(IRenderBitmap*& pBitmapRef)
{
	if (!pBitmapRef)
		return nullptr;

	ImageRes* pImageRes = GetSkinImageRes();
	if (!pImageRes)
		return nullptr;

	return pImageRes->GetRenderBitmapId(pBitmapRef);
}

