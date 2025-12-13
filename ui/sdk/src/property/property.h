#ifndef _UI_SDK_SRC_PROPERTY_H_
#define _UI_SDK_SRC_PROPERTY_H_

#include <string>
#include "include/interface/iproperty.h"

namespace ui {

// PropertyValue 创建释放，尽量不使用stdptr，减小对象内存使用。
template<class T, class... Types>
T* mallocValue(Types... args) {
  T* p = (T*)::malloc(sizeof(T));
  new (p) T(args...);
  return p;
}

enum class PropertyValueType : int {
  Int = 1 << 0,
  Bool = 1 << 1,
  String = 1 << 2
};

// 属性值基类
struct PropertyValue {
  // 注：不要添加虚函数，会导致每个实例也多一个虚表指针。
  static PropertyValue* Parse(PropertyValueType, const char* text);
};


// 每个属性的注册信息
class Property : public IProperty {
public:
  ~Property();

  // flags相关操作
  IProperty& AsData() override;
  IProperty& Inheritable() override;
  IProperty& ToFree() override;

  bool IsData() override;
  bool CanInherit() override;
  bool NeedsFree() override;

public:
  // virtual table pointer

  // value可能是static，也可能是malloc，根据flags标志位进行释放
  PropertyValue *value = nullptr;

  PropertyValueType type;
  unsigned int flags = 0;
};


struct IntValue : public PropertyValue {
  IntValue(int v) : value(v) {}
  static PropertyValueType Type() { return PropertyValueType::Int; }

  static IntValue* s_0() { static IntValue v(0); return &v;}
  static IntValue* s_1() { static IntValue v(1); return &v;}
  static IntValue* s_minus1() { static IntValue v(-1); return &v;}
  
  static IntValue* Parse(const char* text);

  int value = 0;
};

struct BoolValue : public PropertyValue  {
  BoolValue(bool v) : value(v) {}
  static PropertyValueType Type() { return PropertyValueType::Bool; }

  static BoolValue* s_true() { static BoolValue v(true); return &v;}
  static BoolValue* s_false() { static BoolValue v(false); return &v;}

  static BoolValue* Parse(const char* text);

  bool value = false;
};

struct StringValue : public PropertyValue  {
  StringValue(const char* v) { if (v) { value = v; } }
  static PropertyValueType Type() { return PropertyValueType::String; }

  static StringValue* s_empty() { static StringValue v(nullptr); return &v;}
  
  static StringValue* Parse(const char* text);

  void Set(const char* text) {
    if (text) { value = text; }
    else { value.clear(); }
  }
  std::string value;
};


}


#endif