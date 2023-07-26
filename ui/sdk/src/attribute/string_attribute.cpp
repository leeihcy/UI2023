#include "include/inc.h"
#include "string_attribute.h"
#include "attribute.h"
#include "src/resource/i18nres.h"
#include "src/resource/res_bundle.h"
#include "src/application/uiapplication.h"
#include "attributex64.h"


namespace ui
{
AttributeBase*  CreateStringAttribute()
{
    return new StringAttribute();
}

// TODO: 
// ע��Ϊ��֧��i18n�ֶεı༭���ܣ����ò���������һ���洢�ռ������string id
//    ��Ϊ�˾��������ƻ��Ǳ༭ģʽ�µĴ����߼���Ŀǰ�������ǽ�set�����޸�Ϊ
//    set(const wchar_t* text, int size)
//    �ڱ༭ģʽ�£���text��Ϊ���Σ�ǰһ�α�ʾת��֮������ݣ���һ�����ݱ�ʾ
//    string id���м���\0�ָ���
//    ���ַ�ʽǿ���ⲿ��Ҫ����һ���µĺ��������Ǻܺã���û�뵽���õİ취 

AttributeBase*  CreateI18nStringAttribute()
{
    return new I18nStringAttribute();
}

StringAttribute::StringAttribute()
{
    m_pIStringAttribute = nullptr;
    m_pBindValue = nullptr;
    // _this = nullptr;
    // _getter = nullptr;
    // _setter = nullptr;
}

StringAttribute::~StringAttribute()
{
    SAFE_DELETE(m_pIStringAttribute);
}

IStringAttribute*  StringAttribute::GetIStringAttribute()
{
    if (!m_pIStringAttribute)
        m_pIStringAttribute = new IStringAttribute(this);

    return m_pIStringAttribute;
}

void  StringAttribute::BindReference(String& s)
{
    m_pBindValue = &s;
}

// ��������£�Ĭ��ֵΪnullptr
// void  StringAttribute::BindFunction(
//         void* _this, pfnStringSetter s, pfnStringGetter g)
// {
//     this->_this = _this;
//     this->_setter = s;
//     this->_getter = g;
// }

void StringAttribute::Bind(slot<void(const wchar_t*)>&& s, slot<const wchar_t*()>&& g)
{
    m_setter.connect(std::forward<slot<void(const wchar_t*)>>(s));
    m_getter.connect(std::forward<slot<const wchar_t*()>>(g));
}
// void  StringAttribute::BindFunction(
//         const std::function<void(const wchar_t*)>& s, 
//         const std::function<const wchar_t*()>& g)
// {
//     _func_setter = s;
//     _func_getter = g;
// }

const wchar_t*  StringAttribute::Get()
{
	const wchar_t* szValue = nullptr;
    if (m_pBindValue)
    {
		szValue = m_pBindValue->c_str();
    }
//     else if (_this && _getter)
//     {
//         const wchar_t*  szReturn = nullptr;
// #ifdef _WIN64
// 		szReturn = UIx64::GetString(_this, _getter);
// #else
// 		long  localVarThis = (long)_this;
// 		long  localVarGetter = (long)_getter;

// 		__asm
// 		{
// 			mov  ecx, localVarThis;  // ����this
// 			call localVarGetter;     // ����
// 			mov  szReturn, eax;      // ��ȡ����ֵ
// 		}
// #endif
// 		szValue = szReturn;
//     }
    //else if (!_func_getter._Empty())
	else if (!m_getter.empty())
	{
        szValue = m_getter.emit();
    }


	return szValue;
}

void  StringAttribute::Set(const wchar_t* szValue)
{
    if (m_pBindValue)
    {
        if (szValue)
            m_pBindValue->assign(szValue);
        else
            m_pBindValue->clear();
    }
//     else if (_this && _setter)
//     {
// #ifdef _WIN64
// 		UIx64::SetString(_this, szValue, _setter);
// #else
// 		long  localVarThis = (long)_this;
// 		long  localVarSetter = (long)_setter;

// 		__asm
// 		{
// 			mov eax, dword ptr [szValue] // ѹ�����
// 			push eax;

// 			mov  ecx, localVarThis;   // ����this
// 			call localVarSetter;      // ����
// 		}
// #endif
//     }
    // else if (!_func_setter._Empty())
	else if (!m_setter.empty())
	{
        m_setter.emit(szValue);
    }
}


void  StringAttribute::Reset()
{
	Set(GetDefault());
}

bool  StringAttribute::IsDefaultValue()
{
    const wchar_t* szValue = Get();
	const wchar_t* szDefault = GetDefault();

    if (m_strDefault.empty())
    {
        if (!szValue || !szValue[0])
            return true;
    }

	if (szValue && m_strDefault == szValue)
		return true;

    return false;
}

StringAttribute*  StringAttribute::SetDefault(const wchar_t* szValue)
{
	if (szValue)
	{
		m_strDefault = szValue;
	}
	else
	{
		m_strDefault.clear();
	}

	return this;
}

const wchar_t*  StringAttribute::GetDefault()
{
	return m_strDefault.c_str();
}

void StringAttribute::Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e)
{
    p->String2Editor(this, e);
}

// void  StringAttribute::Internationalization()
// {
// 	m_bI18n = true;
// }



const wchar_t*  I18nStringAttribute::Get()
{
    const wchar_t* szValue = StringAttribute::Get();

    if (szValue)
    {
        if (GetUIApplication()->IsEditorMode())
        {
            // ȡ��ԭʼ�ַ���id
            const wchar_t* szBuffer = szValue;
            int nLength1 = (int)wcslen(szValue);
            if (nLength1)
                szValue = szBuffer + (nLength1 + 1);
        }
    }

    return szValue;
}

void  I18nStringAttribute::Set(const wchar_t* szValue)
{
    int nSize = 0;
    if (szValue)
    {
        int nLength1 = (int)wcslen(szValue);
        int nLength2 = 0;

        const wchar_t* szValue2 = m_pSkinRes->GetI18nRes().MapConfigValue(szValue);
        if (szValue2)
            nLength2 = (int)wcslen(szValue2);

        int length1 = (nLength1);
        int length2 = (nLength2);

        // �ڱ༭ģʽ���棬���ܽ�ԭʼ��id��������˱��浽ת�����ַ������棬��Getʱ����
        // ȡ������
        if (GetUIApplication()->IsEditorMode())
        {
            wchar_t* szBuffer = GetTempBuffer(nLength1 + nLength2 + 2);
            char* p = (char*)szBuffer;

            int nSize1 = sizeof(wchar_t)*(length1 + 1);
            int nSize2 = sizeof(wchar_t)*(length2 + 1);
            memcpy(p, szValue2, nSize2);
            memcpy(p + nSize2, szValue, nSize1);
            szValue = szBuffer;

            nSize = length1 + length1 + 2;
        }
        else
        {
            szValue = szValue2;
            nSize = length1;
        }
    }

    if (m_pBindValue)
    {
        if (szValue)
            m_pBindValue->assign(szValue, nSize);
        else
            m_pBindValue->clear();
    }
#if 0 // x64TODO
    else if (_this && _setter_ex)
    {
        long  localVarThis = (long)_this;
        long  localVarSetter = (long)_setter_ex;

        __asm
        {
            mov eax, nSize;
            push eax;
            mov eax, dword ptr[szValue]; // ѹ�����
            push eax;

            mov  ecx, localVarThis;   // ����this
            call localVarSetter;      // ����
        }
    }
#endif
    //else if (!_func_setter_ex._Empty())
	else if (!m_setter_ex.empty())
    {
        m_setter_ex.emit(szValue, nSize);
    }
}

// ��������£�Ĭ��ֵΪnullptr
// void  I18nStringAttribute::BindFunctionEx(
//         void* _this, pfnStringExSetter s, pfnStringGetter g)
// {
//     this->_this = _this;
//     this->_setter_ex = s;
//     this->_getter = g;
// }

void  I18nStringAttribute::BindEx(
    slot<void(const wchar_t*, int)>&& s,
    slot<const wchar_t*()>&& g)
{
    m_setter_ex.connect(std::forward<slot<void(const wchar_t*, int)>>(s));
    m_getter.connect(std::forward<slot<const wchar_t*()>>(g));
}

}