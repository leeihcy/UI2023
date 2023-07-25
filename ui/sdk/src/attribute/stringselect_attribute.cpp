#include "include/inc.h"
#include "stringselect_attribute.h"
#include "attribute.h"
#include "include/interface/iuiapplication.h"
#include "src/application/uiapplication.h"


namespace ui
{
    AttributeBase*  CreateStringEnumAttribute()
    {
        return new StringEnumAttribute();
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

StringEnumAttribute*  StringEnumAttribute::Add(const wchar_t* szText)
{
    if (IsExist(szText))
        return this;

    if (szText)
        m_list.push_back(String(szText));

    return this;
}

bool  StringEnumAttribute::IsExist(const wchar_t* szText)
{
    if (!szText)
        return false;

    std::list<String>::iterator iter = m_list.begin();
    for (; iter != m_list.end(); ++iter)
    {
        if ((*iter) == szText)
            return true;
    }

    return false;
}

void  StringEnumAttribute::EnumString(pfnEnumStringEnumCallback callback, long w, long l)
{
    std::list<String>::iterator iter = m_list.begin();
    for (; iter != m_list.end(); ++iter)
    {
        callback((*iter).c_str(), w, l);
    }
    return;
}

const wchar_t*  StringEnumAttribute::Get()
{
    const wchar_t* szValue = StringAttribute::Get();
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

void  StringEnumAttribute::Set(const wchar_t* szValue)
{
    if (!m_bEditable && !IsExist(szValue))
    {
        return StringAttribute::Set(m_strDefault.c_str());
    }

    StringAttribute::Set(szValue);
}

void  EnumRenderBaseNameCallback(const wchar_t* szText, long wParam, long lParam)
{
    StringEnumAttribute* pThis = (StringEnumAttribute*)wParam;
    pThis->Add(szText);
}
StringEnumAttribute*  StringEnumAttribute::FillRenderBaseTypeData()
{
    m_list.clear();
    this->Add(EMPTYTEXT);  // 增加一项默认值，取消当前的选项
    m_pUIApplication->GetIUIApplication()->EnumRenderBaseName(EnumRenderBaseNameCallback, (long)this, 0);
    return this;
}

void  EnumTextRenderBaseNameCallback(const wchar_t* szText, long wParam, long lParam)
{
    StringEnumAttribute* pThis = (StringEnumAttribute*)wParam;
    pThis->Add(szText);
}
StringEnumAttribute*  StringEnumAttribute::FillTextRenderBaseTypeData()
{
    m_list.clear();
	m_pUIApplication->GetIUIApplication()->EnumTextRenderBaseName(EnumTextRenderBaseNameCallback, (long)this, 0);
    return this;
}

void  StringEnumAttribute::Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e)
{
    p->StringEnum2Editor(this, e);
}

}