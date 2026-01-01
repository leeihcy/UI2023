#ifndef _UI_SDK_HTML_CSS_PROPERTY_CSSVALUE_H_
#define _UI_SDK_HTML_CSS_PROPERTY_CSSVALUE_H_

#include "html/base/memory.h"
#include "html/css/property/value_id.h"
#include "html/css/graphics/color.h"

#include <string>

namespace html {

enum class CSSValueClassType {
  Initial,
  Inherited,
  Unset,
  Revert,
  RevertLayer,

  Identifier,
  Color,

  NumericLiteral,
};

class CSSValue {
public:
  CSSValue(CSSValueClassType class_type) : m_class_type(class_type) { }
  CSSValueClassType GetClassType() const { return m_class_type; }
private:
  CSSValueClassType m_class_type;
};

class CSSInitialValue : public CSSValue {
 public:
  static U<CSSInitialValue> Create();
  CSSInitialValue() : CSSValue(CSSValueClassType::Initial) {}
};

class CSSInheritedValue : public CSSValue {
 public:
  static U<CSSInheritedValue> Create();
  CSSInheritedValue() : CSSValue(CSSValueClassType::Inherited) {}
};

class CSSUnsetValue : public CSSValue {
 public:
  static U<CSSUnsetValue> Create();
  CSSUnsetValue() : CSSValue(CSSValueClassType::Unset) {}
};

class CSSRevertValue : public CSSValue {
 public:
  static U<CSSRevertValue> Create();
  CSSRevertValue() : CSSValue(CSSValueClassType::Revert) {}
};

class CSSRevertLayerValue : public CSSValue {
 public:
  static U<CSSRevertLayerValue> Create();
  CSSRevertLayerValue() : CSSValue(CSSValueClassType::RevertLayer) {}
};

// CSSIdentifierValue stores CSS value keywords
class CSSIdentifierValue : public CSSValue {
 public:
  static U<CSSIdentifierValue> Create(CSSValueId);
  explicit CSSIdentifierValue(CSSValueId);
};

// Represents the non-keyword subset of <color>.
class CSSColorValue : public CSSValue {
public:
  static U<CSSColorValue> Create(const Color& color);
  CSSColorValue(Color color) : CSSValue(CSSValueClassType::Color), m_color(color) {}

  const Color& GetColor() const { return m_color; };
private:
  Color m_color;
};

class CSSPrimitiveValue : public CSSValue {
public:
  CSSPrimitiveValue(CSSValueClassType class_type) : CSSValue(class_type) {}
  enum class UnitType {
    kUnknown,
    kNumber,
    kPercentage,

    kPixels,
  };
  enum LengthUnitType {
    kUnitTypePixels = 0,
    kUnitTypePercentage,
  };
  enum class ValueRange {
    kAll,
    kNonNegative,
    kInteger,
    kNonNegativeInteger,
    kPositiveInteger
  };

  static UnitType StringToUnitType(const std::u16string& text);
};

class CSSNumericLiteralValue : public CSSPrimitiveValue {
public:
  static U<CSSNumericLiteralValue> Create(double num, UnitType unit_type);
  explicit CSSNumericLiteralValue(double num, UnitType unit_type);

};

}

#endif