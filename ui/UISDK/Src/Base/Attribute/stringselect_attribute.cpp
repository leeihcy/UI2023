#include "stdafx.h"
#include "stringselect_attribute.h"
#include "attribute.h"
#include "Inc/Interface/iuiapplication.h"
#include "Src/Base/Application/uiapplication.h"

using namespace UI;

namespace UI
{
    AttributeBase*  CreateStringEnumAttribute()
    {
        return new StringEnumAttribute();
    }
}

StringEnumAttribute::StringEnumAttribute()
{
    m_pIStringEnumAttribute = nullptr;
    m_bEditable = false;
}

StringEnumAttribute::~StringEnumAttribute()
{
    SAFE_DELETE(m_pIStringEnumAttribute);
}

IStringEnumAttribute*  StringEnumAttribute::GetIStringEnumAttribute()
{
    if (!m_pIStringEnumAttribute)
        m_pIStringEnumAttribute = new IStringEnumAttribute(this);

    return m_pIStringEnumAttribute;
}

StringEnumAttribute*  StringEnumAttribute::Add(LPCTSTR szText)
{
    if (IsExist(szText))
        return this;

    if (szText)
        m_list.push_back(String(szText));

    return this;
}

bool  StringEnumAttribute::IsExist(LPCTSTR szText)
{
    if (!szText)
        return false;

    list<String>::iterator iter = m_list.begin();
    for (; iter != m_list.end(); ++iter)
    {
        if ((*iter) == szText)
            return true;
    }

    return false;
}

void  StringEnumAttribute::EnumString(pfnEnumStringEnumCallback callback, WPARAM w, LPARAM l)
{
    list<String>::iterator iter = m_list.begin();
    for (; iter != m_list.end(); ++iter)
    {
        callback((*iter).c_str(), w, l);
    }
    return;
}

LPCTSTR  StringEnumAttribute::Get()
{
    LPCTSTR szValue = __super::Get();
    if (!szValue)
        return nullptr;

    if (m_bEditable)
    {
        return szValue;
    }
    else
    {
        if (IsExist(szValue))
            return szValue;
        else
            return m_strDefault.c_str();
    }
}

void  StringEnumAttribute::Set(LPCTSTR szValue)
{
    if (!m_bEditable && !IsExist(szValue))
    {
        return __super::Set(m_strDefault.c_str());
    }

    __super::Set(szValue);
}

void  EnumRenderBaseNameCallback(LPCTSTR szText, WPARAM wParam, LPARAM lParam)
{
    StringEnumAttribute* pThis = (StringEnumAttribute*)wParam;
    pThis->Add(szText);
}
StringEnumAttribute*  StringEnumAttribute::FillRenderBaseTypeData()
{
    m_list.clear();
    this->Add(EMPTYTEXT);  // 增加一项默认值，取消当前的选项
    m_pUIApplication->GetIUIApplication()->EnumRenderBaseName(EnumRenderBaseNameCallback, (WPARAM)this, 0);
    return this;
}

void  EnumTextRenderBaseNameCallback(LPCTSTR szText, WPARAM wParam, LPARAM lParam)
{
    StringEnumAttribute* pThis = (StringEnumAttribute*)wParam;
    pThis->Add(szText);
}
StringEnumAttribute*  StringEnumAttribute::FillTextRenderBaseTypeData()
{
    m_list.clear();
	m_pUIApplication->GetIUIApplication()->EnumTextRenderBaseName(EnumTextRenderBaseNameCallback, (WPARAM)this, 0);
    return this;
}

void  StringEnumAttribute::Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e)
{
    p->StringEnum2Editor(this, e);
}

