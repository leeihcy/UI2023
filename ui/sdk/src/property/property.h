#ifndef _UI_SDK_SRC_PROPERTY_H_
#define _UI_SDK_SRC_PROPERTY_H_

#include <string>
#include "include/interface/iproperty.h"
#include "include/util/rect.h"

namespace ui {

enum class PropertyValueFlags : int {
  // 不需要释放
  Static = 1,
  // 从Pool创建，不需要释放
  Pool = 2,
  // 需要释放
  Alloc = 4,
};

enum class PropertyFlags : int{
  // 这个属性能够继承父对象的值
  Inheritable = 1,

  // 这个属性是数据，不是样式
  AsData = 2
};

enum class PropertyValueType : int {
  Invalid = 0,
  Int = 1 << 0,
  Bool = 1 << 1,
  String = 1 << 2,
  Rect = 1 << 3,
};


// PropertyValue 创建释放，尽量不使用stdptr，减小对象内存使用。
template<class T, class... Types>
T* mallocValue(Types... args) {
  T* p = (T*)::malloc(sizeof(T));
  new (p) T(args... );
  p->flags |= (int)PropertyValueFlags::Alloc;
  return p;
}

// 属性值基类
struct PropertyValue {
  virtual ~PropertyValue() {}
  static PropertyValue* Parse(PropertyValueType, const char* text);

  PropertyValue(PropertyValueType _type, unsigned int _flags = 0) {
    type = _type;
    flags = _flags;
  }
  bool IsAlloc() { return flags & (int)PropertyValueFlags::Alloc; }

  PropertyValueType type : 16;
  unsigned int flags : 16;
};


// 每个属性的注册信息
class Property : public IProperty {
public:
  ~Property();

  PropertyValueType Type() {
    if (value) {
      return value->type;
    }
    return PropertyValueType::Invalid;
  }

  // flags相关操作
  IProperty& AsData() override;
  IProperty& Inheritable() override;

  bool IsData() override;
  bool CanInherit() override;

public:
  // virtual table pointer

  // value可能是static，也可能是malloc，根据PropertyValue::flags标志位进行释放
  PropertyValue *value = nullptr;
  unsigned int flags = 0;
};


struct IntValue : public PropertyValue {
  IntValue(int v, unsigned int _flags = 0) : PropertyValue(PropertyValueType::Int, _flags), value(v) {}

  static IntValue* s_0() { static IntValue v(0, (int)PropertyValueFlags::Static); return &v;}
  static IntValue* s_1() { static IntValue v(1, (int)PropertyValueFlags::Static); return &v;}
  static IntValue* s_minus1() { static IntValue v(-1, (int)PropertyValueFlags::Static); return &v;}
  
  static IntValue* Create(int value);
  static IntValue* Parse(const char* text);

  int value = 0;
};

struct BoolValue : public PropertyValue  {
  BoolValue(bool v, unsigned int _flags = 0) : PropertyValue(PropertyValueType::Bool, _flags), value(v) {}

  static BoolValue* s_true() { static BoolValue v(true, (int)PropertyValueFlags::Static); return &v;}
  static BoolValue* s_false() { static BoolValue v(false, (int)PropertyValueFlags::Static); return &v;}

  static BoolValue* Create(bool b);
  static BoolValue* Parse(const char* text);

  bool value = false;
};

struct StringValue : public PropertyValue  {
  StringValue(const char* v, unsigned int _flags = 0) : PropertyValue(PropertyValueType::String, _flags) { if (v) { value = v; } }

  static StringValue* s_empty() { static StringValue v(nullptr, (int)PropertyValueFlags::Static); return &v;}
  
  static StringValue* Create(const char* text);
  static StringValue* Parse(const char* text);

  void Set(const char* text) {
    if (text) { value = text; }
    else { value.clear(); }
  }
  std::string value;
};

struct RectValue : public PropertyValue {
  RectValue(const Rect& rect, unsigned int _flags = 0) : PropertyValue(PropertyValueType::Rect, _flags), value(rect) {}
  static PropertyValueType Type() { return PropertyValueType::Rect; }
  
  static RectValue* s_empty() { 
    static RectValue v(ui::Rect::MakeLTRB(0, 0, 0, 0), (int)PropertyValueFlags::Static); 
    return &v;
  }

  static RectValue* Create(const Rect& rect);
  static RectValue* Parse(const char* text);

  Rect value;
};

}


#endif