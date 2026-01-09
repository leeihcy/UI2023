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

  U<CSSNumericLiteralValue> PixelCacheValue(int value) {
    auto iter = m_pixel_numeric_value_map.find(value);
    if (iter != m_pixel_numeric_value_map.end()) {
      return U<CSSNumericLiteralValue>::from_pool(iter->second);
    }
    CSSNumericLiteralValue *v =
        new CSSNumericLiteralValue(value, CSSPrimitiveValue::UnitType::kPixels);
    m_pixel_numeric_value_map[value] = v;
    return U<CSSNumericLiteralValue>::from_pool(v);
  }
  U<CSSNumericLiteralValue> PercentCacheValue(int value) {
    auto iter = m_percent_numeric_value_map.find(value);
    if (iter != m_percent_numeric_value_map.end()) {
      return U<CSSNumericLiteralValue>::from_pool(iter->second);
    }
    CSSNumericLiteralValue *v = new CSSNumericLiteralValue(
        value, CSSPrimitiveValue::UnitType::kPercentage);
    m_percent_numeric_value_map[value] = v;
    return U<CSSNumericLiteralValue>::from_pool(v);
  }
  U<CSSNumericLiteralValue> NumberCacheValue(int value) {
    auto iter = m_number_value_map.find(value);
    if (iter != m_number_value_map.end()) {
      return U<CSSNumericLiteralValue>::from_pool(iter->second);
    }
    CSSNumericLiteralValue *v = new CSSNumericLiteralValue(
        value, CSSPrimitiveValue::UnitType::kInteger);
    m_number_value_map[value] = v;
    return U<CSSNumericLiteralValue>::from_pool(v);
  }

  U<CSSIdentifierValue> IdentifierCacheValue(CSSValueId value_id) {
    auto iter = m_identifier_value_map.find(value_id);
    if (iter != m_identifier_value_map.end()) {
      return U<CSSIdentifierValue>::from_pool(iter->second);
    }
    CSSIdentifierValue* v = new CSSIdentifierValue(value_id);
    m_identifier_value_map[value_id] = v;
    return U<CSSIdentifierValue>::from_pool(v);
  }
  U<CSSColorValue> GetOrCreateColor(const Color& color) {
    // This is the empty value of the hash table.
    // See ColorHashTraitsForCSSValuePool.
    if (color == Color::kTransparent) {
      return U<CSSColorValue>::from_managed(TransparentColor());
    }

    // Just because they are common.
    if (color == Color::kWhite) {
      return U<CSSColorValue>::from_managed(WhiteColor());
    }
    if (color == Color::kBlack) {
      return U<CSSColorValue>::from_managed(BlackColor());
    }

    auto iter = m_color_value_cache.find(color);
    if (iter != m_color_value_cache.end()) {
      return U<CSSColorValue>::from_pool(iter->second);
    }

    CSSColorValue* v = new CSSColorValue(color);
    m_color_value_cache[color] = v;
    return U<CSSColorValue>::from_pool(v);
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


U<CSSInitialValue> CSSInitialValue::Create() {
  static CSSInitialValue s;
  return U<CSSInitialValue>::from_managed(&s);
}
U<CSSInheritedValue> CSSInheritedValue::Create() {
  static CSSInheritedValue s;
  return U<CSSInheritedValue>::from_managed(&s);
}
U<CSSUnsetValue> CSSUnsetValue::Create() {
  static CSSUnsetValue s;
  return U<CSSUnsetValue>::from_managed(&s);
}
U<CSSRevertValue> CSSRevertValue::Create() {
  static CSSRevertValue s;
  return U<CSSRevertValue>::from_managed(&s);
}
U<CSSRevertLayerValue> CSSRevertLayerValue::Create() {
  static CSSRevertLayerValue s;
  return U<CSSRevertLayerValue>::from_managed(&s);
}

U<CSSIdentifierValue> CSSIdentifierValue::Create(CSSValueId value_id) {
  return CSSValuePool::Get().IdentifierCacheValue(value_id);
}
CSSIdentifierValue::CSSIdentifierValue(CSSValueId id) 
  : m_value_id(id), CSSValue(CSSValueClassType::Identifier)  {

}

U<CSSColorValue> CSSColorValue::Create(const Color& color) {
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
U<CSSNumericLiteralValue> CSSNumericLiteralValue::Create(double value, UnitType unit_type) {
  if (value < 0 || value > CSSValuePool::kMaximumCacheableIntegerValue) {
    return U<CSSNumericLiteralValue>::make_new(value, unit_type);
  }

  // 小数字特殊处理

  int int_value = static_cast<int>(value);
  if (value != int_value || (value == 0 && std::signbit(value))) {
    return U<CSSNumericLiteralValue>::make_new(value, unit_type);
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
      return U<CSSNumericLiteralValue>::make_new(value, unit_type);
  }
}

CSSNumericLiteralValue::CSSNumericLiteralValue(double num, UnitType unit_type)
    : m_num(num),
      CSSPrimitiveValue(unit_type, CSSValueClassType::NumericLiteral) {}

} // namespace html