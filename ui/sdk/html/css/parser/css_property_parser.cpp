#include "html/css/parser/css_property_parser.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_parser.h"
#include "html/css/property/value_id.h"
#include "html/css/property/css_parsing_utils.h"
#include "html/css/property/css_property.h"
#include <cassert>

namespace html {

// static
bool CSSPropertyParser::ParseValue(CSSParserContext &context,
                                   CSSPropertyId property_id,
                                   bool allow_important) {
  CSSPropertyParser parser(context);
  return parser.ParseValueStart(property_id, allow_important);
}

CSSPropertyParser::CSSPropertyParser(CSSParserContext &context) : m_context(context)
{
}

U<CSSValue> CSSPropertyParser::ConsumeCSSWideKeyword(
    bool allow_important_annotation,
    bool& important) {

  CSSParserTokenStream::State savepoint = m_context.token_stream.Save();

  U<CSSValue> value = css_parsing_utils::ConsumeCSSWideKeyword(m_context.token_stream);
  if (!value) {
    return nullptr;
  }

  important = css_parsing_utils::MaybeConsumeImportant(
      m_context.token_stream, allow_important_annotation);
  if (!m_context.token_stream.AtEnd()) {
    m_context.token_stream.Restore(savepoint);
    return nullptr;
  }

  return value;
}

bool CSSPropertyParser::ParseCSSWideKeyword(CSSPropertyId property_id, bool allow_important_annotation) {
  bool important;
  U<CSSValue> value =
      ConsumeCSSWideKeyword(allow_important_annotation, important);
  if (!value) {
    return false;
  }

  // const StylePropertyShorthand& shorthand = shorthandForProperty(property_id);
  // if (!shorthand.length()) {
  //   if (!CSSProperty::Get(property).IsProperty()) {
  //     return false;
  //   }
    // AddProperty(property, CSSPropertyID::kInvalid, *value, important,
    //             IsImplicitProperty::kNotImplicit, *parsed_properties_);
  // } else {
  //   css_parsing_utils::AddExpandedPropertyForValue(property, *value, important,
  //                                                  *parsed_properties_);
  // }

  m_context.parsed_properties.push_back(CSSPropertyValue(
    CSSPropertyName(property_id), std::move(value), important
  ));
  return true;
}

bool CSSPropertyParser::ParseValueStart(CSSPropertyId property_id, bool allow_important) {
  if (ParseCSSWideKeyword(property_id, allow_important)) {
    return true;
  }

  // 将property_id转换成对应的属性类，让属性类去做进一步的属性解析。
  CSSParserTokenStream::State savepoint = m_context.token_stream.Save();

  const CSSProperty& property = CSSProperty::Get(property_id);
  if (!property.IsProperty()) {
    return false;
  }

  // if (!IsPropertyAllowedInRule(property, rule_type)) {
  //   return false;
  // }
  bool is_shorthand = property.IsShorthand();
  if (is_shorthand) {
    if (static_cast<const Shorthand&>(property).ParseShorthand(m_context)) {
      bool important = css_parsing_utils::MaybeConsumeImportant(
          m_context.token_stream, allow_important);
      if (m_context.token_stream.AtEnd()) {
        if (important) {
          // for (wtf_size_t property_idx = parsed_properties_size;
          //      property_idx < parsed_properties_->size(); ++property_idx) {
          //   (*parsed_properties_)[property_idx].SetImportant();
          // }
        }
        return true;
      }
    }
  } else {
    if (U<CSSValue> parsed_value =
            css_parsing_utils::ParseLonghand(property_id, m_context)) {
      bool important = css_parsing_utils::MaybeConsumeImportant(
          m_context.token_stream, allow_important);
      
      // 当前上下文应该处于 ; Boundary之中。    
      if (m_context.token_stream.AtEnd()) {
        //
        //   AddProperty(property_id, CSSPropertyID::kInvalid, *parsed_value,
        //               important, IsImplicitProperty::kNotImplicit,
        //               *parsed_properties_);
        m_context.parsed_properties.push_back(CSSPropertyValue(
            CSSPropertyName(property_id), std::move(parsed_value), important));
        return true;
      }
    }
  }

  assert(false); // TODO
  return false;
}

}