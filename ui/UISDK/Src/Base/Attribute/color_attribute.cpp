#include "stdafx.h"
#include "color_attribute.h"
#include "attribute.h"
#include "..\Application\uiapplication.h"
#include "..\Object\object.h"
#include "Src\Resource\skinres.h"


namespace UI
{
    AttributeBase*  CreateColorAttribute()
    {
        return new ColorAttribute();
    }
}

ColorAttribute::ColorAttribute() 
{
    m_pIColorAttribute = nullptr;

    m_ppBindValue = nullptr;
	m_pBindValue = nullptr;
//     _this = nullptr;
//     _setter = nullptr;
//     _getter = nullptr;
}

ColorAttribute::~ColorAttribute()
{
	SAFE_DELETE(m_pIColorAttribute);
}

void  ColorAttribute::SetBindValue(void* p)
{
    m_pBindValue = (Color*)p;
}
void  ColorAttribute::SetBindValue2(void** p)
{
	m_ppBindValue = (Color**)p;
}

void  ColorAttribute::Set(LPCTSTR szColorId)
{
	if (!m_pSkinRes)
    {
        UIASSERT(0);
        return;
    }

	if (m_ppBindValue)
	{
		SAFE_RELEASE(*m_ppBindValue);
		if (!szColorId)
			return;

		ColorRes& colorRes = m_pSkinRes->GetColorRes();
		colorRes.GetColor(szColorId, m_ppBindValue);

#ifdef EDITOR_MODE
        if (*m_ppBindValue)
        {
            (*m_ppBindValue)->SetResId(szColorId);
        }
#endif
	}
	else if (m_pBindValue)
	{
		if (!szColorId)
			return;

		ColorRes& colorRes = m_pSkinRes->GetColorRes();
		Color* pColor = nullptr;
		colorRes.GetColor(szColorId, &pColor);
		if (pColor)
		{
			*m_pBindValue = *pColor;
			SAFE_RELEASE(pColor);
		}

#ifdef EDITOR_MODE
		if (m_ppBindValue)
		{
			m_pBindValue->SetResId(szColorId);
		}
#endif
	}
}

void  ColorAttribute::Reset()
{
	if (m_ppBindValue)
	{
		SAFE_RELEASE(*m_ppBindValue);
	}
	else if (m_pBindValue)
	{

	}
}

LPCTSTR  ColorAttribute::Get()
{
    if (m_ppBindValue && *m_ppBindValue)
	{
	#ifdef EDITOR_MODE
		LPCTSTR szText = (*m_ppBindValue)->GetResId();
		if (szText)
			return szText;
	#endif

		LPTSTR szBuffer = GetTempBuffer();
		(*m_ppBindValue)->ToWebString(szBuffer);
		return szBuffer;
	}
	else if (m_pBindValue)
	{
#ifdef EDITOR_MODE
		LPCTSTR szText = m_pBindValue->GetResId();
		if (szText)
			return szText;
#endif

		LPTSTR szBuffer = GetTempBuffer();
		m_pBindValue->ToWebString(szBuffer);
		return szBuffer;
	}

	return nullptr;
}

bool  ColorAttribute::IsDefaultValue()
{
	if (m_ppBindValue)
	{
		if (*m_ppBindValue == nullptr)
			return true;
	}
	else if (m_pBindValue)
	{

	}

	return false;
}

void  ColorAttribute::Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e)
{
	p->Color2Editor(this, e);
}

IColorAttribute*  ColorAttribute::GetIColorAttribute()
{
    if (!m_pIColorAttribute)
        m_pIColorAttribute = new IColorAttribute(this);

    return m_pIColorAttribute;
}
