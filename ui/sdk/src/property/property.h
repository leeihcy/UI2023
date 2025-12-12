#ifndef _UI_SDK_SRC_PROPERTY_H_
#define _UI_SDK_SRC_PROPERTY_H_

#include <string>

namespace ui {

// 这个属性的Value指针需要调用free进行释放
#define PF_MallocValue 1

// 这个属性能够继承父对象的值
#define PF_Inherit 2

// 属性值基类
struct PropertyValue {
};

// PropertyValue 创建释放，尽量不使用stdptr，减小对象内存使用。
template<class T, class... Types>
PropertyValue* mallocValue(Types... args) {
  T* p = (T*)::malloc(sizeof(T));
  new (p) T(args...);
  return p;
}

void freeValue(PropertyValue*& v, int flags);

// 属性注册信息
struct Property {
  ~Property();

  int id = 0;
  int flags = 0;
  std::string key;

  // value可能是static，也可能是malloc，根据flags标志位进行释放
  PropertyValue *value = nullptr;
};


struct IntValue : public PropertyValue {
  IntValue(int v) : value(v) {}
  static PropertyValue* s_0() { static IntValue v(0); return &v;}
  static PropertyValue* s_1() { static IntValue v(1); return &v;}
  static PropertyValue* s_minus1() { static IntValue v(-1); return &v;}

  int value = 0;
};

struct BoolValue : public PropertyValue  {
  BoolValue(bool v) : value(v) {}
  static PropertyValue* s_true() { static BoolValue v(true); return &v;}
  static PropertyValue* s_false() { static BoolValue v(false); return &v;}
  
  bool value = false;
};

struct StringValue : public PropertyValue  {
  StringValue(const char* v) {
    if (v) { value = v; }
  }
  static PropertyValue* s_empty() { static StringValue v(nullptr); return &v;}
  void Set(const char* text) {
    if (text) { value = text; }
    else { value.clear(); }
  }
  std::string value;
};


}


#endif