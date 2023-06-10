#include "stdafx.h"
#include "bool_attribute.h"
#include "attribute.h"
#include "attributex64.h"

using namespace UI;

namespace UI
{
    AttributeBase*  CreateBoolAttribute()
    {
        return new BoolAttribute();
    }
}

BoolAttribute::BoolAttribute() 
{
    m_pIBoolAttribute = nullptr;
    m_pBindValue = nullptr;
    m_bDefault = false;

    _this = nullptr;
    _setter = nullptr;
    _getter = nullptr;
}

BoolAttribute::~BoolAttribute()
{
    SAFE_DELETE(m_pIBoolAttribute);
}
IBoolAttribute*  BoolAttribute::GetIBoolAttribute()
{
    if (!m_pIBoolAttribute)
        m_pIBoolAttribute = new IBoolAttribute(this);

    return m_pIBoolAttribute;
}

void  BoolAttribute::SetBindValue(void* p)
{
    m_pBindValue = (bool*)p;
}

void  BoolAttribute::SetBindFuction(void* _this, void* _setter, void* _getter)
{
    this->_this = _this;
    this->_setter = (pfnBoolSetter)_setter;
    this->_getter = (pfnBoolGetter)_getter;
}

BoolAttribute*  BoolAttribute::SetDefault(bool b)
{
	m_bDefault = b;
	return this;
}

//
// 注：不能直接写：b = _getter();
//     这样的结果是将BoolAttribute的this又mov到ecx了
//
bool  BoolAttribute::GetBool()
{
    if (m_pBindValue)
    {
        return (*m_pBindValue);
    }
    else if (_getter && _this)
    {
        bool bReturn = m_bDefault;
#ifdef _WIN64
		bReturn = UIx64::GetBool(_this, _getter);
#else
        long localVarThis = (long)_this;
        long localVarGetter = (long)_getter;

        __asm
        {
            mov ecx, localVarThis;  // 设置this
            call localVarGetter;    // 调用
            mov bReturn, al;        // 获取返回值
        }
#endif
        return bReturn;
    }
    else
    {
        return m_bDefault;
    }
}
void  BoolAttribute::SetBool(bool b)
{
    if (m_pBindValue)
    {
        (*m_pBindValue) = b;
    }
    else if (_setter && _this)
    {
#ifdef _WIN64
		UIx64::SetBool(_this, b, _setter);
#else
        long localVarThis = (long)_this;
        long localVarSetter = (long)_setter;

        __asm
        {
            movzx eax, byte ptr [b]   // 压入参数
            push eax;

            mov ecx, localVarThis;   // 设置this
            call localVarSetter;     // 调用
        }
#endif
    }
}
LPCTSTR  BoolAttribute::Get()
{
    bool b = GetBool();

	TCHAR* szText = GetTempBuffer(64);
    _tcscpy(szText, b ? XML_BOOL_VALUE_1 : XML_BOOL_VALUE_0);
    
    return szText;
}

void  BoolAttribute::Set(LPCTSTR szValue)
{
    bool b = false;
    if (0 == _tcscmp(szValue, XML_BOOL_VALUE_TRUE)  || 
        0 == _tcscmp(szValue, XML_BOOL_VALUE_1)     ||
        0 == _tcscmp(szValue, XML_BOOL_VALUE_YES))
    {
        b = true; 
    }
//  else if (0 == _tcscmp(szValue, XML_BOOL_VALUE_FALSE) ||
//      0 == _tcscmp(szValue, XML_BOOL_VALUE_0)      ||
//      0 == _tcscmp(szValue, XML_BOOL_VALUE_NO))
//  {
//      b = false;
//  }

    SetBool(b);
}

void  BoolAttribute::Reset()
{
	SetBool(m_bDefault);
}

bool  BoolAttribute::IsDefaultValue()
{
    return GetBool() == m_bDefault;
}

void  BoolAttribute::Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e)
{
    p->Bool2Editor(this, e);
}
