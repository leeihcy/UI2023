#pragma once
#include "bool_attribute.h"
#include "enum_attribute.h"
#include "flags_attribute.h"
#include "include/interface/iattribute.h"
#include "include/util/rect.h"
#include "int_attribute.h"
#include "length_attribute.h"
#include "rect_attribute.h"
#include "radius_attribute.h"
#include "size_attribute.h"
#include "string_attribute.h"

/*
  2015-2-27 11:28 属性加载代码重构

  目标：现在的属性读取代码，属性的获取与一些逻辑代码交杂在一起，显示很乱。
        因此在这里要把属性的读取，与一些相关的逻辑代码尽量的分离，属性的
        读取也是透明的，只需要定义属性就行。另外也重新考虑对UIEditor的支持。

    Q1: render.type类属性如何实现

    Q2: 属性的加载顺序对于有依赖的属性如何实现，如只有先解释了render.type，
        解决 render.font/render.image

    Q3:
  子类中如何修改默认属性。例如object.transparent默认为false，但在label中，默认为true
        . 不支持？必须支持，例如 panel 要不支持transparent非常麻烦
        --> 专门增加一个变量，用于表示这个属性的默认值是什么。
  SetDefaultTransparent、SetDefaultRejectMsgAll
*/
namespace ui {

class AttributeBase;
class StringEnumAttribute;

// 创建一个属性类函数定义
typedef AttributeBase *(*pfnCreateAttributeClass)();

// 属性类工厂
class AttributeClassFactory {
public:
  AttributeClassFactory();
  ~AttributeClassFactory();
  bool Register(int, pfnCreateAttributeClass);
  AttributeBase *CreateInstance(int);

private:
  typedef std::map<int, pfnCreateAttributeClass> _ClassMap;
  typedef _ClassMap::iterator _ClassIter;

  _ClassMap m_map;
};

struct IMapAttribute;

// xml属性序列化
class AttributeSerializer {
public:
  AttributeSerializer(SerializeParam *, const char *szGroupName);
  ~AttributeSerializer();
  AttributeBase *Add(int eType, const char *szKey);
  AttributeBase *Add(int eType, const char *szKey, void *pBindValue);
  // AttributeBase*  Add(int eType, const char* szKey, void* _this, void*
  // _setter, void* _getter);

  void DoAction();
  void Load();
  void Save();
  void Editor();

  IApplication *GetUIApplication();
  SerializeParam *GetSerializeData();

public:
  // 封装

  // 仅内部使用
  StringAttribute *AddString(const char *, std::string &sBindValue);
  StringAttribute *AddString(const char *, slot<void(const char *)> &&s,
                             slot<const char *()> &&g);
  StringAttribute *AddString(const char *szKey,
                             const std::function<void(const char *)> &s,
                             const std::function<const char *()> &g);

  // 仅内部使用
  StringAttribute *AddI18nString(const char *, std::string &sBindValue);
  StringAttribute *AddI18nString(const char *,
                                 slot<void(const char *, int)> &&s,
                                 slot<const char *()> &&g);
  StringEnumAttribute *AddStringEnum(const char *, slot<void(const char *)> &&s,
                                     slot<const char *()> &&g);

  BoolAttribute *AddBool(const char *, bool &bBindValue);
  BoolAttribute *AddBool(const char *, slot<void(bool)> &&s, slot<bool()> &&g);

  IntAttribute *AddInt(const char *, int &lBindValue);
  IntAttribute *AddInt(const char *, slot<void(int)> &&s, slot<int()> &&g);

  LengthAttribute *AddLength(const char *, Length &bind_value);
  LengthAttribute *AddLength(const char *, slot<void(Length)> &&s,
                             slot<Length()> &&g);

  FlagsAttribute *AddFlags(const char *, int &lBindValue);

  EnumAttribute *AddEnum(const char *, int &lBindValue);
  EnumAttribute *AddEnum(const char *, slot<void(int)> &&s, slot<int()> &&g);

  RectAttribute *AddRect(const char *, Rect &rcBindValue);
  RectAttribute *AddRect(const char *, slot<void(Rect *)> &&s,
                         slot<void(Rect *)> &&g);

  RadiusAttribute *AddRadius(const char *, CornerRadius &rcBindValue);
  RadiusAttribute *AddRadius(const char *, slot<void(CornerRadius *)> &&s,
                         slot<void(CornerRadius *)> &&g);

  SizeAttribute *AddSize(const char *, Size &sBindValue);
  SizeAttribute *AddSize(const char *, slot<void(Size *)> &&s,
                         slot<void(Size *)> &&g);

  Region9Attribute *Add9Region(const char *, C9Region &rBindValue);
  Region9Attribute *Add9Region(const char *, slot<void(C9Region *)> &&s,
                               slot<void(C9Region *)> &&g);

  ColorAttribute *AddColor(const char *, Color &pBindValue);
  ColorAttribute *AddColor(const char *, slot<void(Color)>&& setter, slot<Color()>&& getter);
  
  RenderBaseAttribute *AddRenderBase(const char *szPrefix, Object *pObj,
                                     IRenderBase *&pBindValue);
  TextRenderBaseAttribute *AddTextRenderBase(const char *szPrefix, Object *pObj,
                                             ITextRenderBase *&pBindValue);

private:
  // AttributeBase*  GetAttributeClassByKey(const char*);

private:
  typedef std::list<AttributeBase *> _AttrList;
  typedef _AttrList::iterator _AttrIter;

  _AttrList m_list;
  SerializeParam *m_pData;
  std::string m_strGroupName;
};

// IUIEditor 与 各种Attribute 之间的桥梁
class AttributeEditorProxy {
public:
  AttributeEditorProxy(IAttributeEditorProxy &, IUIEditor *p);
  ~AttributeEditorProxy();

  void Clear();
  void AddAttribute(AttributeBase *p, SerializeParam *pData,
                    const char *szGroupName);

  AttributeBase *FindAttributeByKey(const char *szKey);
  bool Set(const char *szKey, const char *szValue);
  const char *Get(const char *szKey);

  void LoadAttribute2Editor(IObject *pObj);
  UpdateAttribute2EditorResult UpdateAttribute2Editor(const char *szKey);

  void String2Editor(StringAttribute *p, EditorAttributeFlag e);
  void CharArray2Editor(CharArrayAttribute *p, EditorAttributeFlag e);
  void Bool2Editor(BoolAttribute *p, EditorAttributeFlag e);
  void Long2Editor(IntAttribute *p, EditorAttributeFlag e);
  void Enum2Editor(EnumAttribute *p, EditorAttributeFlag e);
  void Flags2Editor(FlagsAttribute *p, EditorAttributeFlag e);
  void Rect2Editor(RectAttribute *p, EditorAttributeFlag e);
  void CornerRadius2Editor(RadiusAttribute* p, EditorAttributeFlag e);
  void Size2Editor(SizeAttribute *p, EditorAttributeFlag e);
  void Region92Editor(Region9Attribute *p, EditorAttributeFlag e);
  void StringEnum2Editor(StringEnumAttribute *p, EditorAttributeFlag e);
  void Color2Editor(ColorAttribute *p, EditorAttributeFlag e);
  void RenderBase2Editor(RenderBaseAttribute *p, EditorAttributeFlag e);
  void TextRenderBase2Editor(TextRenderBaseAttribute *p, EditorAttributeFlag e);

private:
  IAttributeEditorProxy &m_oIProxy;
  IUIEditor *m_pEditor;

  typedef std::list<AttributeBase *> _AttrList;
  typedef _AttrList::iterator _AttrIter;

  _AttrList m_list;
  bool m_bLoaded; // 标识是否已加载过
};

} // namespace ui