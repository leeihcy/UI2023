#include "stdafx.h"
#include "long_attribute.h"
#include "attribute.h"
#include "Src\Util\DPI\dpihelper.h"
#include "attributex64.h"

using namespace UI;

namespace UI
{
    AttributeBase*  CreateLongAttribute()
    {
        return new LongAttribute();
    }

}

LongAttribute::LongAttribute() 
{
    m_pILongAttribute = nullptr;
    m_pBindValue = nullptr;
    m_lDefault = 0;

	m_eDpiScaleType = LONGATTR_DPI_SCALE_DISABLE;

    _this = nullptr;
    _setter = nullptr;
    _getter = nullptr;
}

LongAttribute::~LongAttribute()
{
    SAFE_DELETE(m_pILongAttribute);
}

ILongAttribute*  LongAttribute::GetILongAttribute()
{
    if (!m_pILongAttribute)    
        m_pILongAttribute = new ILongAttribute(this);

    return m_pILongAttribute;
}

void  LongAttribute::SetBindValue(void* p)
{
    m_pBindValue = (long*)p;
}

void  LongAttribute::SetBindFuction(void* _this, void* _setter, void* _getter)
{
    this->_this = _this;
    this->_setter = (pfnLongSetter)_setter;
    this->_getter = (pfnLongGetter)_getter;
}

LongAttribute*  LongAttribute::SetDefault(long l)
{
	m_lDefault = l;

	return this;
}

//
// 注：不能直接写：b = _getter();
//     这样的结果是将LongAttribute的this又mov到ecx了
//
long  LongAttribute::GetLong()
{
	long lReturn = m_lDefault;
    if (m_pBindValue)
    {
        lReturn = (*m_pBindValue);
    }
    else if (_getter && _this)
    {
        lReturn = m_lDefault;
#ifdef _WIN64
		lReturn = UIx64::GetLong(_this, _getter);
#else 
		ULONG_PTR localVarThis = (ULONG_PTR)_this;
		ULONG_PTR localVarGetter = (ULONG_PTR)_getter;

        __asm
        {
            mov ecx, localVarThis;  // 设置this
            call localVarGetter;    // 调用
            mov lReturn, eax;       // 获取返回值
        }
#endif
    }
    else
    {
        lReturn = m_lDefault;
    }
	
	switch (m_eDpiScaleType)
	{
	case LONGATTR_DPI_SCALE_ENABLE:
		lReturn = UI::RestoreByDpi(lReturn);
		break;

	case LONGATTR_DPI_SCALE_GRATETHAN_0:
		lReturn = UI::RestoreByDpi_if_gt0(lReturn);
		break;
	}

	return lReturn;
}

void  LongAttribute::SetLong(long l)
{
	switch (m_eDpiScaleType)
	{
	case LONGATTR_DPI_SCALE_ENABLE:
		l = UI::ScaleByDpi(l);
		break;

	case LONGATTR_DPI_SCALE_GRATETHAN_0:
		l = UI::ScaleByDpi_if_gt0(l);
		break;
	}

    if (m_pBindValue)
    {
        (*m_pBindValue) = l;
    }
    else if (_setter && _this)
    {
#ifdef _WIN64
		UIx64::SetLong(_this, l, _setter);
#else
        ULONG_PTR localVarThis = (ULONG_PTR)_this;
		ULONG_PTR localVarSetter = (ULONG_PTR)_setter;

        __asm
        {
            mov eax, dword ptr [l]   // 压入参数
            push eax;

            mov ecx, localVarThis;   // 设置this
            call localVarSetter;     // 调用
        }
#endif
    }
}
LPCTSTR  LongAttribute::Get()
{
    long lValue = GetLong();
    
    LPCTSTR szAlias = m_mapAlias.GetAlias(lValue);
    if (szAlias)
    {
        return szAlias;
    }
    else
    {
		TCHAR* szText = GetTempBuffer(64);
        _stprintf(szText, TEXT("%d"), lValue);
        return szText;
    }
}

void  LongAttribute::Set(LPCTSTR szValue)
{
    long lValue = m_lDefault;

    if (m_mapAlias.GetAlias(szValue, &lValue))
    {
        SetLong(lValue);
    }
    else
    {
        lValue = _ttoi(szValue);
        SetLong(lValue);
    }
}

void  LongAttribute::Reset()
{
	SetLong(m_lDefault);
}

bool  LongAttribute::IsDefaultValue()
{
    return GetLong() == m_lDefault;
}

LongAttribute*  LongAttribute::AddAlias(long l, LPCTSTR sz)
{
    m_mapAlias.AddAlias(l, sz);
    return this;
}

void  LongAttribute::Editor(SERIALIZEDATA* pData, AttributeEditorProxy* p, EditorAttributeFlag e)
{
    p->Long2Editor(this, e);
}

long  LongAttribute::EnumAlias(pfnEnumAliasCallback c, WPARAM w, LPARAM l)
{
	return m_mapAlias.EnumAlias(c, w, l);
}
uint  LongAttribute::GetAliasCount()
{
	return m_mapAlias.GetCount();
}

LongAttribute* LongAttribute::SetDpiScaleType( LONGATTR_DPI_SCALE_TYPE e )
{
	m_eDpiScaleType = e;
	return this;
}