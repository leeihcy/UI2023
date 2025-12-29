#ifndef _UI_SDK_HTML_CSS_PROPERTY_CSSVALUE_H_
#define _UI_SDK_HTML_CSS_PROPERTY_CSSVALUE_H_
#include "html/css/property/value_id.h"

namespace html {

enum CSSValueClassType {
  Initial,
  Inherited,
  Unset,
  Revert,
  RevertLayer,

  Identifier,
};

class CSSValue {
public:
  CSSValue(CSSValueClassType class_type) : m_class_type(class_type) {

  }

private:
  CSSValueClassType m_class_type;
};

class CSSInitialValue : public CSSValue {
 public:
  static CSSInitialValue* Create();
  CSSInitialValue() : CSSValue(CSSValueClassType::Initial) {}
};

class CSSInheritedValue : public CSSValue {
 public:
  static CSSInheritedValue* Create();
  CSSInheritedValue() : CSSValue(CSSValueClassType::Inherited) {}
};

class CSSUnsetValue : public CSSValue {
 public:
  static CSSUnsetValue* Create();
  CSSUnsetValue() : CSSValue(CSSValueClassType::Unset) {}
};

class CSSRevertValue : public CSSValue {
 public:
  static CSSRevertValue* Create();
  CSSRevertValue() : CSSValue(CSSValueClassType::Revert) {}
};

class CSSRevertLayerValue : public CSSValue {
 public:
  static CSSRevertLayerValue* Create();
  CSSRevertLayerValue() : CSSValue(CSSValueClassType::RevertLayer) {}
};

// CSSIdentifierValue stores CSS value keywords
class CSSIdentifierValue : public CSSValue {
 public:
  static CSSIdentifierValue* Create(CSSValueId);
  explicit CSSIdentifierValue(CSSValueId);
};

}

#endif