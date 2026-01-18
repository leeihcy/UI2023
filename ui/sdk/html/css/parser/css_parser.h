#ifndef _UI_SDK_HTML_CSS_PARSER_CSSPARSER_H_
#define _UI_SDK_HTML_CSS_PARSER_CSSPARSER_H_

#include "html/css/parser/allowed_rules.h"
#include "html/css/parser/css_parser_token_stream.h"
#include "html/css/parser/css_nesting_type.h"
#include "html/css/property/css_value.h"
#include "html/css/property/property_id.h"
#include "html/css/style_sheet_contents.h"
#include "html/css/style_rule.h"

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
  explicit CSSPropertyValue(const CSSPropertyName& name, A<CSSValue>&& value, bool important=false)
    : m_property_id(name.m_property_id), m_value(std::move(value)), m_important(important){
      if (m_property_id == CSSPropertyId::Variable) {
        m_custom_property_name = name.m_custom_property_name;
      }
  }
  CSSPropertyId Id() { return m_property_id; }
  const std::string& CustomPropertyName() { return m_custom_property_name;}
  const CSSValue* Value() { return m_value.get(); }
  bool IsImportant() { return m_important; }
  void SetImportant(bool important) { m_important = important; }
private:
  CSSPropertyId m_property_id;
  std::string m_custom_property_name;
  A<CSSValue> m_value;
  bool m_important = false;
};

struct CSSParserContext {
  // 输入内容
  CSSParserTokenStream token_stream;

  // css内容解析结果
  // StyleSheetContents style_sheet;

  // style=""内容解析结果
  std::vector<CSSPropertyValue> parsed_properties;
};

class CSSPropertyValueSet {
public:
  CSSPropertyValueSet(std::vector<CSSPropertyValue> &&array)
      : m_property_vector(std::move(array)) {}

  size_t Size() { return m_property_vector.size(); }
  CSSPropertyValue &Item(unsigned int i) {
    if (i >= m_property_vector.size()) {
      abort();
    }
    return m_property_vector[i];
  };

private:
  std::vector<CSSPropertyValue> m_property_vector;
};


class CSSParser {
public:
  std::unique_ptr<CSSPropertyValueSet>
  ParseInlineStyleDeclaration(const char *bytes, size_t size);

  bool ParseStyleSheet(const char *bytes, size_t size);

protected:
  void ConsumeBlockContents(CSSParserContext& context);
  bool ConsumeDeclaration(CSSParserContext& context);
  void ConsumeDeclarationValue(CSSParserContext& context, CSSPropertyId property_id);
  
  bool ConsumeRuleList(CSSParserContext &context, AllowedRules allowed_rules);
  A<StyleRule> ConsumeQualifiedRule(CSSParserContext &context,
                                    AllowedRules allowed_rules);
  A<StyleRule> ConsumeStyleRule(CSSParserContext &context);
};

}

#endif