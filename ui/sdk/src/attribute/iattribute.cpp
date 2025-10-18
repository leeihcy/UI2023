#include "include/interface/iattribute.h"
#include "9region_attribute.h"
#include "attribute.h"
#include "bool_attribute.h"
#include "color_attribute.h"
#include "enum_attribute.h"
#include "flags_attribute.h"
#include "include/inc.h"
#include "include/interface/iobject.h"
#include "include/util/rect.h"
#include "int_attribute.h"
#include "rect_attribute.h"
#include "radius_attribute.h"
#include "renderbase_attribute.h"
#include "size_attribute.h"
#include "string_attribute.h"
#include "stringselect_attribute.h"
#include "textrenderbase_attribute.h"

using namespace ui;

//////////////////////////////////////////////////////////////////////////
IIntAttribute::IIntAttribute(IntAttribute *p) { m_pImpl = p; }
IIntAttribute *IIntAttribute::SetDefault(int l) {
  m_pImpl->SetDefault(l);
  return this;
}
IIntAttribute *IIntAttribute::AddAlias(int l, const char *sz) {
  m_pImpl->AddAlias(l, sz);
  return this;
}
IIntAttribute *IIntAttribute::AsData() {
  m_pImpl->AsData();
  return this;
}
IIntAttribute *IIntAttribute::SetDpiScaleType(LONGATTR_DPI_SCALE_TYPE e) {
  m_pImpl->SetDpiScaleType(e);
  return this;
}

const char *IIntAttribute::GetKey() { return m_pImpl->GetKey(); }
const char *IIntAttribute::GetDesc() { return EMPTYTEXT; }
const char *IIntAttribute::GetGroupName() { return m_pImpl->GetGroupName(); }
const char *IIntAttribute::GetParentKey() { return m_pImpl->GetParentKey(); }
int IIntAttribute::GetLong() { return m_pImpl->GetLong(); }
const char *IIntAttribute::Get() { return m_pImpl->Get(); }
int IIntAttribute::EnumAlias(pfnEnumAliasCallback c, int w, int l) {
  return m_pImpl->EnumAlias(c, w, l);
}
uint IIntAttribute::GetAliasCount() { return m_pImpl->GetAliasCount(); }

/////
ILengthAttribute::ILengthAttribute(LengthAttribute *p) { m_pImpl = p; }
ILengthAttribute *ILengthAttribute::SetDefault(Length l) {
  m_pImpl->SetDefault(l);
  return this;
}
ILengthAttribute *ILengthAttribute::AsData() {
  m_pImpl->AsData();
  return this;
}
ILengthAttribute *ILengthAttribute::SetDpiScaleType(LONGATTR_DPI_SCALE_TYPE e) {
  m_pImpl->SetDpiScaleType(e);
  return this;
}

const char *ILengthAttribute::GetKey() { return m_pImpl->GetKey(); }
const char *ILengthAttribute::GetDesc() { return EMPTYTEXT; }
const char *ILengthAttribute::GetGroupName() { return m_pImpl->GetGroupName(); }
const char *ILengthAttribute::GetParentKey() { return m_pImpl->GetParentKey(); }
Length ILengthAttribute::GetLength() { return m_pImpl->GetLength(); }
const char *ILengthAttribute::Get() { return m_pImpl->Get(); }

//////////////////////////////////////////////////////////////////////////

IBoolAttribute::IBoolAttribute(BoolAttribute *p) { m_pImpl = p; }
IBoolAttribute *IBoolAttribute::SetDefault(bool b) {
  m_pImpl->SetDefault(b);
  return this;
}
IBoolAttribute *IBoolAttribute::AsData() {
  m_pImpl->AsData();
  return this;
}

const char *IBoolAttribute::GetKey() { return m_pImpl->GetKey(); }

const char *IBoolAttribute::GetDesc() { return EMPTYTEXT; }
const char *IBoolAttribute::GetGroupName() { return m_pImpl->GetGroupName(); }
const char *IBoolAttribute::GetParentKey() { return m_pImpl->GetParentKey(); }
const char *IBoolAttribute::Get() { return m_pImpl->Get(); }
bool IBoolAttribute::GetBool() { return m_pImpl->GetBool(); }

bool IBoolAttribute::GetDefaultBool() { return m_pImpl->GetDefaultBool(); }

IBoolAttribute *IBoolAttribute::ReloadOnChanged() {
  m_pImpl->ReloadOnChanged();
  return this;
}

//////////////////////////////////////////////////////////////////////////
IStringAttribute::IStringAttribute(StringAttribute *p) { m_pImpl = p; }

IStringAttribute *IStringAttribute::SetDefault(const char *sz) {
  m_pImpl->SetDefault(sz);
  return this;
}
IStringAttribute *IStringAttribute::AsData() {
  m_pImpl->AsData();
  return this;
}
// IStringAttribute*  IStringAttribute::Internationalization()
// {
// 	m_pImpl->Internationalization();
// 	return this;
// }

const char *IStringAttribute::GetKey() { return m_pImpl->GetKey(); }
const char *IStringAttribute::Get() { return m_pImpl->Get(); }
const char *IStringAttribute::GetDesc() { return EMPTYTEXT; }
const char *IStringAttribute::GetGroupName() { return m_pImpl->GetGroupName(); }
const char *IStringAttribute::GetParentKey() { return m_pImpl->GetParentKey(); }

//////////////////////////////////////////////////////////////////////////

ICharArrayAttribute::ICharArrayAttribute(CharArrayAttribute *p) { m_pImpl = p; }

//////////////////////////////////////////////////////////////////////////

IEnumAttribute::IEnumAttribute(EnumAttribute *p) { m_pImpl = p; }
IEnumAttribute *IEnumAttribute::AddOption(int l, const char *sz) {
  m_pImpl->AddOption(l, sz);
  return this;
}
IEnumAttribute *IEnumAttribute::SetDefault(int l) {
  m_pImpl->SetDefault(l);
  return this;
}
IEnumAttribute *IEnumAttribute::AsData() {
  m_pImpl->AsData();
  return this;
}

const char *IEnumAttribute::GetKey() { return m_pImpl->GetKey(); }
const char *IEnumAttribute::GetDesc() { return EMPTYTEXT; }
const char *IEnumAttribute::GetGroupName() { return m_pImpl->GetGroupName(); }
const char *IEnumAttribute::GetParentKey() { return m_pImpl->GetParentKey(); }
const char *IEnumAttribute::Get() { return m_pImpl->Get(); }
int IEnumAttribute::GetLong() { return m_pImpl->GetLong(); }
int IEnumAttribute::EnumAlias(pfnEnumAliasCallback c, int w, int l) {
  return m_pImpl->EnumAlias(c, w, l);
}
uint IEnumAttribute::GetAliasCount() { return m_pImpl->GetAliasCount(); }

IEnumAttribute *IEnumAttribute::ReloadOnChanged() {
  m_pImpl->ReloadOnChanged();
  return this;
}

//////////////////////////////////////////////////////////////////////////

IFlagsAttribute::IFlagsAttribute(FlagsAttribute *p) { m_pImpl = p; }
IFlagsAttribute *IFlagsAttribute::AddFlag(int l, const char *sz) {
  m_pImpl->AddFlag(l, sz);
  return this;
}
IFlagsAttribute *IFlagsAttribute::SetDefault(int l) {
  m_pImpl->SetDefault(l);
  return this;
}
IFlagsAttribute *IFlagsAttribute::AsData() {
  m_pImpl->AsData();
  return this;
}

const char *IFlagsAttribute::GetKey() { return m_pImpl->GetKey(); }
const char *IFlagsAttribute::GetDesc() { return EMPTYTEXT; }
const char *IFlagsAttribute::GetGroupName() { return m_pImpl->GetGroupName(); }
const char *IFlagsAttribute::GetParentKey() { return m_pImpl->GetParentKey(); }
const char *IFlagsAttribute::Get() { return m_pImpl->Get(); }
int IFlagsAttribute::GetLong() { return m_pImpl->GetLong(); }
int IFlagsAttribute::EnumAlias(pfnEnumAliasCallback c, int w, int l) {
  return m_pImpl->EnumAlias(c, w, l);
}
uint IFlagsAttribute::GetAliasCount() { return m_pImpl->GetAliasCount(); }

//////////////////////////////////////////////////////////////////////////

IRectAttribute::IRectAttribute(RectAttribute *p) { m_pImpl = p; }
IRectAttribute *IRectAttribute::AsData() {
  m_pImpl->AsData();
  return this;
}
IRectAttribute *IRectAttribute::SetCompatibleKey(const char *key) {
  m_pImpl->SetCompatibleKey(key);
  return this;
}
IRectAttribute *IRectAttribute::SetDefault(const Rect& rc) {
  m_pImpl->SetDefault(rc);
  return this;
}

const char *IRectAttribute::GetKey() { return m_pImpl->GetKey(); }
const char *IRectAttribute::GetDesc() { return m_pImpl->GetDesc(); }
const char *IRectAttribute::GetGroupName() { return m_pImpl->GetGroupName(); }
const char *IRectAttribute::GetParentKey() { return m_pImpl->GetParentKey(); }
const char *IRectAttribute::Get() { return m_pImpl->Get(); }

//////////////////////////////////////////////////////////////////////////

IRadiusAttribute::IRadiusAttribute(RadiusAttribute *p) { m_pImpl = p; }
IRadiusAttribute *IRadiusAttribute::AsData() {
  m_pImpl->AsData();
  return this;
}
IRadiusAttribute *IRadiusAttribute::SetCompatibleKey(const char *key) {
  m_pImpl->SetCompatibleKey(key);
  return this;
}
IRadiusAttribute *IRadiusAttribute::SetDefault(const CornerRadius& rc) {
  m_pImpl->SetDefault(rc);
  return this;
}

const char *IRadiusAttribute::GetKey() { return m_pImpl->GetKey(); }
const char *IRadiusAttribute::GetDesc() { return m_pImpl->GetDesc(); }
const char *IRadiusAttribute::GetGroupName() { return m_pImpl->GetGroupName(); }
const char *IRadiusAttribute::GetParentKey() { return m_pImpl->GetParentKey(); }
const char *IRadiusAttribute::Get() { return m_pImpl->Get(); }

//////////////////////////////////////////////////////////////////////////

ISizeAttribute::ISizeAttribute(SizeAttribute *p) { m_pImpl = p; }
ISizeAttribute *ISizeAttribute::AsData() {
  m_pImpl->AsData();
  return this;
}

const char *ISizeAttribute::GetKey() { return m_pImpl->GetKey(); }
const char *ISizeAttribute::GetDesc() { return m_pImpl->GetDesc(); }
const char *ISizeAttribute::GetGroupName() { return m_pImpl->GetGroupName(); }
const char *ISizeAttribute::GetParentKey() { return m_pImpl->GetParentKey(); }
const char *ISizeAttribute::Get() { return m_pImpl->Get(); }

//////////////////////////////////////////////////////////////////////////

IRegion9Attribute::IRegion9Attribute(Region9Attribute *p) { m_pImpl = p; }

IRegion9Attribute *IRegion9Attribute::AsData() {
  m_pImpl->AsData();
  return this;
}

const char *IRegion9Attribute::GetKey() { return m_pImpl->GetKey(); }
const char *IRegion9Attribute::GetDesc() { return m_pImpl->GetDesc(); }
const char *IRegion9Attribute::GetGroupName() {
  return m_pImpl->GetGroupName();
}
const char *IRegion9Attribute::GetParentKey() {
  return m_pImpl->GetParentKey();
}
const char *IRegion9Attribute::Get() { return m_pImpl->Get(); }

//////////////////////////////////////////////////////////////////////////

IStringEnumAttribute::IStringEnumAttribute(StringEnumAttribute *p) {
  m_pImpl = p;
}

const char *IStringEnumAttribute::Get() { return m_pImpl->Get(); }

void IStringEnumAttribute::EnumString(pfnEnumStringEnumCallback callback, int w,
                                      int l) {
  m_pImpl->EnumString(callback, w, l);
}

IStringEnumAttribute *IStringEnumAttribute::FillRenderBaseTypeData() {
  m_pImpl->FillRenderBaseTypeData();
  return this;
}
IStringEnumAttribute *IStringEnumAttribute::ReloadOnChanged() {
  m_pImpl->ReloadOnChanged();
  return this;
}
IStringEnumAttribute *IStringEnumAttribute::FillTextRenderBaseTypeData() {
  m_pImpl->FillTextRenderBaseTypeData();
  return this;
}

IStringEnumAttribute *IStringEnumAttribute::AsData() {
  m_pImpl->AsData();
  return this;
}

const char *IStringEnumAttribute::GetKey() { return m_pImpl->GetKey(); }
const char *IStringEnumAttribute::GetDesc() { return m_pImpl->GetDesc(); }
const char *IStringEnumAttribute::GetGroupName() {
  return m_pImpl->GetGroupName();
}
const char *IStringEnumAttribute::GetParentKey() {
  return m_pImpl->GetParentKey();
}
//////////////////////////////////////////////////////////////////////////

IColorAttribute::IColorAttribute(ColorAttribute *p) { m_pImpl = p; }
IColorAttribute *IColorAttribute::SetDefault(Color c) {
  m_pImpl->SetDefault(c);
  return this;
}
IColorAttribute *IColorAttribute::Bind(slot<void(Color)> &&setter,
                                       slot<Color()> &&getter) {
  m_pImpl->Bind(std::forward<slot<void(Color)>>(setter),
                std::forward<slot<Color()>>(getter));
  return this;
}
const char *IColorAttribute::GetKey() { return m_pImpl->GetKey(); }
const char *IColorAttribute::GetDesc() { return m_pImpl->GetDesc(); }
const char *IColorAttribute::GetGroupName() { return m_pImpl->GetGroupName(); }
const char *IColorAttribute::GetParentKey() { return m_pImpl->GetParentKey(); }
const char *IColorAttribute::Get() { return m_pImpl->Get(); }

//////////////////////////////////////////////////////////////////////////

IRenderBaseAttribute::IRenderBaseAttribute(RenderBaseAttribute *p) {
  m_pImpl = p;
}
IRenderBaseAttribute *IRenderBaseAttribute::AsData() {
  m_pImpl->AsData();
  return this;
}

const char *IRenderBaseAttribute::GetKey() { return m_pImpl->GetKey(); }
const char *IRenderBaseAttribute::GetDesc() { return m_pImpl->GetDesc(); }
const char *IRenderBaseAttribute::Get() { return m_pImpl->Get(); }

const char *IRenderBaseAttribute::GetGroupName() {
  return m_pImpl->GetGroupName();
}
const char *IRenderBaseAttribute::GetParentKey() {
  return m_pImpl->GetParentKey();
}

//////////////////////////////////////////////////////////////////////////

ITextRenderBaseAttribute::ITextRenderBaseAttribute(TextRenderBaseAttribute *p) {
  m_pImpl = p;
}
ITextRenderBaseAttribute *ITextRenderBaseAttribute::AsData() {
  m_pImpl->AsData();
  return this;
}

const char *ITextRenderBaseAttribute::GetKey() { return m_pImpl->GetKey(); }
const char *ITextRenderBaseAttribute::GetDesc() { return m_pImpl->GetDesc(); }
const char *ITextRenderBaseAttribute::Get() { return m_pImpl->Get(); }
const char *ITextRenderBaseAttribute::GetGroupName() {
  return m_pImpl->GetGroupName();
}
const char *ITextRenderBaseAttribute::GetParentKey() {
  return m_pImpl->GetParentKey();
}

//////////////////////////////////////////////////////////////////////////

AttributeSerializerWrap::AttributeSerializerWrap(SerializeParam *p,
                                                 const char *szGroupName) {
  m_pImpl = new AttributeSerializer(p, szGroupName);
}
AttributeSerializerWrap::~AttributeSerializerWrap() { SAFE_DELETE(m_pImpl); }
AttributeSerializer *AttributeSerializerWrap::GetImpl() { return m_pImpl; }

IStringAttribute *AttributeSerializerWrap::AddString(
    const char *szKey, slot<void(const char *)> &&s, slot<const char *()> &&g) {
  StringAttribute *p =
      m_pImpl->AddString(szKey, std::forward<slot<void(const char *)>>(s),
                         std::forward<slot<const char *()>>(g));
  if (!p) {
    return nullptr;
  }
  return p->GetIStringAttribute();
}

// IStringAttribute*  AttributeSerializerWrap::AddString(
//     const char* szKey,
//     const std::function<void(const char*)>& s,
//     const std::function<const char*()>& g)
// {
//     StringAttribute* p = m_pImpl->AddString(szKey, s, g);
//     if (!p)
//         return nullptr;

//     return p->GetIStringAttribute();
// }

// IStringAttribute*  AttributeSerializerWrap::AddI18nString(const char* szKey,
// void* _this, pfnStringExSetter s, pfnStringGetter g)
// {
//     StringAttribute* p = m_pImpl->AddI18nString(szKey, _this, s, g);
//     if (!p)
//         return nullptr;
//
//     return p->GetIStringAttribute();
// }

// IStringAttribute*  AttributeSerializerWrap::AddI18nString(
//     const char* szKey,
//     const std::function<void(const char*, int)>& s,
//     const std::function<const char*()>& g)
// {
//     StringAttribute* p = m_pImpl->AddI18nString(szKey, s, g);
//     if (!p)
//         return nullptr;

//     return p->GetIStringAttribute();
// }

// IStringEnumAttribute*  AttributeSerializerWrap::AddStringEnum(const char*
// szKey, void* _this, pfnStringSetter s, pfnStringGetter g)
// {
// 	StringEnumAttribute* p = m_pImpl->AddStringEnum(szKey, _this, s, g);
// 	if (!p)
// 		return nullptr;

// 	return p->GetIStringEnumAttribute();
// }

IBoolAttribute *AttributeSerializerWrap::AddBool(const char *szKey,
                                                 bool &bBindValue) {
  BoolAttribute *p = m_pImpl->AddBool(szKey, bBindValue);
  if (!p)
    return nullptr;

  return p->GetIBoolAttribute();
}

IIntAttribute *AttributeSerializerWrap::AddInt(const char *szKey,
                                               int &lBindValue) {
  IntAttribute *p = m_pImpl->AddInt(szKey, lBindValue);
  if (!p)
    return nullptr;

  return p->GetIIntAttribute();
}

ILengthAttribute *AttributeSerializerWrap::AddLength(const char *szKey,
                                                     Length &bind_value) {
  LengthAttribute *p = m_pImpl->AddLength(szKey, bind_value);
  if (!p)
    return nullptr;

  return p->GetILengthAttribute();
}

IFlagsAttribute *AttributeSerializerWrap::AddFlags(const char *szKey,
                                                   int &lBindValue) {
  FlagsAttribute *p = m_pImpl->AddFlags(szKey, lBindValue);
  if (!p)
    return nullptr;

  return p->GetIFlagsAttribute();
}

IEnumAttribute *AttributeSerializerWrap::AddEnum(const char *szKey,
                                                 int &lBindValue) {
  EnumAttribute *p = m_pImpl->AddEnum(szKey, lBindValue);
  if (!p)
    return nullptr;

  return p->GetIEnumAttribute();
}
// IEnumAttribute*  AttributeSerializerWrap::AddEnum(const char* szKey, void*
// _this, pfnLongSetter s, pfnLongGetter g)
// {
//     EnumAttribute* p = m_pImpl->AddEnum(szKey, _this, s, g);
//     if (!p)
//         return nullptr;

//     return p->GetIEnumAttribute();
// }

IRectAttribute *AttributeSerializerWrap::AddRect(const char *szKey,
                                                 Rect &rcBindValue) {
  RectAttribute *p = m_pImpl->AddRect(szKey, rcBindValue);
  if (!p)
    return nullptr;

  return p->GetIRectAttribute();
}
// IRectAttribute*  AttributeSerializerWrap::AddRect(const char* szKey, void*
// _this, pfnRectSetter s, pfnRectGetter g)
// {
// 	RectAttribute* p = m_pImpl->AddRect(szKey, _this, s, g);
// 	if (!p)
// 		return nullptr;

// 	return p->GetIRectAttribute();
// }

ISizeAttribute *AttributeSerializerWrap::AddSize(const char *szKey,
                                                 Size &sBindValue) {
  SizeAttribute *p = m_pImpl->AddSize(szKey, sBindValue);
  if (!p)
    return nullptr;

  return p->GetISizeAttribute();
}
// ISizeAttribute*  AttributeSerializerWrap::AddSize(const char* szKey, void*
// _this, pfnSizeSetter s, pfnSizeGetter g)
// {
// 	SizeAttribute* p = m_pImpl->AddSize(szKey, _this, s, g);
// 	if (!p)
// 		return nullptr;

// 	return p->GetISizeAttribute();
// }

IRegion9Attribute *AttributeSerializerWrap::Add9Region(const char *szKey,
                                                       C9Region &rBindValue) {
  Region9Attribute *p = m_pImpl->Add9Region(szKey, rBindValue);
  if (!p)
    return nullptr;

  return p->GetIRegion9Attribute();
}

// IColorAttribute *AttributeSerializerWrap::AddColor(const char *szKey,
//                                                    Color *&pBindValue) {
//   ColorAttribute *p = m_pImpl->AddColor(szKey, pBindValue);
//   if (!p)
//     return nullptr;

//   return p->GetIColorAttribute();
// }
IColorAttribute *AttributeSerializerWrap::AddColor(const char *szKey,
                                                   Color &pBindValue) {
  ColorAttribute *p = m_pImpl->AddColor(szKey, pBindValue);
  if (!p)
    return nullptr;

  return p->GetIColorAttribute();
}
IColorAttribute *AttributeSerializerWrap::AddColor(const char *szKey,
                                                   slot<void(Color)> &&setter,
                                                   slot<Color()> &&getter) {
  ColorAttribute *p =
      m_pImpl->AddColor(szKey, std::forward<slot<void(Color)>>(setter),
                        std::forward<slot<Color()>>(getter));
  if (!p)
    return nullptr;

  return p->GetIColorAttribute();
}
// IRegion9Attribute*  AttributeSerializerWrap::Add9Region(const char* szKey,
// void* _this, pfnRectSetter s, pfnRectGetter g)
// {
// 	Region9Attribute* p = m_pImpl->Add9Region(szKey, _this, s, g);
// 	if (!p)
// 		return nullptr;

// 	return p->GetIRegion9Attribute();
// }

IRenderBaseAttribute *
AttributeSerializerWrap::AddRenderBase(const char *szPrefix,
                                       std::shared_ptr<IRenderBase> &pBindValue) {
  RenderBaseAttribute *p = m_pImpl->AddRenderBase(
      szPrefix, pBindValue);
  if (!p)
    return nullptr;

  return p->GetIRenderBaseAttribute();
}

ITextRenderBaseAttribute *
AttributeSerializerWrap::AddTextRenderBase(const char *szPrefix,
                                           std::shared_ptr<ITextRenderBase> &pBindValue) {
  TextRenderBaseAttribute *p = m_pImpl->AddTextRenderBase(
      szPrefix, pBindValue);
  if (!p)
    return nullptr;

  return p->GetITextRenderBaseAttribute();
}
//////////////////////////////////////////////////////////////////////////

IAttributeEditorProxy::IAttributeEditorProxy(IUIEditor *p) {
  m_pImpl = new AttributeEditorProxy(*this, p);
}
IAttributeEditorProxy::~IAttributeEditorProxy() { SAFE_DELETE(m_pImpl); }
AttributeEditorProxy *IAttributeEditorProxy::GetImpl() { return m_pImpl; }

void IAttributeEditorProxy::LoadAttribute2Editor(IObject *pObj) {
  m_pImpl->LoadAttribute2Editor(pObj);
}
UpdateAttribute2EditorResult
IAttributeEditorProxy::UpdateAttribute2Editor(const char *szKey) {
  return m_pImpl->UpdateAttribute2Editor(szKey);
}
bool IAttributeEditorProxy::SetAttribute(const char *szKey,
                                         const char *szValue) {
  return m_pImpl->Set(szKey, szValue);
}

const char *IAttributeEditorProxy::GetAttribute(const char *szKey) {
  return m_pImpl->Get(szKey);
}