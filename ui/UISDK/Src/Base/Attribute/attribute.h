#pragma once
#include "Inc\Interface\iattribute.h"
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
namespace UI
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
    typedef map<long, pfnCreateAttributeClass>  _ClassMap;
    typedef _ClassMap::iterator  _ClassIter;

    _ClassMap  m_map;
};

interface IMapAttribute;

// xml属性序列化
class AttributeSerializer
{
public:
    AttributeSerializer(SERIALIZEDATA*, LPCTSTR szGroupName);
    ~AttributeSerializer();
    AttributeBase*  Add(long eType, LPCTSTR szKey);
    AttributeBase*  Add(long eType, LPCTSTR szKey, void* pBindValue);
    AttributeBase*  Add(long eType, LPCTSTR szKey, void* _this, void* _setter, void* _getter);

    void  DoAction();
    void  Load();
    void  Save();
    void  Editor();

    IUIApplication*  GetUIApplication();
    SERIALIZEDATA*  GetSerializeData();

public:
    // 封装

    StringAttribute*  AddString(LPCTSTR, String& sBindValue); // 仅内部使用
	StringAttribute*  AddString(LPCTSTR, void* _this, pfnStringSetter s, pfnStringGetter g);
    StringAttribute*  AddString(LPCTSTR szKey, const function<void(LPCTSTR)>& s, const function<LPCTSTR()>& g);

    StringAttribute*  AddI18nString(LPCTSTR, String& sBindValue); // 仅内部使用
    //StringAttribute*  AddI18nString(LPCTSTR, void* _this, pfnStringExSetter s, pfnStringGetter g);
    StringAttribute*  AddI18nString(LPCTSTR, const function<void(LPCTSTR, int)>& s, const function<LPCTSTR()>& g);
    StringEnumAttribute*  AddStringEnum(LPCTSTR, void* _this, pfnStringSetter s, pfnStringGetter g);

    BoolAttribute*  AddBool(LPCTSTR, bool& bBindValue);
    BoolAttribute*  AddBool(LPCTSTR, void* _this, pfnBoolSetter s, pfnBoolGetter g);

    LongAttribute*  AddLong(LPCTSTR, long& lBindValue);
    LongAttribute*  AddLong(LPCTSTR, void* _this, pfnLongSetter s, pfnLongGetter g);

    FlagsAttribute*  AddFlags(LPCTSTR, long& lBindValue);

    EnumAttribute*  AddEnum(LPCTSTR, long& lBindValue);
    EnumAttribute*  AddEnum(LPCTSTR, void* _this, pfnLongSetter s, pfnLongGetter g);

	RectAttribute*  AddRect(LPCTSTR, RECT& rcBindValue);
	RectAttribute*  AddRect(LPCTSTR, void* _this, pfnRectSetter s, pfnRectGetter g);

	SizeAttribute*  AddSize(LPCTSTR, SIZE& sBindValue);
	SizeAttribute*  AddSize(LPCTSTR, void* _this, pfnSizeSetter s, pfnSizeGetter g);

	Region9Attribute*  Add9Region(LPCTSTR, C9Region& rBindValue);
	Region9Attribute*  Add9Region(LPCTSTR, void* _this, pfnRectSetter s, pfnRectGetter g);

	ColorAttribute*  AddColor(LPCTSTR, Color*& pBindValue);
	ColorAttribute*  AddColor(LPCTSTR, Color& pBindValue);
    RenderBaseAttribute*  AddRenderBase(LPCTSTR szPrefix, Object* pObj, IRenderBase*& pBindValue);
    TextRenderBaseAttribute*  AddTextRenderBase(LPCTSTR szPrefix, Object* pObj, ITextRenderBase*& pBindValue);

private:
    // AttributeBase*  GetAttributeClassByKey(LPCTSTR);

private:
    typedef  list<AttributeBase*>  _AttrList;
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
			LPCTSTR szGroupName);

    AttributeBase*  FindAttributeByKey(LPCTSTR szKey);
    bool  Set(LPCTSTR szKey, LPCTSTR szValue);
    LPCTSTR  Get(LPCTSTR szKey);

    void  LoadAttribute2Editor(IObject* pObj);
	UpdateAttribute2EditorResult  UpdateAttribute2Editor(LPCTSTR szKey);


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

    typedef  list<AttributeBase*>  _AttrList;
    typedef  _AttrList::iterator   _AttrIter;

    _AttrList  m_list;
    bool  m_bLoaded;  // 标识是否已加载过
};

}