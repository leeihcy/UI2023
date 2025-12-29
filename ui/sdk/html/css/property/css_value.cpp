#include "html/css/property/css_value.h"
#include <map>

namespace html {

class CSSValuePool {
public:
  static CSSValuePool& Get() {
    static CSSValuePool s;
    return s;
  }

  CSSIdentifierValue* IdentifierCacheValue(CSSValueId value_id) {
    auto iter = m_identifier_value_map.find(value_id);
    if (iter != m_identifier_value_map.end()) {
      return iter->second;
    }
    CSSIdentifierValue* v = new CSSIdentifierValue(value_id);
    m_identifier_value_map[value_id] = v;
    return v;
  }
private:
  std::map<CSSValueId, CSSIdentifierValue*> m_identifier_value_map;
};


CSSInitialValue* CSSInitialValue::Create() {
  static CSSInitialValue s;
  return &s;
}
CSSInheritedValue* CSSInheritedValue::Create() {
  static CSSInheritedValue s;
  return &s;
}
CSSUnsetValue* CSSUnsetValue::Create() {
  static CSSUnsetValue s;
  return &s;
}
CSSRevertValue* CSSRevertValue::Create() {
  static CSSRevertValue s;
  return &s;
}
CSSRevertLayerValue* CSSRevertLayerValue::Create() {
  static CSSRevertLayerValue s;
  return &s;
}

CSSIdentifierValue* CSSIdentifierValue::Create(CSSValueId value_id) {
  return CSSValuePool::Get().IdentifierCacheValue(value_id);
}
CSSIdentifierValue::CSSIdentifierValue(CSSValueId id) 
  : CSSValue(CSSValueClassType::Identifier)  {

}
}