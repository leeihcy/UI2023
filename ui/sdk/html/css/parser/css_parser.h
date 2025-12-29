#ifndef _UI_SDK_HTML_CSS_PARSER_CSSPARSER_H_
#define _UI_SDK_HTML_CSS_PARSER_CSSPARSER_H_

#include "html/css/parser/css_parser_token_stream.h"
#include "html/css/property/css_value.h"
#include "html/css/property/property_id.h"

#include <string>
#include <vector>


namespace html {

class CSSPropertyName {
public:
  CSSPropertyName(CSSPropertyId id) : m_property_id(id) {}
  CSSPropertyName(const std::string &custom_name)
      : m_custom_property_name(custom_name) {}

public:
  CSSPropertyId m_property_id = CSSPropertyId::Invalid;
  std::string m_custom_property_name;
};

class CSSPropertyValue {
public:
  explicit CSSPropertyValue(const CSSPropertyName& name, const CSSValue* value, bool important=false)
    : m_property_id(name.m_property_id), m_value(value), m_important(important){
      if (m_property_id == CSSPropertyId::Variable) {
        m_custom_property_name = name.m_custom_property_name;
      }
  }
private:
  CSSPropertyId m_property_id;
  std::string m_custom_property_name;
  const CSSValue* m_value;
  bool m_important = false;
};

struct CSSParserContext {
  CSSParserTokenStream token_stream;
  std::vector<CSSPropertyValue> parsed_properties;
};

class CSSParser {
public:
  void ParseInlineStyleDeclaration(const char* bytes, size_t size);

protected:
  void ConsumeBlockContents(CSSParserContext& context);
  bool ConsumeDeclaration(CSSParserContext& context);
  void ConsumeDeclarationValue(CSSParserContext& context, CSSPropertyId property_id);
};

}

#endif