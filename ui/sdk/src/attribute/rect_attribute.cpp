#include "include/inc.h"
#include "rect_attribute.h"
#include "attribute.h"
#include "attributex64.h"


namespace ui
{
    AttributeBase*  CreateRectAttribute()
    {
        return new RectAttribute();
    }

RectAttribute::RectAttribute() 
{
    m_pIRectAttribute = nullptr;
    m_pBindValue = nullptr;
    m_rcDefault.SetEmpty();

    // _this = nullptr;
    // _setter = nullptr;
    // _getter = nullptr;
}

RectAttribute::~RectAttribute()
{
    SAFE_DELETE(m_pIRectAttribute);
}

void  RectAttribute::SetBindValue(void* p)
{
    m_pBindValue = (Rect*)p;
}

// void  RectAttribute::SetBindFuction(void* _this, void* _setter, void* _getter)
// {
//     this->_this = _this;
//     this->_setter = (pfnRectSetter)_setter;
//     this->_getter = (pfnRectGetter)_getter;
// }
void RectAttribute::Bind(slot<void(Rect*)>&& s, slot<void(Rect*)>&& g)
{
    m_setter.connect(std::forward<slot<void(Rect*)>>(s));
    m_getter.connect(std::forward<slot<void(Rect*)>>(g));
}

RectAttribute*  RectAttribute::SetDefault(Rect* prc)
{
	if (prc)
        m_rcDefault.CopyFrom(*prc);
	else
		m_rcDefault.SetEmpty();

	return this;
}

//
// 注：不能直接写：b = _getter();
//     这样的结果是将RectAttribute的this又mov到ecx了
//
void  RectAttribute::get(Rect* prc)
{
    if (m_pBindValue)
    {
        prc->CopyFrom(*m_pBindValue);
    }
//     else if (_getter && _this)
//     {
// #ifdef _WIN64
// 		UIx64::GetRect(_this, prc, _getter);
// #else
//         long localVarThis = (long)_this;
//         long localVarGetter = (long)_getter;

//         __asm
//         {
//             mov eax, prc
//             push eax;

// 			mov ecx, localVarThis;  
// 			call localVarGetter;    
// 		}
// #endif
//     }
    else if (!m_getter.empty()) {
        m_getter.emit(prc);
    }
    else
    {
        prc->CopyFrom(m_rcDefault);
    }
}

void  RectAttribute::set(Rect* prc)
{
    if (!prc)
        prc = &m_rcDefault;

    if (m_pBindValue)
    {
		m_pBindValue->CopyFrom(*prc);
    }
//     else if (_setter && _this)
//     {
// #ifdef _WIN64
// 		UIx64::SetRect(_this, prc, _setter);
// #else
//         long localVarThis = (long)_this;
//         long localVarSetter = (long)_setter;

//         __asm
//         {
//             mov eax, prc   // 压入参数
//             push eax;

//             mov ecx, localVarThis;   // 设置this
//             call localVarSetter;     // 调用
//         }
// #endif
//     }
    else if (!m_setter.empty()) {
        m_setter.emit(prc);
    }
}

const wchar_t*  RectAttribute::Get()
{
	Rect rc = {0};
    get(&rc);

	wchar_t* szText = GetTempBuffer(64);
	wprintf(szText, TEXT("%d,%d,%d,%d"), 
		rc.left, rc.top, rc.right, rc.bottom);
    
    return szText;
}

void  RectAttribute::Set(const wchar_t* szValue)
{
	Rect rc = {0};
	if (util::TranslateRECT(szValue, &rc, XML_SEPARATOR))
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
	Rect rc = {0};
	get(&rc);

	return m_rcDefault == rc ? true:false;
}

void  RectAttribute::Editor(SerializeParam* pData, AttributeEditorProxy* p, EditorAttributeFlag e)
{
    p->Rect2Editor(this, e);
}

IRectAttribute*  RectAttribute::GetIRectAttribute()
{
    if (!m_pIRectAttribute)
        m_pIRectAttribute = new IRectAttribute(this);
    return m_pIRectAttribute;
}
}