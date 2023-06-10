#ifndef _UI_IATTRIBUTE_H_
#define _UI_IATTRIBUTE_H_
#include <functional>

namespace UI
{

// 属性类型枚举，默认支持的一些属性类型。
// 对于外部自定义的属性类型，可以用一个大数字来定义(long)
enum DEFAULT_ATTRIBUTE_TYPE
{
    ATTRIBUTE_TYPE_STRING = 0,
    ATTRIBUTE_TYPE_I18N_STRING,
    ATTRIBUTE_TYPE_CHARARRAY,
    ATTRIBUTE_TYPE_BOOL,
	ATTRIBUTE_TYPE_RECT,
    ATTRIBUTE_TYPE_LONG,
	ATTRIBUTE_TYPE_SIZE,
    ATTRIBUTE_TYPE_FLAGS,         // 多选，对应数值
    ATTRIBUTE_TYPE_STRINGENUM,    // 从字符串列表中，选择一项，取其内容
    ATTRIBUTE_TYPE_ENUM,          // 从字符串列表中，选择一项，对应一个数值
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
template<class To, class From>
To memfun_cast(From memfunc)
{
    union
    {
        From memfunc;
        To addr;
    }u;
    u.memfunc = memfunc;
    return u.addr;
}

// 属性get/set 成员函数定义

typedef void  (__thiscall *pfnStringSetter)(LPCTSTR);
typedef LPCTSTR  (__thiscall *pfnStringGetter)();

// typedef void(__thiscall *pfnStringExSetter)(LPCTSTR, int);

typedef void  (__thiscall *pfnBoolSetter)(bool);
typedef bool  (__thiscall *pfnBoolGetter)();

typedef void  (__thiscall *pfnLongSetter)(long);
typedef long  (__thiscall *pfnLongGetter)();

typedef void  (__thiscall *pfnRectSetter)(LPRECT);
typedef void  (__thiscall *pfnRectGetter)(LPRECT);

typedef void  (__thiscall *pfnSizeSetter)(LPSIZE);
typedef void  (__thiscall *pfnSizeGetter)(LPSIZE);
typedef void  (__thiscall *pfnPointSetter)(LPPOINT);
typedef void  (__thiscall *pfnPointGetter)(LPPOINT);

typedef void  (__thiscall *pfn9RegionSetter)(C9Region*);
typedef void  (__thiscall *pfn9RegionGetter)(C9Region*);

typedef void  (*pfnEnumAliasCallback)(LPCTSTR szText, long lValue, WPARAM, LPARAM);
typedef void  (*pfnEnumStringEnumCallback)(LPCTSTR szText, WPARAM, LPARAM);



enum LONGATTR_DPI_SCALE_TYPE
{
	LONGATTR_DPI_SCALE_DISABLE, // 不是dpi属性
	LONGATTR_DPI_SCALE_ENABLE,     // 所有情况下都拉伸
	LONGATTR_DPI_SCALE_GRATETHAN_0,  // 大于0的情况下拉伸
};

class LongAttribute;
interface UIAPI ILongAttribute
{
    ILongAttribute(LongAttribute*);
    ILongAttribute*  SetDefault(long l);
    ILongAttribute*  AddAlias(long, LPCTSTR);
    ILongAttribute*  AsData();
	ILongAttribute*  SetDpiScaleType(LONGATTR_DPI_SCALE_TYPE e);


    LPCTSTR  GetKey();
    LPCTSTR  GetDesc();
	LPCTSTR  GetGroupName();
    LPCTSTR  GetParentKey();
    LPCTSTR  Get();
    long  GetLong();

	unsigned int  GetAliasCount();
	long  EnumAlias(pfnEnumAliasCallback, WPARAM, LPARAM);

private:
    LongAttribute*  m_pImpl;
};

class BoolAttribute;
interface UIAPI IBoolAttribute
{
    IBoolAttribute(BoolAttribute*);
    IBoolAttribute*  SetDefault(bool b);
    IBoolAttribute*  AsData();

    LPCTSTR  GetKey();
    LPCTSTR  GetDesc();
	LPCTSTR  GetGroupName();
    LPCTSTR  GetParentKey();
    LPCTSTR  Get();
    bool  GetBool();
    bool  GetDefaultBool();

    IBoolAttribute*  ReloadOnChanged();
private:
    BoolAttribute*  m_pImpl;
};


class StringAttribute;
interface UIAPI IStringAttribute
{
    IStringAttribute(StringAttribute*);
    IStringAttribute*  SetDefault(LPCTSTR);
    IStringAttribute*  AsData();

    LPCTSTR  GetKey();
    LPCTSTR  GetDesc();
	LPCTSTR  GetGroupName();
    LPCTSTR  GetParentKey();
    LPCTSTR  Get();
	
private:
    StringAttribute*  m_pImpl;
};

class CharArrayAttribute;
interface ICharArrayAttribute
{
    ICharArrayAttribute(CharArrayAttribute*);
private:
    CharArrayAttribute*  m_pImpl;
};

class EnumAttribute;
interface UIAPI IEnumAttribute
{
    IEnumAttribute(EnumAttribute*);
    IEnumAttribute*  AddOption(long, LPCTSTR);
    IEnumAttribute*  SetDefault(long l);
    IEnumAttribute*  AsData();

	LPCTSTR  GetKey();
	LPCTSTR  GetDesc();
	LPCTSTR  GetGroupName();
    LPCTSTR  GetParentKey();
    LPCTSTR  Get();
	long  GetLong();

	unsigned int  GetAliasCount();
	long  EnumAlias(pfnEnumAliasCallback, WPARAM, LPARAM);

	IEnumAttribute*  ReloadOnChanged();

private:
    EnumAttribute*  m_pImpl;
};

class FlagsAttribute;
interface UIAPI IFlagsAttribute
{
    IFlagsAttribute(FlagsAttribute*);
    IFlagsAttribute*  AddFlag(long, LPCTSTR);
    IFlagsAttribute*  SetDefault(long l);
    IFlagsAttribute*  AsData();

    LPCTSTR  GetKey();
    LPCTSTR  GetDesc();
    LPCTSTR  GetGroupName();
    LPCTSTR  GetParentKey();
    LPCTSTR  Get();
    long  GetLong();

    unsigned int  GetAliasCount();
    long  EnumAlias(pfnEnumAliasCallback, WPARAM, LPARAM);
private:
    FlagsAttribute*  m_pImpl;
};

class RectAttribute;
interface UIAPI IRectAttribute
{
    IRectAttribute(RectAttribute*);
    IRectAttribute*  AsData();

    LPCTSTR  GetKey();
    LPCTSTR  GetDesc();
	LPCTSTR  GetGroupName();
    LPCTSTR  GetParentKey();
    LPCTSTR  Get();
private:
    RectAttribute*  m_pImpl;
};

class SizeAttribute;
interface UIAPI ISizeAttribute
{
	ISizeAttribute(SizeAttribute*);
	ISizeAttribute*  AsData();

	LPCTSTR  GetKey();
	LPCTSTR  GetDesc();
	LPCTSTR  GetGroupName();
	LPCTSTR  GetParentKey();
	LPCTSTR  Get();
private:
	SizeAttribute*  m_pImpl;
};


class Region9Attribute;
interface UIAPI IRegion9Attribute
{
	IRegion9Attribute(Region9Attribute*);
	IRegion9Attribute*  AsData();

	LPCTSTR  GetKey();
	LPCTSTR  GetDesc();
	LPCTSTR  GetGroupName();
    LPCTSTR  GetParentKey();
	LPCTSTR  Get();
private:
	Region9Attribute*  m_pImpl;
};

class StringEnumAttribute;
interface UIAPI IStringEnumAttribute
{
    IStringEnumAttribute(StringEnumAttribute*);
    IStringEnumAttribute*  AsData();

    LPCTSTR  GetKey();
    LPCTSTR  GetDesc();
	LPCTSTR  GetGroupName();
    LPCTSTR  GetParentKey();
    LPCTSTR  Get();
    void  EnumString(pfnEnumStringEnumCallback, WPARAM, LPARAM);

	IStringEnumAttribute*  FillRenderBaseTypeData();
	IStringEnumAttribute*  FillTextRenderBaseTypeData();
	IStringEnumAttribute*  ReloadOnChanged();

private:
    StringEnumAttribute*  m_pImpl;
};

class ColorAttribute;
interface UIAPI IColorAttribute
{
	IColorAttribute(ColorAttribute*);
	
	LPCTSTR  GetKey();
	LPCTSTR  GetDesc();
	LPCTSTR  GetGroupName();
	LPCTSTR  GetParentKey();
	LPCTSTR  Get();
private:
	ColorAttribute*  m_pImpl;
};

class RenderBaseAttribute;
interface UIAPI IRenderBaseAttribute
{
    IRenderBaseAttribute(RenderBaseAttribute*);
    IRenderBaseAttribute*  AsData();

    LPCTSTR  GetKey();
    LPCTSTR  GetDesc();
    LPCTSTR  Get();

	LPCTSTR  GetGroupName();
	LPCTSTR  GetParentKey();

private:
    RenderBaseAttribute*  m_pImpl;
};
class TextRenderBaseAttribute;
interface UIAPI ITextRenderBaseAttribute
{
    ITextRenderBaseAttribute(TextRenderBaseAttribute*);
    ITextRenderBaseAttribute*  AsData();

    LPCTSTR  GetKey();
    LPCTSTR  GetDesc();
    LPCTSTR  Get();

	LPCTSTR  GetGroupName();
	LPCTSTR  GetParentKey();

private:
    TextRenderBaseAttribute*  m_pImpl;
};

class AttributeSerializer;
interface UIAPI AttributeSerializerWrap
{
    AttributeSerializerWrap(SERIALIZEDATA*, LPCTSTR szGroupName);
    ~AttributeSerializerWrap();
	AttributeSerializer*  GetImpl();

    IStringAttribute*  AddString(LPCTSTR szKey, void* _this, pfnStringSetter s, pfnStringGetter g);
    IStringAttribute*  AddString(LPCTSTR szKey, const std::function<void(LPCTSTR)>& s, const std::function<LPCTSTR()>& g);
    // IStringAttribute*  AddI18nString(LPCTSTR szKey, void* _this, pfnStringExSetter s, pfnStringGetter g);
    IStringAttribute*  AddI18nString(LPCTSTR szKey, const std::function<void(LPCTSTR, int)>& s, const std::function<LPCTSTR()>& g);
	IStringEnumAttribute*  AddStringEnum(LPCTSTR, void* _this, pfnStringSetter s, pfnStringGetter g);

    IBoolAttribute*  AddBool(LPCTSTR szKey, bool& bBindValue);
    IBoolAttribute*  AddBool(LPCTSTR szKey, void* _this, pfnBoolSetter s, pfnBoolGetter g);

    ILongAttribute*  AddLong(LPCTSTR szKey, long& lBindValue);
	ILongAttribute*  AddLong(LPCTSTR szKey, int& lBindValue);
    ILongAttribute*  AddLong(LPCTSTR szKey, void* _this, pfnLongSetter s, pfnLongGetter g);

	IFlagsAttribute*  AddFlags(LPCTSTR, long& lBindValue);
    IEnumAttribute*  AddEnum(LPCTSTR szKey, long& lBindValue);
    IEnumAttribute*  AddEnum(LPCTSTR szKey, void* _this, pfnLongSetter s, pfnLongGetter g);

	IRectAttribute*  AddRect(LPCTSTR, RECT& rcBindValue);
	IRectAttribute*  AddRect(LPCTSTR, void* _this, pfnRectSetter s, pfnRectGetter g);

	ISizeAttribute*  AddSize(LPCTSTR, SIZE& sBindValue);
	ISizeAttribute*  AddSize(LPCTSTR, void* _this, pfnSizeSetter s, pfnSizeGetter g);

	IColorAttribute*  AddColor(LPCTSTR, Color*& pBindValue);
	IColorAttribute*  AddColor(LPCTSTR, Color& pBindValue);
	IRegion9Attribute*  Add9Region(LPCTSTR, C9Region& rBindValue);
	IRegion9Attribute*  Add9Region(LPCTSTR, void* _this, pfnRectSetter s, pfnRectGetter g);

	IRenderBaseAttribute*  AddRenderBase(LPCTSTR szPrefix, IObject* pObj, IRenderBase*& pBindValue);
	ITextRenderBaseAttribute*  AddTextRenderBase(LPCTSTR szPrefix, IObject* pObj, ITextRenderBase*& pBindValue);

private:
    AttributeSerializer*  m_pImpl;
};

enum UpdateAttribute2EditorResult
{
    UpdateAttribute2EditorFailed,
    UpdateAttribute2EditorSuccess,
    UpdateAttribute2EditorNeedReload,
};

class AttributeEditorProxy;
interface UIAPI IAttributeEditorProxy
{
public:
    IAttributeEditorProxy(IUIEditor*);
    ~IAttributeEditorProxy();
    AttributeEditorProxy*  GetImpl();    

    bool  SetAttribute(LPCTSTR szKey, LPCTSTR szValue);
    LPCTSTR  GetAttribute(LPCTSTR szKey);
    void  LoadAttribute2Editor(IObject*);
	UpdateAttribute2EditorResult  UpdateAttribute2Editor(LPCTSTR szKey);

private:
    AttributeEditorProxy*  m_pImpl;
};

}

#endif