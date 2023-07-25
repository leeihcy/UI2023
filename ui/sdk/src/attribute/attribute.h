#pragma once
#include "include/interface/iattribute.h"
#include "string_attribute.h"
#include "enum_attribute.h"
#include "long_attribute.h"
#include "bool_attribute.h"
#include "rect_attribute.h"
#include "size_attribute.h"
#include "flags_attribute.h"

/*
  2015-2-27 11:28 属性加载代码重构

  目标：现在的属性读取代码，属性的获取与一些逻辑代码交杂在一起，显示很乱。
        因此在这里要把属性的读取，与一些相关的逻辑代码尽量的分离，属性的
        读取也是透明的，只需要定义属性就行。另外也重新考虑对UIEditor的支持。

    Q1: render.type类属性如何实现

    Q2: 属性的加载顺序对于有依赖的属性如何实现，如只有先解释了render.type，
        解决 render.font/render.image 

    Q3: 子类中如何修改默认属性。例如object.transparent默认为false，但在label中，默认为true
        . 不支持？必须支持，例如 panel 要不支持transparent非常麻烦
        --> 专门增加一个变量，用于表示这个属性的默认值是什么。 SetDefaultTransparent、SetDefaultRejectMsgAll
*/
namespace ui
{

class AttributeBase;
class StringEnumAttribute;

// 创建一个属性类函数定义
typedef AttributeBase* (*pfnCreateAttributeClass)();

// 属性类工厂
class AttributeClassFactory
{
public:
    AttributeClassFactory();
    ~AttributeClassFactory();
    bool  Register(long, pfnCreateAttributeClass);
    AttributeBase*  CreateInstance(long);

private:
    typedef std::map<long, pfnCreateAttributeClass>  _ClassMap;
    typedef _ClassMap::iterator  _ClassIter;

    _ClassMap  m_map;
};

struct IMapAttribute;

// xml属性序列化
class AttributeSerializer
{
public:
    AttributeSerializer(SERIALIZEDATA*, const wchar_t* szGroupName);
    ~AttributeSerializer();
    AttributeBase*  Add(long eType, const wchar_t* szKey);
    AttributeBase*  Add(long eType, const wchar_t* szKey, void* pBindValue);
    // AttributeBase*  Add(long eType, const wchar_t* szKey, void* _this, void* _setter, void* _getter);

    void  DoAction();
    void  Load();
    void  Save();
    void  Editor();

    IUIApplication*  GetUIApplication();
    SERIALIZEDATA*  GetSerializeData();

public:
    // 封装

    StringAttribute*  AddString(const wchar_t*, String& sBindValue); // 仅内部使用
	// StringAttribute*  AddString(const wchar_t*, void* _this, pfnStringSetter s, pfnStringGetter g);
    StringAttribute*  AddString(const wchar_t*, slot<void(const wchar_t*)>&& s, slot<const wchar_t*()>&& g);
    StringAttribute*  AddString(const wchar_t* szKey, const std::function<void(const wchar_t*)>& s, const std::function<const wchar_t*()>& g);

    StringAttribute*  AddI18nString(const wchar_t*, String& sBindValue); // 仅内部使用
    StringAttribute*  AddI18nString(const wchar_t*, 
        slot<void(const wchar_t*, int)>&& s, 
        slot<const wchar_t*()>&& g);
    // StringAttribute*  AddI18nString(const wchar_t*, void* _this, pfnStringExSetter s, pfnStringGetter g);
    // StringAttribute*  AddI18nString(const wchar_t*, const std::function<void(const wchar_t*, int)>& s, const std::function<const wchar_t*()>& g);
    // StringEnumAttribute*  AddStringEnum(const wchar_t*, void* _this, pfnStringSetter s, pfnStringGetter g);

    BoolAttribute*  AddBool(const wchar_t*, bool& bBindValue);
    // BoolAttribute*  AddBool(const wchar_t*, void* _this, pfnBoolSetter s, pfnBoolGetter g);
    BoolAttribute*  AddBool(const wchar_t*, slot<void(bool)>&& s, slot<bool()>&& g);

    LongAttribute*  AddLong(const wchar_t*, long& lBindValue);
    // LongAttribute*  AddLong(const wchar_t*, void* _this, pfnLongSetter s, pfnLongGetter g);
    LongAttribute*  AddLong(const wchar_t*, slot<void(long)>&& s, slot<long()>&& g);

    FlagsAttribute*  AddFlags(const wchar_t*, long& lBindValue);

    EnumAttribute*  AddEnum(const wchar_t*, long& lBindValue);
    // EnumAttribute*  AddEnum(const wchar_t*, void* _this, pfnLongSetter s, pfnLongGetter g);
    EnumAttribute*  AddEnum(const wchar_t*, slot<void(long)>&& s, slot<long()>&& g);


	RectAttribute*  AddRect(const wchar_t*, RECT& rcBindValue);
	// RectAttribute*  AddRect(const wchar_t*, void* _this, pfnRectSetter s, pfnRectGetter g);
    RectAttribute*  AddRect(const wchar_t*, slot<void(RECT*)>&& s, slot<void(RECT*)>&& g);

	SizeAttribute*  AddSize(const wchar_t*, SIZE& sBindValue);
	// SizeAttribute*  AddSize(const wchar_t*, void* _this, pfnSizeSetter s, pfnSizeGetter g);
    SizeAttribute*  AddSize(const wchar_t*, slot<void(SIZE*)>&& s, slot<void(SIZE*)>&& g);

	Region9Attribute*  Add9Region(const wchar_t*, C9Region& rBindValue);
	// Region9Attribute*  Add9Region(const wchar_t*, void* _this, pfnRectSetter s, pfnRectGetter g);
    Region9Attribute*  Add9Region(const wchar_t*, slot<void(C9Region*)>&& s, slot<void(C9Region*)>&& g);


	ColorAttribute*  AddColor(const wchar_t*, Color*& pBindValue);
	ColorAttribute*  AddColor(const wchar_t*, Color& pBindValue);
    RenderBaseAttribute*  AddRenderBase(const wchar_t* szPrefix, Object* pObj, IRenderBase*& pBindValue);
    TextRenderBaseAttribute*  AddTextRenderBase(const wchar_t* szPrefix, Object* pObj, ITextRenderBase*& pBindValue);

private:
    // AttributeBase*  GetAttributeClassByKey(const wchar_t*);

private:
    typedef  std::list<AttributeBase*>  _AttrList;
    typedef  _AttrList::iterator   _AttrIter;

    _AttrList  m_list;
    SERIALIZEDATA*  m_pData;
	String  m_strGroupName;
};


// IUIEditor 与 各种Attribute 之间的桥梁
class AttributeEditorProxy
{
public:
    AttributeEditorProxy(IAttributeEditorProxy&, IUIEditor* p);
    ~AttributeEditorProxy();
    
    void  Clear();
    void  AddAttribute(
			AttributeBase* p, 
			SERIALIZEDATA* pData, 
			const wchar_t* szGroupName);

    AttributeBase*  FindAttributeByKey(const wchar_t* szKey);
    bool  Set(const wchar_t* szKey, const wchar_t* szValue);
    const wchar_t*  Get(const wchar_t* szKey);

    void  LoadAttribute2Editor(IObject* pObj);
	UpdateAttribute2EditorResult  UpdateAttribute2Editor(const wchar_t* szKey);


    void  String2Editor(StringAttribute* p, EditorAttributeFlag e);
    void  CharArray2Editor(CharArrayAttribute* p, EditorAttributeFlag e);
    void  Bool2Editor(BoolAttribute* p, EditorAttributeFlag e);
    void  Long2Editor(LongAttribute* p, EditorAttributeFlag e);
	void  Enum2Editor(EnumAttribute* p, EditorAttributeFlag e);
    void  Flags2Editor(FlagsAttribute* p, EditorAttributeFlag e);
    void  Rect2Editor(RectAttribute* p, EditorAttributeFlag e);
	void  Size2Editor(SizeAttribute* p, EditorAttributeFlag e);
	void  Region92Editor(Region9Attribute* p, EditorAttributeFlag e);
    void  StringEnum2Editor(StringEnumAttribute* p, EditorAttributeFlag e);
	void  Color2Editor(ColorAttribute* p, EditorAttributeFlag e);
    void  RenderBase2Editor(RenderBaseAttribute* p, EditorAttributeFlag e);
    void  TextRenderBase2Editor(
                TextRenderBaseAttribute* p, EditorAttributeFlag e);

private:
    IAttributeEditorProxy&  m_oIProxy;
    IUIEditor*  m_pEditor;

    typedef  std::list<AttributeBase*>  _AttrList;
    typedef  _AttrList::iterator   _AttrIter;

    _AttrList  m_list;
    bool  m_bLoaded;  // 标识是否已加载过
};

}