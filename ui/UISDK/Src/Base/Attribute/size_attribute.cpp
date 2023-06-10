#include "stdafx.h"
#include "size_attribute.h"
#include "attribute.h"
#include "attributex64.h"

using namespace UI;

namespace UI
{
    AttributeBase*  CreateSizeAttribute()
    {
        return new SizeAttribute();
    }
}

SizeAttribute::SizeAttribute() 
{
    m_pISizeAttribute = nullptr;
    m_pBindValue = nullptr;
    m_sizeDefault.cx = m_sizeDefault.cy = 0;

    _this = nullptr;
    _setter = nullptr;
    _getter = nullptr;
}

SizeAttribute::~SizeAttribute()
{
    SAFE_DELETE(m_pISizeAttribute);
}

void  SizeAttribute::SetBindValue(void* p)
{
    m_pBindValue = (SIZE*)p;
}

void  SizeAttribute::SetBindFuction(void* _this, void* _setter, void* _getter)
{
    this->_this = _this;
    this->_setter = (pfnSizeSetter)_setter;
    this->_getter = (pfnSizeGetter)_getter;
}

SizeAttribute*  SizeAttribute::SetDefault(LPSIZE pSize)
{
	if (pSize)
	{
		m_sizeDefault.cx = pSize->cx;
		m_sizeDefault.cy = pSize->cy;
	}
	else
	{
		m_sizeDefault.cx = m_sizeDefault.cy = 0;
	}

	return this;
}

//
// 注：不能直接写：b = _getter();
//     这样的结果是将SizeAttribute的this又mov到ecx了
//
void  SizeAttribute::get(LPSIZE psize)
{
    if (m_pBindValue)
    {
		psize->cx = m_pBindValue->cx;
		psize->cy = m_pBindValue->cy;
    }
    else if (_getter && _this)
    {
#ifdef _WIN64
		UIx64::GetSize(_this, psize, _getter);
#else
		long localVarThis = (long)_this;
		long localVarGetter = (long)_getter;
        __asm
        {
            mov eax, psize
            push eax;

			mov ecx, localVarThis;  
			call localVarGetter;    
        }
#endif
    }
    else
    {
		psize->cx = m_sizeDefault.cx;
		psize->cy = m_sizeDefault.cy;
    }
}

void  SizeAttribute::set(LPSIZE psize)
{
    if (m_pBindValue)
    {
		m_pBindValue->cx = psize->cx;
		m_pBindValue->cy = psize->cy;
    }
    else if (_setter && _this)
    {
#ifdef _WIN64
		UIx64::SetSize(_this, psize, _setter);
#else
        long localVarThis = (long)_this;
        long localVarSetter = (long)_setter;

        __asm
        {
            mov eax, psize   // 压入参数
            push eax;

            mov ecx, localVarThis;   // 设置this
            call localVarSetter;     // 调用
        }
#endif
    }
}

LPCTSTR  SizeAttribute::Get()
{
	SIZE s = {0};
    get(&s);

	TCHAR* szText = GetTempBuffer(64);
	_stprintf(szText, TEXT("%d,%d"), s.cx, s.cy);
    
    return szText;
}

void  SizeAttribute::Set(LPCTSTR szValue)
{
	SIZE s = {0};
	if (Util::TranslateSIZE(szValue, &s, XML_SEPARATOR))
	{
		set(&s);
	}
}

void  SizeAttribute::Reset()
{
	set(&m_sizeDefault);
}

bool  SizeAttribute::IsDefaultValue()
{
	SIZE s = {0};
	get(&s);

	if (s.cx == m_sizeDefault.cx && s.cy == m_sizeDefault.cy)
		return true;
	else
		return false;
}

void  SizeAttribute::Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e)
{
    p->Size2Editor(this, e);
}

ISizeAttribute*  SizeAttribute::GetISizeAttribute()
{
    if (!m_pISizeAttribute)
        m_pISizeAttribute = new ISizeAttribute(this);
    return m_pISizeAttribute;
}