#include "stdafx.h"
#include "renderbase.h"
#include "Src\Base\Object\object.h"
#include "Src\Base\Attribute\attribute.h"
#include "Inc\Interface\iuires.h"
#include "Src\Resource\skinres.h"
#include "Src\Base\Application\uiapplication.h"
#include "Inc\Interface\renderlibrary.h"
#include "Src\UIObject\Window\windowbase.h"
#include "Src\Layer\windowrender.h"
#pragma comment(lib, "uxtheme.lib")

using namespace UI;


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

SkinRes*  RenderBase::GetSkinRes()
{
    if (!m_pObject)
        return nullptr;

    return m_pObject->GetSkinRes();
}
ColorRes*  RenderBase::GetSkinColorRes()
{
    SkinRes* pSkinRes = GetSkinRes();
    if (!pSkinRes)
        return nullptr;

    return &pSkinRes->GetColorRes();
}

ImageRes*  RenderBase::GetSkinImageRes()
{
    SkinRes* pSkinRes = GetSkinRes();
    if (!pSkinRes)
        return nullptr;

    return &pSkinRes->GetImageRes();
}

void  RenderBase::CheckThemeChanged()
{
    if (this->IsThemeRender())
    {
        UISendMessage(static_cast<IMessage*>(m_pIRenderBase), WM_THEMECHANGED);
    }
}

void  RenderBase::_LoadColor(LPCTSTR szColorId, Color*& pColorRef)
{
	SAFE_RELEASE(pColorRef);
	if (!szColorId)
		return;

	ColorRes* pColorRes = GetSkinColorRes();
	if (!pColorRes)
		return;

	pColorRes->GetColor(szColorId, &pColorRef);
}
LPCTSTR  RenderBase::_GetColorId(Color*& pColorRef)
{
	if (!pColorRef)
		return nullptr;

	ColorRes* pColorRes = GetSkinColorRes();
	if (pColorRes)
	{
		LPCTSTR szId = pColorRes->GetColorId(pColorRef);
		if (szId)
			return szId;
	}

	TCHAR* szBuffer = GetTempBuffer();
	pColorRef->ToWebString(szBuffer);
	return szBuffer;
}

void  RenderBase::_LoadBitmap(LPCTSTR szBitmapId, IRenderBitmap*& pBitmapRef)
{
	SAFE_RELEASE(pBitmapRef);
	if (!szBitmapId)
		return;


	if (m_pObject)
	{
		SkinRes* pSkinRes = m_pObject->GetSkinRes();
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
}

LPCTSTR  RenderBase::_GetBitmapId(IRenderBitmap*& pBitmapRef)
{
	if (!pBitmapRef)
		return nullptr;

	ImageRes* pImageRes = GetSkinImageRes();
	if (!pImageRes)
		return nullptr;

	return pImageRes->GetRenderBitmapId(pBitmapRef);
}

