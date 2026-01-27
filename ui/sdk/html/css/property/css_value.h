#ifndef _UI_SDK_HTML_CSS_PROPERTY_CSSVALUE_H_
#define _UI_SDK_HTML_CSS_PROPERTY_CSSVALUE_H_

#include "html/base/atomic_string.h"
#include "html/base/casting.h"
#include "html/base/memory.h"
#include "html/css/property/value_id.h"
#include "html/css/graphics/color.h"
#include "html/css/css_variable_data.h"
#include <string>
#include <vector>

namespace html {

enum class CSSValueClassType {
  Initial,
  Inherited,
  Unset,
  Revert,
  RevertLayer,

  Pair,
  List,

  Identifier,
  Color,
  Image,
  RepeatStyle,

  NumericLiteral,
  
  UnparsedDeclarationClass,
};

// static constexpr size_t kValueListSeparatorBits = 2;
enum class CSSValueListSeparator { 
  kSpaceSeparator, 
  kCommaSeparator, 
  kSlashSeparator 
};

class CSSValue {
public:
  CSSValue(CSSValueClassType class_type) : m_class_type(class_type) { }
  CSSValueClassType GetClassType() const { return m_class_type; }

public:
  bool IsNumericLiteralValue() const {
    return m_class_type == CSSValueClassType::NumericLiteral;
  }
  bool IsIdentifierValue() const {
    return m_class_type == CSSValueClassType::Identifier;
  }
  bool IsPair() const {
    return m_class_type == CSSValueClassType::Pair;
  }
  bool IsUnparsedDeclarationClass() const {
    return m_class_type == CSSValueClassType::UnparsedDeclarationClass;
  }
  AtomicString CssText() {
    return CustomCSSText();
  }
protected:
  // TODO: Blink没有使用虚函数。
  virtual AtomicString CustomCSSText() { return g_empty_atom; }
private:
  CSSValueClassType m_class_type;
};

class CSSInitialValue : public CSSValue {
 public:
  static A<CSSInitialValue> Create();
  CSSInitialValue() : CSSValue(CSSValueClassType::Initial) {}

  AtomicString CustomCSSText() override {
    return AtomicString(u"initial");
  }
};

class CSSInheritedValue : public CSSValue {
 public:
  static A<CSSInheritedValue> Create();
  CSSInheritedValue() : CSSValue(CSSValueClassType::Inherited) {}

  AtomicString CustomCSSText() override {
    return AtomicString(u"inherit");
  }
};

class CSSUnsetValue : public CSSValue {
 public:
  static A<CSSUnsetValue> Create();
  CSSUnsetValue() : CSSValue(CSSValueClassType::Unset) {}

  AtomicString CustomCSSText() override {
    return AtomicString(u"unset");
  }
};

class CSSRevertValue : public CSSValue {
 public:
  static A<CSSRevertValue> Create();
  CSSRevertValue() : CSSValue(CSSValueClassType::Revert) {}

  AtomicString CustomCSSText() override {
    return AtomicString(u"revert");
  }
};

class CSSRevertLayerValue : public CSSValue {
 public:
  static A<CSSRevertLayerValue> Create();
  CSSRevertLayerValue() : CSSValue(CSSValueClassType::RevertLayer) {}

  AtomicString CustomCSSText() override {
    return AtomicString(u"revert-layer");
  }
};

// CSSIdentifierValue stores CSS value keywords
class CSSIdentifierValue : public CSSValue {
 public:
  static A<CSSIdentifierValue> Create(CSSValueID);
  explicit CSSIdentifierValue(CSSValueID);

  CSSValueID GetValueId() const { return m_value_id; }
protected:
  CSSValueID m_value_id;
};
template<>
struct DowncastTraits<CSSIdentifierValue> {
  static bool AllowFrom(const CSSValue& from) {
    return from.IsIdentifierValue();
  }
};

class CSSValuePair : public CSSValue {
public:
  enum class IdenticalValuesPolicy {
    Drop,
    Keep
  };

  explicit CSSValuePair(A<CSSValue> &&first, A<CSSValue> &&second, IdenticalValuesPolicy policy)
      : m_first(std::move(first)), m_second(std::move(second)), m_policy(policy),
        CSSValue(CSSValueClassType::Pair) {}

  const CSSValue* First() const { return m_first.get(); }
  const CSSValue* Second() const { return m_second.get(); }

private:
  A<CSSValue> m_first;
  A<CSSValue> m_second;
  IdenticalValuesPolicy m_policy;
};
template<>
struct DowncastTraits<CSSValuePair> {
  static bool AllowFrom(const CSSValue& from) {
    return from.IsPair();
  }
};

class CSSValueList : public CSSValue {
public:
  CSSValueList(std::vector< A<CSSValue> >&& values, CSSValueListSeparator separator): 
    m_values(std::move(values)), m_separator(separator), CSSValue(CSSValueClassType::List) {}
private:
  std::vector< A<CSSValue> > m_values;
  CSSValueListSeparator m_separator;
};

// Represents the non-keyword subset of <color>.
class CSSColorValue : public CSSValue {
public:
  static A<CSSColorValue> Create(const Color& color);
  CSSColorValue(Color color) : CSSValue(CSSValueClassType::Color), m_color(color) {}

  const Color& GetColor() const { return m_color; };
private:
  Color m_color;
};

class CSSPrimitiveValue : public CSSValue {
public:
  enum class UnitType {
    kUnknown,
    kNumber,
    kPercentage,

    kPixels,
    kInteger,

    kEms,
    kChs,
    kExs,
    kIcs,
    kCaps,

    kRems,
    kRexs,
    kRchs,
    kRics,
    kRlhs,
    kRcaps,

    kLhs,
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

public:
  CSSPrimitiveValue(UnitType unit_type, CSSValueClassType class_type)
      : m_unit_type(unit_type), CSSValue(class_type) {}

  UnitType GetUnitType() const { return m_unit_type; }
protected:
  // blink中将这个字段放到cssvalue类中声明为uint8来节省内存。
  UnitType m_unit_type;
};

class CSSNumericLiteralValue : public CSSPrimitiveValue {
public:
  static A<CSSNumericLiteralValue> Create(double num, UnitType unit_type);
  explicit CSSNumericLiteralValue(double num, UnitType unit_type);

  double GetNum() const { return m_num; }
protected:
  const double m_num;
};
template<>
struct DowncastTraits<CSSNumericLiteralValue> {
  static bool AllowFrom(const CSSValue& from) {
    return from.IsNumericLiteralValue();
  }
};

class CSSUrlData {
public:
  CSSUrlData(const std::u16string& unresolved_url) : m_unresolved_url(unresolved_url) {

  }
private:
  const std::u16string m_unresolved_url;
};

class CSSImageValue : public CSSValue {
public:
  CSSImageValue(A<CSSUrlData>&& url_data): m_url_data(std::move(url_data)), 
    CSSValue(CSSValueClassType::Image) {}

private:
  A<CSSUrlData> m_url_data;
};

class CSSRepeatStyleValue : public CSSValue {
public:
  explicit CSSRepeatStyleValue(A<CSSIdentifierValue>&& repeat);
  CSSRepeatStyleValue(A<CSSIdentifierValue>&& x, A<CSSIdentifierValue>&& y);

private:
  A<CSSIdentifierValue> m_x = nullptr;
  A<CSSIdentifierValue> m_y = nullptr;
};

class CSSUnparsedDeclarationValue final : public CSSValue {
public:
  explicit CSSUnparsedDeclarationValue(A<CSSVariableData> &&data)
      : CSSValue(CSSValueClassType::UnparsedDeclarationClass),
        m_data(std::move(data)) {}

private:
  A<CSSVariableData> m_data;
};
template<>
struct DowncastTraits<CSSUnparsedDeclarationValue> {
  static bool AllowFrom(const CSSValue& from) {
    return from.IsUnparsedDeclarationClass();
  }
};

}

#endif