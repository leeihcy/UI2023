#include "html/css/property/css_value.h"
#include "html/base/memory.h"
#include "html/util/util.h"
#include <cassert>
#include <map>
#include <cmath>  // std::signbit

namespace html {

class CSSValuePool {
public:
  static CSSValuePool& Get() {
    static CSSValuePool s;
    return s;
  }
  CSSValuePool() {
  }

  A<CSSNumericLiteralValue> PixelCacheValue(int value) {
    auto iter = m_pixel_numeric_value_map.find(value);
    if (iter != m_pixel_numeric_value_map.end()) {
      return A<CSSNumericLiteralValue>::from_pool(iter->second);
    }
    CSSNumericLiteralValue *v =
        new CSSNumericLiteralValue(value, CSSPrimitiveValue::UnitType::kPixels);
    m_pixel_numeric_value_map[value] = v;
    return A<CSSNumericLiteralValue>::from_pool(v);
  }
  A<CSSNumericLiteralValue> PercentCacheValue(int value) {
    auto iter = m_percent_numeric_value_map.find(value);
    if (iter != m_percent_numeric_value_map.end()) {
      return A<CSSNumericLiteralValue>::from_pool(iter->second);
    }
    CSSNumericLiteralValue *v = new CSSNumericLiteralValue(
        value, CSSPrimitiveValue::UnitType::kPercentage);
    m_percent_numeric_value_map[value] = v;
    return A<CSSNumericLiteralValue>::from_pool(v);
  }
  A<CSSNumericLiteralValue> NumberCacheValue(int value) {
    auto iter = m_number_value_map.find(value);
    if (iter != m_number_value_map.end()) {
      return A<CSSNumericLiteralValue>::from_pool(iter->second);
    }
    CSSNumericLiteralValue *v = new CSSNumericLiteralValue(
        value, CSSPrimitiveValue::UnitType::kInteger);
    m_number_value_map[value] = v;
    return A<CSSNumericLiteralValue>::from_pool(v);
  }

  A<CSSIdentifierValue> IdentifierCacheValue(CSSValueId value_id) {
    auto iter = m_identifier_value_map.find(value_id);
    if (iter != m_identifier_value_map.end()) {
      return A<CSSIdentifierValue>::from_pool(iter->second);
    }
    CSSIdentifierValue* v = new CSSIdentifierValue(value_id);
    m_identifier_value_map[value_id] = v;
    return A<CSSIdentifierValue>::from_pool(v);
  }
  A<CSSColorValue> GetOrCreateColor(const Color& color) {
    // This is the empty value of the hash table.
    // See ColorHashTraitsForCSSValuePool.
    if (color == Color::kTransparent) {
      return A<CSSColorValue>::from_managed(TransparentColor());
    }

    // Just because they are common.
    if (color == Color::kWhite) {
      return A<CSSColorValue>::from_managed(WhiteColor());
    }
    if (color == Color::kBlack) {
      return A<CSSColorValue>::from_managed(BlackColor());
    }

    auto iter = m_color_value_cache.find(color);
    if (iter != m_color_value_cache.end()) {
      return A<CSSColorValue>::from_pool(iter->second);
    }

    CSSColorValue* v = new CSSColorValue(color);
    m_color_value_cache[color] = v;
    return A<CSSColorValue>::from_pool(v);
  }
  CSSColorValue* TransparentColor() { return &m_color_transparent; }
  CSSColorValue* WhiteColor() { return &m_color_white; }
  CSSColorValue* BlackColor() { return &m_color_black; }
 
  static const int kMaximumCacheableIntegerValue = 255;

private:
  std::map<CSSValueId, CSSIdentifierValue*> m_identifier_value_map;

  std::map<int, CSSNumericLiteralValue*> m_pixel_numeric_value_map;
  std::map<int, CSSNumericLiteralValue*> m_percent_numeric_value_map;
  std::map<int, CSSNumericLiteralValue*> m_number_value_map;

  CSSColorValue m_color_transparent = {Color::kTransparent};
  CSSColorValue m_color_white = {Color::kWhite};
  CSSColorValue m_color_black = {Color::kBlack};
  std::map<Color, CSSColorValue*> m_color_value_cache;
};


A<CSSInitialValue> CSSInitialValue::Create() {
  static CSSInitialValue s;
  return A<CSSInitialValue>::from_managed(&s);
}
A<CSSInheritedValue> CSSInheritedValue::Create() {
  static CSSInheritedValue s;
  return A<CSSInheritedValue>::from_managed(&s);
}
A<CSSUnsetValue> CSSUnsetValue::Create() {
  static CSSUnsetValue s;
  return A<CSSUnsetValue>::from_managed(&s);
}
A<CSSRevertValue> CSSRevertValue::Create() {
  static CSSRevertValue s;
  return A<CSSRevertValue>::from_managed(&s);
}
A<CSSRevertLayerValue> CSSRevertLayerValue::Create() {
  static CSSRevertLayerValue s;
  return A<CSSRevertLayerValue>::from_managed(&s);
}

A<CSSIdentifierValue> CSSIdentifierValue::Create(CSSValueId value_id) {
  return CSSValuePool::Get().IdentifierCacheValue(value_id);
}
CSSIdentifierValue::CSSIdentifierValue(CSSValueId id) 
  : m_value_id(id), CSSValue(CSSValueClassType::Identifier)  {

}

A<CSSColorValue> CSSColorValue::Create(const Color& color) {
  return CSSValuePool::Get().GetOrCreateColor(color);
}

CSSPrimitiveValue::UnitType CSSPrimitiveValue::StringToUnitType(const std::u16string& text) {
  unsigned length = static_cast<unsigned>(text.size());
  switch (length) {
  case 1:
    break;
  case 2:
    switch (ToASCIILower(text[0])) {
      case 'p':
        switch (ToASCIILower(text[1])) {
          case 'x':
            return CSSPrimitiveValue::UnitType::kPixels;
        }
      break;
    }
  case 3:
    break;
  case 4:
    break;
  default:
    break;
  }
  return CSSPrimitiveValue::UnitType::kUnknown;
}

// static 
A<CSSNumericLiteralValue> CSSNumericLiteralValue::Create(double value, UnitType unit_type) {
  if (value < 0 || value > CSSValuePool::kMaximumCacheableIntegerValue) {
    return A<CSSNumericLiteralValue>::make_new(value, unit_type);
  }

  // 小数字特殊处理

  int int_value = static_cast<int>(value);
  if (value != int_value || (value == 0 && std::signbit(value))) {
    return A<CSSNumericLiteralValue>::make_new(value, unit_type);
  }

  CSSNumericLiteralValue* result = nullptr;
  switch (unit_type) {
    case CSSPrimitiveValue::UnitType::kPixels:
      return CSSValuePool::Get().PixelCacheValue(value);
    case CSSPrimitiveValue::UnitType::kPercentage:
      return CSSValuePool::Get().PercentCacheValue(value);
    case CSSPrimitiveValue::UnitType::kNumber:
    case CSSPrimitiveValue::UnitType::kInteger:
      return CSSValuePool::Get().NumberCacheValue(value);
    default:
      return A<CSSNumericLiteralValue>::make_new(value, unit_type);
  }
}

CSSNumericLiteralValue::CSSNumericLiteralValue(double num, UnitType unit_type)
    : m_num(num),
      CSSPrimitiveValue(unit_type, CSSValueClassType::NumericLiteral) {}


CSSRepeatStyleValue::CSSRepeatStyleValue(A<CSSIdentifierValue>&& x, A<CSSIdentifierValue>&& y) : 
  m_x(std::move(x)), m_y(std::move(y)),
  CSSValue(CSSValueClassType::RepeatStyle) {

}

CSSRepeatStyleValue::CSSRepeatStyleValue(A<CSSIdentifierValue>&& id): CSSValue(CSSValueClassType::RepeatStyle) {
    switch (id->GetValueId()) {
    case CSSValueId::RepeatX:
      m_x.reset(CSSIdentifierValue::Create(CSSValueId::Repeat));
      m_y.reset(CSSIdentifierValue::Create(CSSValueId::NoRepeat));
      break;

    case CSSValueId::RepeatY:
      m_x.reset(CSSIdentifierValue::Create(CSSValueId::NoRepeat));
      m_y.reset(CSSIdentifierValue::Create(CSSValueId::Repeat));
      break;

    default:
      m_x.reset(std::move(id));
      m_y.share(m_x);
      break;
  }
}

} // namespace html