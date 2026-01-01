#include "src/property/property.h"

#include "include/macro/xmldefine.h"
#include "src/util/util.h"

namespace ui {

Property::~Property() { 
  if (value) {
    if (value->flags & (int)PropertyValueFlags::Alloc) {
      delete value;
    }
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

bool Property::IsData() { return flags & (int)PropertyFlags::AsData; }
bool Property::CanInherit() { return flags & (int)PropertyFlags::Inheritable; }

PropertyValue* PropertyValue::Parse(PropertyValueType type, const char* text) {
  switch (type) {
  case PropertyValueType::Int:
    return IntValue::Parse(text);
  case PropertyValueType::Bool:
    return BoolValue::Parse(text);
  case PropertyValueType::String:
    return StringValue::Parse(text);
  case PropertyValueType::Rect:
    return RectValue::Parse(text);
  default:
    assert(false && "TODO: Not implement");
    return nullptr;
  }
}

IntValue* IntValue::Create(int value) {
  if (value == 0) {
    return s_0();
  } else if (value == -1) {
    return s_minus1();
  } else if (value == 1) {
    return s_1();
  }
  return mallocValue<IntValue>(value);
}
IntValue* IntValue::Parse(const char* text) {
  return Create(atoi(text));
}

BoolValue* BoolValue::Create(bool value) {
  if (value) {
    return s_true();
  } else {
    return s_false();
  }
}
BoolValue* BoolValue::Parse(const char* text) {
  bool b = false;
  if (0 == strcmp(text, XML_BOOL_VALUE_TRUE) ||
      0 == strcmp(text, XML_BOOL_VALUE_1) ||
      0 == strcmp(text, XML_BOOL_VALUE_YES)) {
    b = true;
  }
  return Create(b);
}

StringValue *StringValue::Create(const char *text) {
  if (text) {
    return mallocValue<StringValue>(text);
  } else {
    return s_empty();
  }
}
StringValue *StringValue::Parse(const char *text) { return Create(text); }

RectValue* RectValue::Create(const Rect& rect) {
  if (rect.IsEmpty()) {
    return RectValue::s_empty();
  }
  return mallocValue<RectValue>(rect);
}
RectValue* RectValue::Parse(const char* text) {
  Rect rc = {0};
  util::TranslateRECT(text, &rc, XML_SEPARATOR);
  return Create(rc);
}

} // namespace ui
