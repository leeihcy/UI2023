#ifndef _UI_IATTRIBUTE_H_
#define _UI_IATTRIBUTE_H_
#include <functional>
#include "sdk/include/interface.h"
#include "sdk/include/common/signalslot/slot.h"

namespace ui {
struct IObject;
struct SERIALIZEDATA;
struct Rect;
struct Size;
struct Color;
struct IRenderBase;
struct ITextRenderBase;
struct IUIEditor;
class C9Region;

// 属性类型枚举，默认支持的一些属性类型。
// 对于外部自定义的属性类型，可以用一个大数字来定义(long)
enum DEFAULT_ATTRIBUTE_TYPE {
  ATTRIBUTE_TYPE_STRING = 0,
  ATTRIBUTE_TYPE_I18N_STRING,
  ATTRIBUTE_TYPE_CHARARRAY,
  ATTRIBUTE_TYPE_BOOL,
  ATTRIBUTE_TYPE_RECT,
  ATTRIBUTE_TYPE_LONG,
  ATTRIBUTE_TYPE_SIZE,
  ATTRIBUTE_TYPE_FLAGS,      // 多选，对应数值
  ATTRIBUTE_TYPE_STRINGENUM, // 从字符串列表中，选择一项，取其内容
  ATTRIBUTE_TYPE_ENUM, // 从字符串列表中，选择一项，对应一个数值
  ATTRIBUTE_TYPE_9REGION,
  ATTRIBUTE_TYPE_UICOLOR,
  ATTRIBUTE_TYPE_RENDERBASE = 100,
  ATTRIBUTE_TYPE_TEXTRENDERBASE,
};

//
// http://stackoverflow.com/questions/8121320/get-memory-address-of-member-function
//
// C++ does not support converting a pointer to a member function to
// an arbitrary pointer.To get a raw pointer, the address of the member
// function must be moved into a temporrary member - function pointer,
// then passed by taking it's address, then de-referencing it. Fortunately,
// the compiler will optimize the code to remove the extra pointer
// operations.

// tricky cast
// void* pPtr = (void*&)&MyClass::Func; // this works

// c++中获取成员函数指针作为参数会报error
// 这里使用一个模板来获取成员函数指针
//
// e.g.
// memfun_cast<pfnLongSetter>(&Class::MemberFunction)
//
template <class To, class From> To memfun_cast(From memfunc) {
  union {
    From memfunc;
    To addr;
  } u;
  u.memfunc = memfunc;
  return u.addr;
}

typedef void (*pfnEnumAliasCallback)(const wchar_t *szText, long lValue, long,
                                     long);
typedef void (*pfnEnumStringEnumCallback)(const wchar_t *szText, long, long);

enum LONGATTR_DPI_SCALE_TYPE {
  LONGATTR_DPI_SCALE_DISABLE,     // 不是dpi属性
  LONGATTR_DPI_SCALE_ENABLE,      // 所有情况下都拉伸
  LONGATTR_DPI_SCALE_GRATETHAN_0, // 大于0的情况下拉伸
};

class LongAttribute;
struct UIAPI ILongAttribute {
  ILongAttribute(LongAttribute *);
  ILongAttribute *SetDefault(long l);
  ILongAttribute *AddAlias(long, const wchar_t *);
  ILongAttribute *AsData();
  ILongAttribute *SetDpiScaleType(LONGATTR_DPI_SCALE_TYPE e);

  const wchar_t *GetKey();
  const wchar_t *GetDesc();
  const wchar_t *GetGroupName();
  const wchar_t *GetParentKey();
  const wchar_t *Get();
  long GetLong();

  unsigned int GetAliasCount();
  long EnumAlias(pfnEnumAliasCallback, long, long);

private:
  LongAttribute *m_pImpl;
};

class BoolAttribute;
struct UIAPI IBoolAttribute {
  IBoolAttribute(BoolAttribute *);
  IBoolAttribute *SetDefault(bool b);
  IBoolAttribute *AsData();

  const wchar_t *GetKey();
  const wchar_t *GetDesc();
  const wchar_t *GetGroupName();
  const wchar_t *GetParentKey();
  const wchar_t *Get();
  bool GetBool();
  bool GetDefaultBool();

  IBoolAttribute *ReloadOnChanged();

private:
  BoolAttribute *m_pImpl;
};

class StringAttribute;
struct UIAPI IStringAttribute {
  IStringAttribute(StringAttribute *);
  IStringAttribute *SetDefault(const wchar_t *);
  IStringAttribute *AsData();

  const wchar_t *GetKey();
  const wchar_t *GetDesc();
  const wchar_t *GetGroupName();
  const wchar_t *GetParentKey();
  const wchar_t *Get();

private:
  StringAttribute *m_pImpl;
};

class CharArrayAttribute;
struct ICharArrayAttribute {
  ICharArrayAttribute(CharArrayAttribute *);

private:
  CharArrayAttribute *m_pImpl;
};

class EnumAttribute;
struct UIAPI IEnumAttribute {
  IEnumAttribute(EnumAttribute *);
  IEnumAttribute *AddOption(long, const wchar_t *);
  IEnumAttribute *SetDefault(long l);
  IEnumAttribute *AsData();

  const wchar_t *GetKey();
  const wchar_t *GetDesc();
  const wchar_t *GetGroupName();
  const wchar_t *GetParentKey();
  const wchar_t *Get();
  long GetLong();

  unsigned int GetAliasCount();
  long EnumAlias(pfnEnumAliasCallback, long, long);

  IEnumAttribute *ReloadOnChanged();

private:
  EnumAttribute *m_pImpl;
};

class FlagsAttribute;
struct UIAPI IFlagsAttribute {
  IFlagsAttribute(FlagsAttribute *);
  IFlagsAttribute *AddFlag(long, const wchar_t *);
  IFlagsAttribute *SetDefault(long l);
  IFlagsAttribute *AsData();

  const wchar_t *GetKey();
  const wchar_t *GetDesc();
  const wchar_t *GetGroupName();
  const wchar_t *GetParentKey();
  const wchar_t *Get();
  long GetLong();

  unsigned int GetAliasCount();
  long EnumAlias(pfnEnumAliasCallback, long, long);

private:
  FlagsAttribute *m_pImpl;
};

class RectAttribute;
struct UIAPI IRectAttribute {
  IRectAttribute(RectAttribute *);
  IRectAttribute *AsData();

  const wchar_t *GetKey();
  const wchar_t *GetDesc();
  const wchar_t *GetGroupName();
  const wchar_t *GetParentKey();
  const wchar_t *Get();

private:
  RectAttribute *m_pImpl;
};

class SizeAttribute;
struct UIAPI ISizeAttribute {
  ISizeAttribute(SizeAttribute *);
  ISizeAttribute *AsData();

  const wchar_t *GetKey();
  const wchar_t *GetDesc();
  const wchar_t *GetGroupName();
  const wchar_t *GetParentKey();
  const wchar_t *Get();

private:
  SizeAttribute *m_pImpl;
};

class Region9Attribute;
struct UIAPI IRegion9Attribute {
  IRegion9Attribute(Region9Attribute *);
  IRegion9Attribute *AsData();

  const wchar_t *GetKey();
  const wchar_t *GetDesc();
  const wchar_t *GetGroupName();
  const wchar_t *GetParentKey();
  const wchar_t *Get();

private:
  Region9Attribute *m_pImpl;
};

class StringEnumAttribute;
struct UIAPI IStringEnumAttribute {
  IStringEnumAttribute(StringEnumAttribute *);
  IStringEnumAttribute *AsData();

  const wchar_t *GetKey();
  const wchar_t *GetDesc();
  const wchar_t *GetGroupName();
  const wchar_t *GetParentKey();
  const wchar_t *Get();
  void EnumString(pfnEnumStringEnumCallback, long, long);

  IStringEnumAttribute *FillRenderBaseTypeData();
  IStringEnumAttribute *FillTextRenderBaseTypeData();
  IStringEnumAttribute *ReloadOnChanged();

private:
  StringEnumAttribute *m_pImpl;
};

class ColorAttribute;
struct UIAPI IColorAttribute {
  IColorAttribute(ColorAttribute *);

  const wchar_t *GetKey();
  const wchar_t *GetDesc();
  const wchar_t *GetGroupName();
  const wchar_t *GetParentKey();
  const wchar_t *Get();

private:
  ColorAttribute *m_pImpl;
};

class RenderBaseAttribute;
struct UIAPI IRenderBaseAttribute {
  IRenderBaseAttribute(RenderBaseAttribute *);
  IRenderBaseAttribute *AsData();

  const wchar_t *GetKey();
  const wchar_t *GetDesc();
  const wchar_t *Get();

  const wchar_t *GetGroupName();
  const wchar_t *GetParentKey();

private:
  RenderBaseAttribute *m_pImpl;
};
class TextRenderBaseAttribute;
struct UIAPI ITextRenderBaseAttribute {
  ITextRenderBaseAttribute(TextRenderBaseAttribute *);
  ITextRenderBaseAttribute *AsData();

  const wchar_t *GetKey();
  const wchar_t *GetDesc();
  const wchar_t *Get();

  const wchar_t *GetGroupName();
  const wchar_t *GetParentKey();

private:
  TextRenderBaseAttribute *m_pImpl;
};

class AttributeSerializer;
struct UIAPI AttributeSerializerWrap {
  AttributeSerializerWrap(SERIALIZEDATA *, const wchar_t *szGroupName);
  ~AttributeSerializerWrap();
  AttributeSerializer *GetImpl();

  // IStringAttribute*  AddString(const wchar_t* szKey, void* _this,
  // pfnStringSetter s, pfnStringGetter g);
  IStringAttribute *AddString(const wchar_t *szKey,
                              slot<void(const wchar_t *)> &&s,
                              slot<const wchar_t *()> &&g);
  // IStringAttribute*  AddString(const wchar_t* szKey, const
  // std::function<void(const wchar_t*)>& s, const std::function<const
  // wchar_t*()>& g); IStringAttribute*  AddI18nString(const wchar_t* szKey,
  // void* _this, pfnStringExSetter s, pfnStringGetter g); IStringAttribute*
  // AddI18nString(const wchar_t* szKey, const std::function<void(const
  // wchar_t*, int)>& s, const std::function<const wchar_t*()>& g);
  // IStringEnumAttribute*  AddStringEnum(const wchar_t*, void* _this,
  // pfnStringSetter s, pfnStringGetter g);

  IBoolAttribute *AddBool(const wchar_t *szKey, bool &bBindValue);
  // IBoolAttribute*  AddBool(const wchar_t* szKey, void* _this, pfnBoolSetter
  // s, pfnBoolGetter g);

  ILongAttribute *AddLong(const wchar_t *szKey, long &lBindValue);
  ILongAttribute *AddLong(const wchar_t *szKey, int &lBindValue);
  // ILongAttribute*  AddLong(const wchar_t* szKey, void* _this, pfnLongSetter
  // s, pfnLongGetter g);

  IFlagsAttribute *AddFlags(const wchar_t *, long &lBindValue);
  IEnumAttribute *AddEnum(const wchar_t *szKey, long &lBindValue);
  // IEnumAttribute*  AddEnum(const wchar_t* szKey, void* _this, pfnLongSetter
  // s, pfnLongGetter g);

  IRectAttribute *AddRect(const wchar_t *, Rect &rcBindValue);
  // IRectAttribute*  AddRect(const wchar_t*, void* _this, pfnRectSetter s,
  // pfnRectGetter g);

  ISizeAttribute *AddSize(const wchar_t *, Size &sBindValue);
  // ISizeAttribute*  AddSize(const wchar_t*, void* _this, pfnSizeSetter s,
  // pfnSizeGetter g);

  IColorAttribute *AddColor(const wchar_t *, Color *&pBindValue);
  IColorAttribute *AddColor(const wchar_t *, Color &pBindValue);
  IRegion9Attribute *Add9Region(const wchar_t *, C9Region &rBindValue);
  // IRegion9Attribute*  Add9Region(const wchar_t*, void* _this, pfnRectSetter
  // s, pfnRectGetter g);

  IRenderBaseAttribute *AddRenderBase(const wchar_t *szPrefix, IObject *pObj,
                                      IRenderBase *&pBindValue);
  ITextRenderBaseAttribute *AddTextRenderBase(const wchar_t *szPrefix,
                                              IObject *pObj,
                                              ITextRenderBase *&pBindValue);

private:
  AttributeSerializer *m_pImpl;
};

enum UpdateAttribute2EditorResult {
  UpdateAttribute2EditorFailed,
  UpdateAttribute2EditorSuccess,
  UpdateAttribute2EditorNeedReload,
};

class AttributeEditorProxy;
struct UIAPI IAttributeEditorProxy {
public:
  IAttributeEditorProxy(IUIEditor *);
  ~IAttributeEditorProxy();
  AttributeEditorProxy *GetImpl();

  bool SetAttribute(const wchar_t *szKey, const wchar_t *szValue);
  const wchar_t *GetAttribute(const wchar_t *szKey);
  void LoadAttribute2Editor(IObject *);
  UpdateAttribute2EditorResult UpdateAttribute2Editor(const wchar_t *szKey);

private:
  AttributeEditorProxy *m_pImpl;
};

} // namespace ui

#endif