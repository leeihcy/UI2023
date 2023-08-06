#include "include/inc.h"
#include "include/interface/iattribute.h"
#include "include/interface/iobject.h"
#include "attribute.h"
#include "string_attribute.h"
#include "long_attribute.h"
#include "enum_attribute.h"
#include "bool_attribute.h"
#include "rect_attribute.h"
#include "stringselect_attribute.h"
#include "9region_attribute.h"
#include "flags_attribute.h"
#include "size_attribute.h"
#include "renderbase_attribute.h"
#include "textrenderbase_attribute.h"
#include "color_attribute.h"

using namespace ui;

//////////////////////////////////////////////////////////////////////////
ILongAttribute::ILongAttribute(LongAttribute* p)
{
    m_pImpl = p;
}
ILongAttribute*  ILongAttribute::SetDefault(long l)
{
    m_pImpl->SetDefault(l);
    return this;
}
ILongAttribute*  ILongAttribute::AddAlias(long l, const wchar_t* sz)
{
    m_pImpl->AddAlias(l, sz);
    return this;
}
ILongAttribute*  ILongAttribute::AsData()
{
    m_pImpl->AsData();
    return this;
}
ILongAttribute*  ILongAttribute::SetDpiScaleType(LONGATTR_DPI_SCALE_TYPE e)
{
	m_pImpl->SetDpiScaleType(e);
	return this;
}

const wchar_t*  ILongAttribute::GetKey()
{
    return m_pImpl->GetKey();
}
const wchar_t*  ILongAttribute::GetDesc()
{
    return EMPTYTEXT;
}
const wchar_t*  ILongAttribute::GetGroupName()
{
	return m_pImpl->GetGroupName();
}
const wchar_t*  ILongAttribute::GetParentKey()
{
    return m_pImpl->GetParentKey();
}
long  ILongAttribute::GetLong()
{
    return m_pImpl->GetLong();
}
const wchar_t*  ILongAttribute::Get()
{
    return m_pImpl->Get();
}
long  ILongAttribute::EnumAlias(pfnEnumAliasCallback c, long w, long l)
{
	return m_pImpl->EnumAlias(c, w, l);
}
uint  ILongAttribute::GetAliasCount()
{
	return m_pImpl->GetAliasCount();
}
//////////////////////////////////////////////////////////////////////////

IBoolAttribute::IBoolAttribute(BoolAttribute* p)
{
    m_pImpl = p;
}
IBoolAttribute*  IBoolAttribute::SetDefault(bool b)
{
    m_pImpl->SetDefault(b);
    return this;
}
IBoolAttribute*  IBoolAttribute::AsData()
{
    m_pImpl->AsData();
    return this;
}

const wchar_t*  IBoolAttribute::GetKey()
{
    return m_pImpl->GetKey();
}

const wchar_t*  IBoolAttribute::GetDesc()
{
    return EMPTYTEXT;
}
const wchar_t*  IBoolAttribute::GetGroupName()
{
	return m_pImpl->GetGroupName();
}
const wchar_t*  IBoolAttribute::GetParentKey()
{
    return m_pImpl->GetParentKey();
}
const wchar_t*  IBoolAttribute::Get()
{
    return m_pImpl->Get();
}
bool  IBoolAttribute::GetBool()
{
    return m_pImpl->GetBool();
}

bool  IBoolAttribute::GetDefaultBool()
{
    return m_pImpl->GetDefaultBool();
}

IBoolAttribute*  IBoolAttribute::ReloadOnChanged()
{
    m_pImpl->ReloadOnChanged();
    return this;
}

//////////////////////////////////////////////////////////////////////////
IStringAttribute::IStringAttribute(StringAttribute* p)
{
    m_pImpl = p;
}

IStringAttribute*  IStringAttribute::SetDefault(const wchar_t* sz)
{
    m_pImpl->SetDefault(sz);
    return this;
}
IStringAttribute*  IStringAttribute::AsData()
{
    m_pImpl->AsData();
    return this;
}
// IStringAttribute*  IStringAttribute::Internationalization()
// {
// 	m_pImpl->Internationalization();
// 	return this;
// }

const wchar_t*  IStringAttribute::GetKey()
{
    return m_pImpl->GetKey();
}
const wchar_t*  IStringAttribute::Get()
{
    return m_pImpl->Get();
}
const wchar_t*  IStringAttribute::GetDesc()
{
    return EMPTYTEXT;
}
const wchar_t*  IStringAttribute::GetGroupName()
{
	return m_pImpl->GetGroupName();
}
const wchar_t*  IStringAttribute::GetParentKey()
{
    return m_pImpl->GetParentKey();
}

//////////////////////////////////////////////////////////////////////////


ICharArrayAttribute::ICharArrayAttribute(CharArrayAttribute* p)
{
    m_pImpl = p;
}

//////////////////////////////////////////////////////////////////////////

IEnumAttribute::IEnumAttribute(EnumAttribute* p)
{
    m_pImpl = p;
}
IEnumAttribute*  IEnumAttribute::AddOption(long l, const wchar_t* sz)
{
    m_pImpl->AddOption(l ,sz);
    return this;
}
IEnumAttribute*  IEnumAttribute::SetDefault(long l)
{
    m_pImpl->SetDefault(l);
    return this;
}
IEnumAttribute*  IEnumAttribute::AsData()
{
    m_pImpl->AsData();
    return this;
}

const wchar_t*  IEnumAttribute::GetKey()
{
	return m_pImpl->GetKey();
}
const wchar_t*  IEnumAttribute::GetDesc()
{
	return EMPTYTEXT;
}
const wchar_t*  IEnumAttribute::GetGroupName()
{
	return  m_pImpl->GetGroupName();
}
const wchar_t*  IEnumAttribute::GetParentKey()
{
    return m_pImpl->GetParentKey();
}
const wchar_t*  IEnumAttribute::Get()
{
    return m_pImpl->Get();
}
long  IEnumAttribute::GetLong()
{
	return m_pImpl->GetLong();
}
long  IEnumAttribute::EnumAlias(pfnEnumAliasCallback c, long w, long l)
{
	return m_pImpl->EnumAlias(c, w, l);
}
uint  IEnumAttribute::GetAliasCount()
{
	return m_pImpl->GetAliasCount();
}

IEnumAttribute*  IEnumAttribute::ReloadOnChanged()
{
	m_pImpl->ReloadOnChanged();
	return this;
}

//////////////////////////////////////////////////////////////////////////

IFlagsAttribute::IFlagsAttribute(FlagsAttribute* p)
{
    m_pImpl = p;
}
IFlagsAttribute*  IFlagsAttribute::AddFlag(long l, const wchar_t* sz)
{
    m_pImpl->AddFlag(l ,sz);
    return this;
}
IFlagsAttribute*  IFlagsAttribute::SetDefault(long l)
{
    m_pImpl->SetDefault(l);
    return this;
}
IFlagsAttribute*  IFlagsAttribute::AsData()
{
    m_pImpl->AsData();
    return this;
}

const wchar_t*  IFlagsAttribute::GetKey()
{
    return m_pImpl->GetKey();
}
const wchar_t*  IFlagsAttribute::GetDesc()
{
    return EMPTYTEXT;
}
const wchar_t*  IFlagsAttribute::GetGroupName()
{
    return  m_pImpl->GetGroupName();
}
const wchar_t*  IFlagsAttribute::GetParentKey()
{
    return m_pImpl->GetParentKey();
}
const wchar_t*  IFlagsAttribute::Get()
{
    return m_pImpl->Get();
}
long  IFlagsAttribute::GetLong()
{
    return m_pImpl->GetLong();
}
long  IFlagsAttribute::EnumAlias(pfnEnumAliasCallback c, long w, long l)
{
    return m_pImpl->EnumAlias(c, w, l);
}
uint  IFlagsAttribute::GetAliasCount()
{
    return m_pImpl->GetAliasCount();
}

//////////////////////////////////////////////////////////////////////////

IRectAttribute::IRectAttribute(RectAttribute* p)
{
    m_pImpl = p;
}
IRectAttribute*  IRectAttribute::AsData()
{
    m_pImpl->AsData();
    return this;
}

const wchar_t*  IRectAttribute::GetKey()
{
    return m_pImpl->GetKey();
}
const wchar_t*  IRectAttribute::GetDesc()
{
    return m_pImpl->GetDesc();
}
const wchar_t*  IRectAttribute::GetGroupName()
{
	return m_pImpl->GetGroupName();
}
const wchar_t*  IRectAttribute::GetParentKey()
{
    return m_pImpl->GetParentKey();
}
const wchar_t*  IRectAttribute::Get()
{
    return m_pImpl->Get();
}

//////////////////////////////////////////////////////////////////////////


ISizeAttribute::ISizeAttribute(SizeAttribute* p)
{
	m_pImpl = p;
}
ISizeAttribute*  ISizeAttribute::AsData()
{
	m_pImpl->AsData();
	return this;
}

const wchar_t*  ISizeAttribute::GetKey()
{
	return m_pImpl->GetKey();
}
const wchar_t*  ISizeAttribute::GetDesc()
{
	return m_pImpl->GetDesc();
}
const wchar_t*  ISizeAttribute::GetGroupName()
{
	return m_pImpl->GetGroupName();
}
const wchar_t*  ISizeAttribute::GetParentKey()
{
	return m_pImpl->GetParentKey();
}
const wchar_t*  ISizeAttribute::Get()
{
	return m_pImpl->Get();
}

//////////////////////////////////////////////////////////////////////////

IRegion9Attribute::IRegion9Attribute(Region9Attribute* p)
{
	m_pImpl = p;
}

IRegion9Attribute*  IRegion9Attribute::AsData()
{
	m_pImpl->AsData();
	return this;
}

const wchar_t*  IRegion9Attribute::GetKey()
{
	return m_pImpl->GetKey();
}
const wchar_t*  IRegion9Attribute::GetDesc()
{
	return m_pImpl->GetDesc();
}
const wchar_t*  IRegion9Attribute::GetGroupName()
{
	return m_pImpl->GetGroupName();
}
const wchar_t*  IRegion9Attribute::GetParentKey()
{
    return m_pImpl->GetParentKey();
}
const wchar_t*  IRegion9Attribute::Get()
{
	return m_pImpl->Get();
}

//////////////////////////////////////////////////////////////////////////

IStringEnumAttribute::IStringEnumAttribute(StringEnumAttribute* p)
{
    m_pImpl = p;
}

const wchar_t*  IStringEnumAttribute::Get()
{
    return m_pImpl->Get();
}

void  IStringEnumAttribute::EnumString(pfnEnumStringEnumCallback callback, long w, long l)
{
    m_pImpl->EnumString(callback, w, l);
}

IStringEnumAttribute*  IStringEnumAttribute::FillRenderBaseTypeData()
{
	m_pImpl->FillRenderBaseTypeData();
	return this;
}
IStringEnumAttribute*  IStringEnumAttribute::ReloadOnChanged()
{
	m_pImpl->ReloadOnChanged();
	return this;
}
IStringEnumAttribute*  IStringEnumAttribute::FillTextRenderBaseTypeData()
{
	m_pImpl->FillTextRenderBaseTypeData();
	return this;
}

IStringEnumAttribute*  IStringEnumAttribute::AsData()
{
    m_pImpl->AsData();
    return this;
}

const wchar_t*  IStringEnumAttribute::GetKey()
{
    return m_pImpl->GetKey();
}
const wchar_t*  IStringEnumAttribute::GetDesc()
{
    return m_pImpl->GetDesc();
}
const wchar_t*  IStringEnumAttribute::GetGroupName()
{
	return m_pImpl->GetGroupName();
}
const wchar_t*  IStringEnumAttribute::GetParentKey()
{
    return m_pImpl->GetParentKey();
}
//////////////////////////////////////////////////////////////////////////

IColorAttribute::IColorAttribute(ColorAttribute* p)
{
	m_pImpl = p;
}

const wchar_t*  IColorAttribute::GetKey()
{
	return m_pImpl->GetKey();
}
const wchar_t*  IColorAttribute::GetDesc()
{
	return m_pImpl->GetDesc();
}
const wchar_t*  IColorAttribute::GetGroupName()
{
	return m_pImpl->GetGroupName();
}
const wchar_t*  IColorAttribute::GetParentKey()
{
	return m_pImpl->GetParentKey();
}
const wchar_t*  IColorAttribute::Get()
{
	return m_pImpl->Get();
}

//////////////////////////////////////////////////////////////////////////

IRenderBaseAttribute::IRenderBaseAttribute(RenderBaseAttribute* p)
{
    m_pImpl = p;
}
IRenderBaseAttribute*  IRenderBaseAttribute::AsData()
{
    m_pImpl->AsData();
    return this;
}

const wchar_t*  IRenderBaseAttribute::GetKey()
{
    return m_pImpl->GetKey();
}
const wchar_t*  IRenderBaseAttribute::GetDesc()
{
    return m_pImpl->GetDesc();
}
const wchar_t*  IRenderBaseAttribute::Get()
{
    return m_pImpl->Get();
}

const wchar_t*  IRenderBaseAttribute::GetGroupName()
{
	return m_pImpl->GetGroupName();
}
const wchar_t*  IRenderBaseAttribute::GetParentKey()
{
	return m_pImpl->GetParentKey();
}

//////////////////////////////////////////////////////////////////////////

ITextRenderBaseAttribute::ITextRenderBaseAttribute(TextRenderBaseAttribute* p)
{
    m_pImpl = p;
}
ITextRenderBaseAttribute*  ITextRenderBaseAttribute::AsData()
{
    m_pImpl->AsData();
    return this;
}

const wchar_t*  ITextRenderBaseAttribute::GetKey()
{
    return m_pImpl->GetKey();
}
const wchar_t*  ITextRenderBaseAttribute::GetDesc()
{
    return m_pImpl->GetDesc();
}
const wchar_t*  ITextRenderBaseAttribute::Get()
{
    return m_pImpl->Get();
}
const wchar_t*  ITextRenderBaseAttribute::GetGroupName()
{
	return m_pImpl->GetGroupName();
}
const wchar_t*  ITextRenderBaseAttribute::GetParentKey()
{
	return m_pImpl->GetParentKey();
}

//////////////////////////////////////////////////////////////////////////

AttributeSerializerWrap::AttributeSerializerWrap(SERIALIZEDATA* p, const wchar_t* szGroupName)
{
    m_pImpl = new AttributeSerializer(p, szGroupName);
}
AttributeSerializerWrap::~AttributeSerializerWrap()
{
    SAFE_DELETE(m_pImpl);
}
AttributeSerializer*  AttributeSerializerWrap::GetImpl()
{
	return m_pImpl;
}

// IStringAttribute*  AttributeSerializerWrap::AddString(const wchar_t* szKey, void* _this, pfnStringSetter s, pfnStringGetter g)
// {
//     StringAttribute* p = m_pImpl->AddString(szKey, _this, s, g);
//     if (!p)
//         return nullptr;

//     return p->GetIStringAttribute();
// }

// IStringAttribute*  AttributeSerializerWrap::AddString(
//     const wchar_t* szKey, 
//     const std::function<void(const wchar_t*)>& s, 
//     const std::function<const wchar_t*()>& g)
// {
//     StringAttribute* p = m_pImpl->AddString(szKey, s, g);
//     if (!p)
//         return nullptr;

//     return p->GetIStringAttribute();
// }

// IStringAttribute*  AttributeSerializerWrap::AddI18nString(const wchar_t* szKey, void* _this, pfnStringExSetter s, pfnStringGetter g)
// {
//     StringAttribute* p = m_pImpl->AddI18nString(szKey, _this, s, g);
//     if (!p)
//         return nullptr;
// 
//     return p->GetIStringAttribute();
// }

// IStringAttribute*  AttributeSerializerWrap::AddI18nString(
//     const wchar_t* szKey, 
//     const std::function<void(const wchar_t*, int)>& s, 
//     const std::function<const wchar_t*()>& g)
// {
//     StringAttribute* p = m_pImpl->AddI18nString(szKey, s, g);
//     if (!p)
//         return nullptr;

//     return p->GetIStringAttribute();
// }

// IStringEnumAttribute*  AttributeSerializerWrap::AddStringEnum(const wchar_t* szKey, void* _this, pfnStringSetter s, pfnStringGetter g)
// {
// 	StringEnumAttribute* p = m_pImpl->AddStringEnum(szKey, _this, s, g);
// 	if (!p)
// 		return nullptr;

// 	return p->GetIStringEnumAttribute();
// }

IBoolAttribute*  AttributeSerializerWrap::AddBool(const wchar_t* szKey, bool& bBindValue)
{
    BoolAttribute* p = m_pImpl->AddBool(szKey, bBindValue);
    if (!p)
        return nullptr;

    return p->GetIBoolAttribute();
}
// IBoolAttribute*  AttributeSerializerWrap::AddBool(const wchar_t* szKey, void* _this, pfnBoolSetter s, pfnBoolGetter g)
// {
//     BoolAttribute* p = m_pImpl->AddBool(szKey, _this, s, g);
//     if (!p)
//         return nullptr;

//     return p->GetIBoolAttribute();
// }

ILongAttribute*  AttributeSerializerWrap::AddLong(const wchar_t* szKey, long& lBindValue)
{
    LongAttribute* p = m_pImpl->AddLong(szKey, lBindValue);
    if (!p)
        return nullptr;

    return p->GetILongAttribute();
}
ILongAttribute*  AttributeSerializerWrap::AddLong(const wchar_t* szKey, int& lBindValue)
{
	return AddLong(szKey, *(long*)&lBindValue);
}
// ILongAttribute*  AttributeSerializerWrap::AddLong(const wchar_t* szKey, void* _this, pfnLongSetter s, pfnLongGetter g)
// {
//     LongAttribute* p = m_pImpl->AddLong(szKey, _this, s, g);
//     if (!p)
//         return nullptr;

//     return p->GetILongAttribute();
// }

IFlagsAttribute*  AttributeSerializerWrap::AddFlags(const wchar_t* szKey, long& lBindValue)
{
	FlagsAttribute* p = m_pImpl->AddFlags(szKey, lBindValue);
	if (!p)
		return nullptr;

	return p->GetIFlagsAttribute();
}

IEnumAttribute*  AttributeSerializerWrap::AddEnum(const wchar_t* szKey, long& lBindValue)
{
    EnumAttribute* p = m_pImpl->AddEnum(szKey, lBindValue);
    if (!p)
        return nullptr;

    return p->GetIEnumAttribute();
}
// IEnumAttribute*  AttributeSerializerWrap::AddEnum(const wchar_t* szKey, void* _this, pfnLongSetter s, pfnLongGetter g)
// {
//     EnumAttribute* p = m_pImpl->AddEnum(szKey, _this, s, g);
//     if (!p)
//         return nullptr;

//     return p->GetIEnumAttribute();
// }

IRectAttribute*  AttributeSerializerWrap::AddRect(const wchar_t* szKey, Rect& rcBindValue)
{
	RectAttribute* p = m_pImpl->AddRect(szKey, rcBindValue);
	if (!p)
		return nullptr;

	return p->GetIRectAttribute();
}
// IRectAttribute*  AttributeSerializerWrap::AddRect(const wchar_t* szKey, void* _this, pfnRectSetter s, pfnRectGetter g)
// {
// 	RectAttribute* p = m_pImpl->AddRect(szKey, _this, s, g);
// 	if (!p)
// 		return nullptr;

// 	return p->GetIRectAttribute();
// }

ISizeAttribute*  AttributeSerializerWrap::AddSize(const wchar_t* szKey, Size& sBindValue)
{
	SizeAttribute* p = m_pImpl->AddSize(szKey, sBindValue);
	if (!p)
		return nullptr;

	return p->GetISizeAttribute();
}
// ISizeAttribute*  AttributeSerializerWrap::AddSize(const wchar_t* szKey, void* _this, pfnSizeSetter s, pfnSizeGetter g)
// {
// 	SizeAttribute* p = m_pImpl->AddSize(szKey, _this, s, g);
// 	if (!p)
// 		return nullptr;

// 	return p->GetISizeAttribute();
// }

IRegion9Attribute*  AttributeSerializerWrap::Add9Region(const wchar_t* szKey, C9Region& rBindValue)
{
	Region9Attribute* p = m_pImpl->Add9Region(szKey, rBindValue);
	if (!p)
		return nullptr;

	return p->GetIRegion9Attribute();
}

IColorAttribute*  AttributeSerializerWrap::AddColor(const wchar_t* szKey, Color*& pBindValue)
{
	ColorAttribute* p = m_pImpl->AddColor(szKey, pBindValue);
	if (!p)
		return nullptr;

	return p->GetIColorAttribute();
}
IColorAttribute*  AttributeSerializerWrap::AddColor(const wchar_t* szKey, Color& pBindValue)
{
	ColorAttribute* p = m_pImpl->AddColor(szKey, pBindValue);
	if (!p)
		return nullptr;

	return p->GetIColorAttribute();
}

// IRegion9Attribute*  AttributeSerializerWrap::Add9Region(const wchar_t* szKey, void* _this, pfnRectSetter s, pfnRectGetter g)
// {
// 	Region9Attribute* p = m_pImpl->Add9Region(szKey, _this, s, g);
// 	if (!p)
// 		return nullptr;

// 	return p->GetIRegion9Attribute();
// }

IRenderBaseAttribute*  AttributeSerializerWrap::AddRenderBase(const wchar_t* szPrefix, IObject* pObj, IRenderBase*& pBindValue)
{
	RenderBaseAttribute* p = m_pImpl->AddRenderBase(
		szPrefix, pObj?pObj->GetImpl():nullptr, pBindValue);
	if (!p)
		return nullptr;

	return p->GetIRenderBaseAttribute();
}

ITextRenderBaseAttribute*  AttributeSerializerWrap::AddTextRenderBase(
		const wchar_t* szPrefix, IObject* pObj, ITextRenderBase*& pBindValue)
{
	TextRenderBaseAttribute* p = m_pImpl->AddTextRenderBase(
		szPrefix, pObj?pObj->GetImpl():nullptr, pBindValue);
	if (!p)
		return nullptr;

	return p->GetITextRenderBaseAttribute();
}
//////////////////////////////////////////////////////////////////////////

IAttributeEditorProxy::IAttributeEditorProxy(IUIEditor* p)
{
    m_pImpl = new AttributeEditorProxy(*this, p);
}
IAttributeEditorProxy::~IAttributeEditorProxy()
{
    SAFE_DELETE(m_pImpl);
}
AttributeEditorProxy*  IAttributeEditorProxy::GetImpl()
{
    return m_pImpl;
}

void  IAttributeEditorProxy::LoadAttribute2Editor(IObject* pObj)
{
    m_pImpl->LoadAttribute2Editor(pObj);
}
UpdateAttribute2EditorResult  IAttributeEditorProxy::UpdateAttribute2Editor(const wchar_t* szKey)
{
	 return m_pImpl->UpdateAttribute2Editor(szKey);
}
bool  IAttributeEditorProxy::SetAttribute(const wchar_t* szKey, const wchar_t* szValue)
{
    return m_pImpl->Set(szKey, szValue);
}

const wchar_t*  IAttributeEditorProxy::GetAttribute(const wchar_t* szKey)
{
    return m_pImpl->Get(szKey);
}