#include "html/css/css_variable_data.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/property/css_parsing_utils.h"

namespace html {

static bool IsFontUnitToken(CSSParserToken token) {
  if (token.GetType() != CSSParserTokenType::Dimension) {
    return false;
  }
  switch (token.GetUnitType()) {
  case CSSPrimitiveValue::UnitType::kEms:
  case CSSPrimitiveValue::UnitType::kChs:
  case CSSPrimitiveValue::UnitType::kExs:
  case CSSPrimitiveValue::UnitType::kIcs:
  case CSSPrimitiveValue::UnitType::kCaps:
    return true;
  default:
    return false;
  }
}

static bool IsRootFontUnitToken(CSSParserToken token) {
  if (token.GetType() != CSSParserTokenType::Dimension) {
    return false;
  }
  switch (token.GetUnitType()) {
  case CSSPrimitiveValue::UnitType::kRems:
  case CSSPrimitiveValue::UnitType::kRexs:
  case CSSPrimitiveValue::UnitType::kRchs:
  case CSSPrimitiveValue::UnitType::kRics:
  case CSSPrimitiveValue::UnitType::kRlhs:
  case CSSPrimitiveValue::UnitType::kRcaps:
    return true;
  default:
    return false;
  }
}

static bool IsLineHeightUnitToken(CSSParserToken token) {
  return token.GetType() == CSSParserTokenType::Dimension &&
         token.GetUnitType() == CSSPrimitiveValue::UnitType::kLhs;
}

void CSSVariableData::ExtractFeatures(const CSSParserToken& token,
                                      bool& has_font_units,
                                      bool& has_root_font_units,
                                      bool& has_line_height_units,
                                      bool& has_dashed_functions) {
  has_font_units |= IsFontUnitToken(token);
  has_root_font_units |= IsRootFontUnitToken(token);
  has_line_height_units |= IsLineHeightUnitToken(token);
  has_dashed_functions |= css_parsing_utils::IsDashedFunctionName(token);
}


CSSVariableData::CSSVariableData(const std::u16string&& original_text,
                                 bool is_animation_tainted,
                                 bool is_attr_tainted,
                                 bool needs_variable_resolution,
                                 bool has_font_units,
                                 bool has_root_font_units,
                                 bool has_line_height_units,
                                 bool has_dashed_functions)
    : length_(original_text.length()),
      is_animation_tainted_(is_animation_tainted),
      is_attr_tainted_(is_attr_tainted),
      needs_variable_resolution_(needs_variable_resolution),
      is_8bit_(false),
      has_font_units_(has_font_units),
      has_root_font_units_(has_root_font_units),
      has_line_height_units_(has_line_height_units),
      has_dashed_functions_(has_dashed_functions),
      m_original_text(original_text) {
  
}

}
