#include "src/property/property.h"

#include "include/macro/xmldefine.h"

namespace ui {

enum class PropertyFlags : int{
  // 这个属性的Value指针需要调用free进行释放
  ToFree = 1,

  // 这个属性能够继承父对象的值
  Inheritable = 2,

  // 这个属性是数据，不是样式
  AsData = 4
};


Property::~Property() { 
  if (NeedsFree()) {
    ::free(value);
  }
  value = nullptr;
}

IProperty& Property::AsData() {
  flags |= (int)PropertyFlags::AsData;
  return *this;
}
IProperty& Property::Inheritable() {
  flags |= (int)PropertyFlags::Inheritable;
  return *this;
}
IProperty& Property::ToFree() {
  flags |= (int)PropertyFlags::ToFree;
  return *this;
}

bool Property::IsData() { return flags & (int)PropertyFlags::AsData; }
bool Property::CanInherit() { return flags & (int)PropertyFlags::Inheritable; }
bool Property::NeedsFree() { return flags & (int)PropertyFlags::ToFree; }



PropertyValue* PropertyValue::Parse(PropertyValueType type, const char* text) {
  switch (type) {
  case PropertyValueType::Int:
    return IntValue::Parse(text);
  case PropertyValueType::Bool:
    return BoolValue::Parse(text);
  case PropertyValueType::String:
    return StringValue::Parse(text);
  default:
    assert(false && "TODO: Not implement");
    return nullptr;
  }
}

IntValue* IntValue::Parse(const char* text) {
  return mallocValue<IntValue>(atoi(text));
}

BoolValue* BoolValue::Parse(const char* text) {
  bool b = false;
  if (0 == strcmp(text, XML_BOOL_VALUE_TRUE) ||
      0 == strcmp(text, XML_BOOL_VALUE_1) ||
      0 == strcmp(text, XML_BOOL_VALUE_YES)) {
    b = true;
  }
  return mallocValue<BoolValue>(b);
}

StringValue* StringValue::Parse(const char* text) {
  return mallocValue<StringValue>(text);
}

} // namespace ui
