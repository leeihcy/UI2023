#include "stdafx.h"
#include "enum_attribute.h"
#include "attribute.h"
#include "Src\Base\Application\uiapplication.h"

using namespace UI;

namespace UI
{
    AttributeBase*  CreateEnumAttribute()
    {
        return new EnumAttribute();
    }
}

EnumAttribute::EnumAttribute()
{
    m_pIEnumAttribute = nullptr;
}
EnumAttribute::~EnumAttribute()
{
    SAFE_DELETE(m_pIEnumAttribute);
}
IEnumAttribute*  EnumAttribute::GetIEnumAttribute()
{
    if (!m_pIEnumAttribute)
        m_pIEnumAttribute = new IEnumAttribute(this);

    return m_pIEnumAttribute;
}

EnumAttribute*  EnumAttribute::AddOption(long l, LPCTSTR sz)
{
    __super::AddAlias(l, sz);
    return this;
}

void  EnumAttribute::Set(LPCTSTR szValue)
{
    long lValue = m_lDefault;

    if (m_mapAlias.GetAlias(szValue, &lValue))
    {
        SetLong(lValue);
		return;
    }

	// 如果szValue是一个数字，则取出来。否则用默认值覆盖
	bool isNumber = false;
	LPCTSTR p = szValue;
	TCHAR c = 0;
	while (c = *p++)
	{
		if (c >= 0x30 && c < 0x39)
		{
			isNumber = true;
			continue;
		}
		else
		{
			isNumber = false;
			break;
		}
	}

	if (isNumber)
	{
		SetLong(_ttoi(szValue));
	}
	else
	{
		SetLong(m_lDefault);
	}
}

LPCTSTR  EnumAttribute::Get()
{
    long lValue = GetLong();

    LPCTSTR szAlias = m_mapAlias.GetAlias(lValue);
    if (szAlias)
    {
        return szAlias;
    }
    else
    {
        return nullptr;
    }
}

bool EnumLayoutTypeCallback(long lType, LPCTSTR szName, WPARAM wParam, LPARAM)
{
    EnumAttribute* pThis = (EnumAttribute*)wParam;
    pThis->AddOption(lType, szName);

    return true;
}

// 填充可选的布局类型：
// "canvas" LAYOUT_TYPE_CANVAS
// ...
EnumAttribute*  EnumAttribute::FillLayoutTypeData()
{
    UIApplication*  pUIApp = GetUIApplication();
    if (pUIApp)
	    pUIApp->GetIUIApplication()->EnumLayoutType(EnumLayoutTypeCallback, (WPARAM)this, 0);

	return this;
}

namespace UI
{
// void  EnumGraphicsRenderLibraryType(
// 		pfnEnumGraphicsRenderLibraryTypeCallback callback, 
// 		WPARAM w, LPARAM l);
}

void EnumGraphicsRenderLibraryTypeCallback(long lType, LPCTSTR szName, WPARAM wParam, LPARAM)
{
	EnumAttribute* pThis = (EnumAttribute*)wParam;
	pThis->AddOption(lType, szName);
}

//
// 填充可用的渲染库类型：
// "gdi"  GRAPHICS_RENDER_LIBRARY_GDI
//  ...
EnumAttribute*  EnumAttribute::FillGraphicsRenderLibraryData()
{
	// EnumGraphicsRenderLibraryType(EnumGraphicsRenderLibraryTypeCallback, (WPARAM)this, 0);
	this->AddOption(GRAPHICS_RENDER_LIBRARY_TYPE_GDI, L"gdi");
	return this;
}

void  EnumAttribute::Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e)
{
	p->Enum2Editor(this, e);
}