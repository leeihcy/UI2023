#ifndef _UI_IATTRIBUTE_H_
#define _UI_IATTRIBUTE_H_
#include "sdk/include/common/signalslot/slot.h"
#include "sdk/include/interface.h"
#include <functional>

namespace ui {
struct IObject;
struct SerializeParam;
struct Rect;
struct Size;
struct Color;
struct Length;
struct IRenderBase;
struct ITextRenderBase;
struct IUIEditor;
struct C9Region;

// 属性类型枚举，默认支持的一些属性类型。
// 对于外部自定义的属性类型，可以用一个大数字来定义(int)
enum DEFAULT_ATTRIBUTE_TYPE {
  ATTRIBUTE_TYPE_STRING = 0,
  ATTRIBUTE_TYPE_I18N_STRING,
  ATTRIBUTE_TYPE_CHARARRAY,
  ATTRIBUTE_TYPE_BOOL,
  ATTRIBUTE_TYPE_RECT,
  ATTRIBUTE_TYPE_INTEGER,
  ATTRIBUTE_TYPE_LENGTH,
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

typedef void (*pfnEnumAliasCallback)(const char *szText, int lValue, llong,
                                     llong);
typedef void (*pfnEnumStringEnumCallback)(const char *szText, llong, llong);

enum LONGATTR_DPI_SCALE_TYPE {
  LONGATTR_DPI_SCALE_DISABLE,     // 不是dpi属性
  LONGATTR_DPI_SCALE_ENABLE,      // 所有情况下都拉伸
  LONGATTR_DPI_SCALE_GRATETHAN_0, // 大于0的情况下拉伸
};

class IntAttribute;
struct UIAPI IIntAttribute {
  IIntAttribute(IntAttribute *);
  IIntAttribute *SetDefault(int l);
  IIntAttribute *AddAlias(int, const char *);
  IIntAttribute *AsData();
  IIntAttribute *SetDpiScaleType(LONGATTR_DPI_SCALE_TYPE e);

  const char *GetKey();
  const char *GetDesc();
  const char *GetGroupName();
  const char *GetParentKey();
  const char *Get();
  int GetLong();

  unsigned int GetAliasCount();
  int EnumAlias(pfnEnumAliasCallback, int, int);

private:
  IntAttribute *m_pImpl;
};

class LengthAttribute;
struct UIAPI ILengthAttribute {
  ILengthAttribute(LengthAttribute *);
  ILengthAttribute *SetDefault(Length l);
  ILengthAttribute *AsData();
  ILengthAttribute *SetDpiScaleType(LONGATTR_DPI_SCALE_TYPE e);

  const char *GetKey();
  const char *GetDesc();
  const char *GetGroupName();
  const char *GetParentKey();
  const char *Get();
  Length GetLength();

private:
  LengthAttribute *m_pImpl; // raw_ptr
};

class BoolAttribute;
struct UIAPI IBoolAttribute {
  IBoolAttribute(BoolAttribute *);
  IBoolAttribute *SetDefault(bool b);
  IBoolAttribute *AsData();

  const char *GetKey();
  const char *GetDesc();
  const char *GetGroupName();
  const char *GetParentKey();
  const char *Get();
  bool GetBool();
  bool GetDefaultBool();

  IBoolAttribute *ReloadOnChanged();

private:
  BoolAttribute *m_pImpl;
};

class StringAttribute;
struct UIAPI IStringAttribute {
  IStringAttribute(StringAttribute *);
  IStringAttribute *SetDefault(const char *);
  IStringAttribute *AsData();

  const char *GetKey();
  const char *GetDesc();
  const char *GetGroupName();
  const char *GetParentKey();
  const char *Get();

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
  IEnumAttribute *AddOption(int, const char *);
  IEnumAttribute *SetDefault(int l);
  IEnumAttribute *AsData();

  const char *GetKey();
  const char *GetDesc();
  const char *GetGroupName();
  const char *GetParentKey();
  const char *Get();
  int GetLong();

  unsigned int GetAliasCount();
  int EnumAlias(pfnEnumAliasCallback, int, int);

  IEnumAttribute *ReloadOnChanged();

private:
  EnumAttribute *m_pImpl;
};

class FlagsAttribute;
struct UIAPI IFlagsAttribute {
  IFlagsAttribute(FlagsAttribute *);
  IFlagsAttribute *AddFlag(int, const char *);
  IFlagsAttribute *SetDefault(int l);
  IFlagsAttribute *AsData();

  const char *GetKey();
  const char *GetDesc();
  const char *GetGroupName();
  const char *GetParentKey();
  const char *Get();
  int GetLong();

  unsigned int GetAliasCount();
  int EnumAlias(pfnEnumAliasCallback, int, int);

private:
  FlagsAttribute *m_pImpl;
};

class RectAttribute;
struct UIAPI IRectAttribute {
  IRectAttribute(RectAttribute *);
  IRectAttribute *AsData();

  IRectAttribute *SetCompatibleKey(const char *key);
  IRectAttribute *SetDefault(Rect);

  const char *GetKey();
  const char *GetDesc();
  const char *GetGroupName();
  const char *GetParentKey();
  const char *Get();

private:
  RectAttribute *m_pImpl;
};

class SizeAttribute;
struct UIAPI ISizeAttribute {
  ISizeAttribute(SizeAttribute *);
  ISizeAttribute *AsData();

  const char *GetKey();
  const char *GetDesc();
  const char *GetGroupName();
  const char *GetParentKey();
  const char *Get();

private:
  SizeAttribute *m_pImpl;
};

class Region9Attribute;
struct UIAPI IRegion9Attribute {
  IRegion9Attribute(Region9Attribute *);
  IRegion9Attribute *AsData();

  const char *GetKey();
  const char *GetDesc();
  const char *GetGroupName();
  const char *GetParentKey();
  const char *Get();

private:
  Region9Attribute *m_pImpl;
};

class StringEnumAttribute;
struct UIAPI IStringEnumAttribute {
  IStringEnumAttribute(StringEnumAttribute *);
  IStringEnumAttribute *AsData();

  const char *GetKey();
  const char *GetDesc();
  const char *GetGroupName();
  const char *GetParentKey();
  const char *Get();
  void EnumString(pfnEnumStringEnumCallback, int, int);

  IStringEnumAttribute *FillRenderBaseTypeData();
  IStringEnumAttribute *FillTextRenderBaseTypeData();
  IStringEnumAttribute *ReloadOnChanged();

private:
  StringEnumAttribute *m_pImpl;
};

class ColorAttribute;
struct UIAPI IColorAttribute {
  IColorAttribute(ColorAttribute *);
  IColorAttribute *SetDefault(Color c);
  IColorAttribute *Bind(slot<void(Color)> &&setter, slot<Color()> &&getter);

  const char *GetKey();
  const char *GetDesc();
  const char *GetGroupName();
  const char *GetParentKey();
  const char *Get();

private:
  ColorAttribute *m_pImpl;
};

class RenderBaseAttribute;
struct UIAPI IRenderBaseAttribute {
  IRenderBaseAttribute(RenderBaseAttribute *);
  IRenderBaseAttribute *AsData();

  const char *GetKey();
  const char *GetDesc();
  const char *Get();

  const char *GetGroupName();
  const char *GetParentKey();

private:
  RenderBaseAttribute *m_pImpl;
};
class TextRenderBaseAttribute;
struct UIAPI ITextRenderBaseAttribute {
  ITextRenderBaseAttribute(TextRenderBaseAttribute *);
  ITextRenderBaseAttribute *AsData();

  const char *GetKey();
  const char *GetDesc();
  const char *Get();

  const char *GetGroupName();
  const char *GetParentKey();

private:
  TextRenderBaseAttribute *m_pImpl;
};

class AttributeSerializer;
struct UIAPI AttributeSerializerWrap {
  AttributeSerializerWrap(SerializeParam *, const char *szGroupName);
  ~AttributeSerializerWrap();
  AttributeSerializer *GetImpl();

  IStringAttribute *AddString(const char *szKey, slot<void(const char *)> &&s,
                              slot<const char *()> &&g);
  IBoolAttribute *AddBool(const char *szKey, bool &bBindValue);
  IIntAttribute *AddInt(const char *szKey, int &lBindValue);
  ILengthAttribute *AddLength(const char *szKey, Length &bind_value);
  IFlagsAttribute *AddFlags(const char *, int &lBindValue);
  IEnumAttribute *AddEnum(const char *szKey, int &lBindValue);
  IRectAttribute *AddRect(const char *, Rect &rcBindValue);
  ISizeAttribute *AddSize(const char *, Size &sBindValue);
  IColorAttribute *AddColor(const char *, Color &pBindValue);
  IColorAttribute *AddColor(const char *szKey, slot<void(Color)> &&setter,
                            slot<Color()> &&getter);
  IRegion9Attribute *Add9Region(const char *, C9Region &rBindValue);
  IRenderBaseAttribute *AddRenderBase(const char *szPrefix, IObject *pObj,
                                      IRenderBase *&pBindValue);
  ITextRenderBaseAttribute *AddTextRenderBase(const char *szPrefix,
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

  bool SetAttribute(const char *szKey, const char *szValue);
  const char *GetAttribute(const char *szKey);
  void LoadAttribute2Editor(IObject *);
  UpdateAttribute2EditorResult UpdateAttribute2Editor(const char *szKey);

private:
  AttributeEditorProxy *m_pImpl;
};

} // namespace ui

#endif