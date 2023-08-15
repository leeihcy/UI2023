#include "include/inc.h"
#include "enum_attribute.h"
#include "attribute.h"
#include "src/application/uiapplication.h"


namespace ui
{
    AttributeBase*  CreateEnumAttribute()
    {
        return new EnumAttribute();
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

EnumAttribute*  EnumAttribute::AddOption(long l, const wchar_t* sz)
{
    LongAttribute::AddAlias(l, sz);
    return this;
}

void  EnumAttribute::Set(const wchar_t* szValue)
{
    long lValue = m_lDefault;

    if (m_mapAlias.GetAlias(szValue, &lValue))
    {
        SetLong(lValue);
		return;
    }

	// 如果szValue是一个数字，则取出来。否则用默认值覆盖
	bool isNumber = false;
	const wchar_t* p = szValue;
	wchar_t c = 0;
	while ((c = *p++))
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
		SetLong(util::wtoi(szValue));
	}
	else
	{
		SetLong(m_lDefault);
	}
}

const wchar_t*  EnumAttribute::Get()
{
    long lValue = GetLong();

    const wchar_t* szAlias = m_mapAlias.GetAlias(lValue);
    if (szAlias)
    {
        return szAlias;
    }
    else
    {
        return nullptr;
    }
}

namespace ui
{
// void  EnumGraphicsRenderLibraryType(
// 		pfnEnumGraphicsRenderLibraryTypeCallback callback, 
// 		long w, long l);
}

void EnumGraphicsRenderLibraryTypeCallback(long lType, const wchar_t* szName, long wParam, long)
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
	// EnumGraphicsRenderLibraryType(EnumGraphicsRenderLibraryTypeCallback, (long)this, 0);
	this->AddOption(GRAPHICS_RENDER_LIBRARY_TYPE_GDI, L"gdi");
	return this;
}

void  EnumAttribute::Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e)
{
	p->Enum2Editor(this, e);
}
}