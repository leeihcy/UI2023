#include "include/inc.h"
#include "size_attribute.h"
#include "attribute.h"
// #include "attributex64.h"


namespace ui
{
    AttributeBase*  CreateSizeAttribute()
    {
        return new SizeAttribute();
    }

SizeAttribute::SizeAttribute() 
{
    m_pISizeAttribute = nullptr;
    m_pBindValue = nullptr;
    m_sizeDefault.width = m_sizeDefault.height = 0;

    // _this = nullptr;
    // _setter = nullptr;
    // _getter = nullptr;
}

SizeAttribute::~SizeAttribute()
{
    SAFE_DELETE(m_pISizeAttribute);
}

void  SizeAttribute::SetBindValue(void* p)
{
    m_pBindValue = (Size*)p;
}

// void  SizeAttribute::SetBindFuction(void* _this, void* _setter, void* _getter)
// {
//     this->_this = _this;
//     this->_setter = (pfnSizeSetter)_setter;
//     this->_getter = (pfnSizeGetter)_getter;
// }
void SizeAttribute::Bind(slot<void(Size*)>&& s, slot<void(Size*)>&& g)
{
    m_setter.connect(std::forward<slot<void(Size*)>>(s));
    m_getter.connect(std::forward<slot<void(Size*)>>(g));
}

SizeAttribute*  SizeAttribute::SetDefault(Size* pSize)
{
	if (pSize)
	{
		m_sizeDefault.width = pSize->width;
		m_sizeDefault.height = pSize->height;
	}
	else
	{
		m_sizeDefault.width = m_sizeDefault.height = 0;
	}

	return this;
}

//
// 注：不能直接写：b = _getter();
//     这样的结果是将SizeAttribute的this又mov到ecx了
//
void  SizeAttribute::get(Size* psize)
{
    if (m_pBindValue)
    {
		psize->width = m_pBindValue->width;
		psize->height = m_pBindValue->height;
    }
//     else if (_getter && _this)
//     {
// #ifdef _WIN64
// 		UIx64::GetSize(_this, psize, _getter);
// #else
// 		long localVarThis = (long)_this;
// 		long localVarGetter = (long)_getter;
//         __asm
//         {
//             mov eax, psize
//             push eax;

// 			mov ecx, localVarThis;  
// 			call localVarGetter;    
//         }
// #endif
//     }
    else if (!m_getter.empty()) {
        m_getter.emit(psize);
    }
    else
    {
		psize->width = m_sizeDefault.width;
		psize->height = m_sizeDefault.height;
    }
}

void  SizeAttribute::set(Size* psize)
{
    if (m_pBindValue)
    {
		m_pBindValue->width = psize->width;
		m_pBindValue->height = psize->height;
    }
//     else if (_setter && _this)
//     {
// #ifdef _WIN64
// 		UIx64::SetSize(_this, psize, _setter);
// #else
//         long localVarThis = (long)_this;
//         long localVarSetter = (long)_setter;

//         __asm
//         {
//             mov eax, psize   // 压入参数
//             push eax;

//             mov ecx, localVarThis;   // 设置this
//             call localVarSetter;     // 调用
//         }
// #endif
//     }
    else if (!m_setter.empty()) {
        m_setter.emit(psize);
    }
}

const wchar_t*  SizeAttribute::Get()
{
	Size s = {0};
    get(&s);

	wchar_t* szText = GetTempBuffer(64);
	wprintf(szText, TEXT("%d,%d"), s.width, s.height);
    
    return szText;
}

void  SizeAttribute::Set(const wchar_t* szValue)
{
	Size s = {0};
	if (util::TranslateSIZE(szValue, &s, XML_SEPARATOR))
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
	Size s = {0};
	get(&s);

	if (s.width == m_sizeDefault.width && s.height == m_sizeDefault.height)
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

}