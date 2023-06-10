#include "stdafx.h"
#include "rect_attribute.h"
#include "attribute.h"
#include "attributex64.h"

using namespace UI;

namespace UI
{
    AttributeBase*  CreateRectAttribute()
    {
        return new RectAttribute();
    }
}

RectAttribute::RectAttribute() 
{
    m_pIRectAttribute = nullptr;
    m_pBindValue = nullptr;
    SetRectEmpty(&m_rcDefault);

    _this = nullptr;
    _setter = nullptr;
    _getter = nullptr;
}

RectAttribute::~RectAttribute()
{
    SAFE_DELETE(m_pIRectAttribute);
}

void  RectAttribute::SetBindValue(void* p)
{
    m_pBindValue = (RECT*)p;
}

void  RectAttribute::SetBindFuction(void* _this, void* _setter, void* _getter)
{
    this->_this = _this;
    this->_setter = (pfnRectSetter)_setter;
    this->_getter = (pfnRectGetter)_getter;
}

RectAttribute*  RectAttribute::SetDefault(LPRECT prc)
{
	if (prc)
		CopyRect(&m_rcDefault, prc);
	else
		SetRectEmpty(&m_rcDefault);

	return this;
}

//
// 注：不能直接写：b = _getter();
//     这样的结果是将RectAttribute的this又mov到ecx了
//
void  RectAttribute::get(LPRECT prc)
{
    if (m_pBindValue)
    {
        CopyRect(prc, m_pBindValue);
    }
    else if (_getter && _this)
    {
#ifdef _WIN64
		UIx64::GetRect(_this, prc, _getter);
#else
        long localVarThis = (long)_this;
        long localVarGetter = (long)_getter;

        __asm
        {
            mov eax, prc
            push eax;

			mov ecx, localVarThis;  
			call localVarGetter;    
		}
#endif
    }
    else
    {
        CopyRect(prc, &m_rcDefault);
    }
}

void  RectAttribute::set(LPRECT prc)
{
    if (!prc)
        prc = &m_rcDefault;

    if (m_pBindValue)
    {
		CopyRect(m_pBindValue, prc);
    }
    else if (_setter && _this)
    {
#ifdef _WIN64
		UIx64::SetRect(_this, prc, _setter);
#else
        long localVarThis = (long)_this;
        long localVarSetter = (long)_setter;

        __asm
        {
            mov eax, prc   // 压入参数
            push eax;

            mov ecx, localVarThis;   // 设置this
            call localVarSetter;     // 调用
        }
#endif
    }
}

LPCTSTR  RectAttribute::Get()
{
	RECT rc = {0};
    get(&rc);

	TCHAR* szText = GetTempBuffer(64);
	_stprintf(szText, TEXT("%d,%d,%d,%d"), 
		rc.left, rc.top, rc.right, rc.bottom);
    
    return szText;
}

void  RectAttribute::Set(LPCTSTR szValue)
{
	RECT rc = {0};
	if (Util::TranslateRECT(szValue, &rc, XML_SEPARATOR))
	{
		set(&rc);
	}
    else
    {
        set(nullptr);
    }
}

void  RectAttribute::Reset()
{
	set(&m_rcDefault);
}

bool  RectAttribute::IsDefaultValue()
{
	RECT rc = {0};
	get(&rc);

	return ::EqualRect(&m_rcDefault, &rc) ? true:false;
}

void  RectAttribute::Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e)
{
    p->Rect2Editor(this, e);
}

IRectAttribute*  RectAttribute::GetIRectAttribute()
{
    if (!m_pIRectAttribute)
        m_pIRectAttribute = new IRectAttribute(this);
    return m_pIRectAttribute;
}