#include "html/css/parser/css_variable_parser.h"
#include "html/css/parser/css_parser_token.h"
#include "html/css/parser/css_parser_token_stream.h"
#include "html/css/css_variable_data.h"
#include "html/css/property/css_parsing_utils.h"
#include <string>

namespace html {


bool CSSVariableParser::IsValidVariableName(const CSSParserToken& token) {
  if (token.GetType() != CSSParserTokenType::Ident) {
    return false;
  }
  return IsValidVariableName(token.Name());
}

bool CSSVariableParser::IsValidVariableName(const std::u16string& string) {
  return string.length() >= 3 && string[0] == '-' && string[1] == '-';
}

// --x:foo;
bool CSSVariableParser::StartsCustomPropertyDeclaration(CSSParserTokenStream& stream) {
   if (!CSSVariableParser::IsValidVariableName(stream.Peek())) {
    return false;
  }
  CSSParserTokenStream::State state = stream.Save();
  stream.ConsumeIncludingWhitespace();  // <ident>
  bool result = stream.Peek().GetType() == CSSParserTokenType::Colon;
  stream.Restore(state);
  return result;
}

static bool IsCustomFunction(const CSSParserToken& token) {
  return /*RuntimeEnabledFeatures::CSSFunctionsEnabled() &&*/
         css_parsing_utils::IsDashedFunctionName(token);
}
#if 0
// Keep in sync with ConsumeMixinArguments() below.
static bool ConsumeCustomFunction(CSSParserTokenStream& stream,
                                  bool& has_references,
                                  bool& has_font_units,
                                  bool& has_root_font_units,
                                  bool& has_line_height_units,
                                  bool& has_dashed_functions,
                                  const CSSParserContext& context) {
  CSSParserTokenStream::BlockGuard guard(stream);
  stream.ConsumeWhitespace();

  // Consume the arguments.
  while (!stream.AtEnd()) {
    // Commas and "{}" blocks are normally not allowed in argument values
    // (at the top level), unless the whole value is wrapped in a "{}" block.
    //
    // https://drafts.csswg.org/css-values-5/#component-function-commas
    if (stream.Peek().GetType() == kLeftBraceToken) {
      CSSParserTokenStream::BlockGuard brace_guard(stream);
      stream.ConsumeWhitespace();
      if (stream.AtEnd()) {
        // Empty values are not allowed. (The "{}" wrapper is not part
        // of the value.)
        return false;
      }
      if (!ConsumeUnparsedValue(stream, /*restricted_value=*/false,
                                /*comma_ends_declaration=*/false,
                                has_references, has_font_units,
                                has_root_font_units, has_line_height_units,
                                has_dashed_functions/*, context*/)) {
        return false;
      }
    } else {
      // Arguments that look like custom property declarations are reserved
      // for named arguments.
      //
      // https://github.com/w3c/csswg-drafts/issues/11749
      if (CSSVariableParser::StartsCustomPropertyDeclaration(stream)) {
        return false;
      }
      // Passing restricted_value=true effectively disallows "{}".
      if (!ConsumeUnparsedValue(stream, /*restricted_value=*/true,
                                /*comma_ends_declaration=*/true, has_references,
                                has_font_units, has_root_font_units,
                                has_line_height_units, has_dashed_functions,
                                context)) {
        return false;
      }
    }
    if (stream.Peek().GetType() == kCommaToken) {
      stream.ConsumeIncludingWhitespace();  // kCommaToken
      if (stream.AtEnd() || stream.Peek().GetType() == kCommaToken) {
        // Empty values are not allowed. (ConsumeUnparsedValue returns true
        // in that case.)
        return false;
      }
      continue;
    } else if (stream.AtEnd()) {
      // No further arguments.
      break;
    }
    // Unexpected token, e.g. '!'.
    return false;
  }
  return true;
}
#endif
static bool ConsumeUnparsedValue(CSSParserTokenStream& stream,
                                 bool restricted_value,
                                 bool comma_ends_declaration,
                                 bool& has_references,
                                 bool& has_font_units,
                                 bool& has_root_font_units,
                                 bool& has_line_height_units,
                                 bool& has_dashed_functions/*,
                                 const CSSParserContext& context*/) {
  size_t block_stack_size = 0;

  // https://drafts.csswg.org/css-syntax/#component-value
  size_t top_level_component_values = 0;
  bool has_top_level_brace = false;
  bool error = false;

  while (true) {
    const CSSParserToken& token = stream.Peek();
    if (token.IsEOF()) {
      break;
    }

    CSSVariableData::ExtractFeatures(token, has_font_units, has_root_font_units,
                                     has_line_height_units,
                                     has_dashed_functions);

    // Save this, since we'll change it below.
    const bool at_top_level = block_stack_size == 0;

    // First check if this is a valid substitution function (e.g. var(),
    // then handle the next token accordingly.
    if (token.GetBlockType() == CSSParserTokenBlockType::Start) {
      // A block may have both var and env references. They can also be nested
      // and used as fallbacks.
      switch (token.FunctionId()) {
#if 0
        case CSSValueID::Invalid:
          // Not a built-in function, but it might be an author-defined
          // CSS function (e.g. --foo()).
          if (!IsCustomFunction(token)) {
            break;
          }
          if (!ConsumeCustomFunction(stream, has_references, has_font_units,
                                     has_root_font_units, has_line_height_units,
                                     has_dashed_functions/*, context*/)) {
            error = true;
          }
          has_references = true;
          continue;
        case CSSValueID::Inherit:
          if (!RuntimeEnabledFeatures::CSSInheritFunctionEnabled()) {
            return false;
          }
          [[fallthrough]];
        case CSSValueID::Var:
          if (!ConsumeVariableReference(
                  stream, has_references, has_font_units, has_root_font_units,
                  has_line_height_units, has_dashed_functions/*, context*/)) {
            error = true;
          }
          has_references = true;
          continue;
        case CSSValueID::Env:
          if (!ConsumeEnvVariableReference(
                  stream, has_references, has_font_units, has_root_font_units,
                  has_line_height_units, has_dashed_functions, context)) {
            error = true;
          }
          has_references = true;
          continue;
        case CSSValueID::Attr:
          if (!ConsumeAttributeReference(
                  stream, has_references, has_font_units, has_root_font_units,
                  has_line_height_units, has_dashed_functions, context)) {
            error = true;
          }
          has_references = true;
          continue;
        case CSSValueID::kInternalAutoBase:
          if (context.GetMode() != kUASheetMode) {
            break;
          }
          if (!ConsumeInternalAutoBase(
                  stream, has_references, has_font_units, has_root_font_units,
                  has_line_height_units, has_dashed_functions, context)) {
            error = true;
          }
          has_references = true;
          continue;
        case CSSValueID::kIf:
          if (!RuntimeEnabledFeatures::CSSInlineIfForStyleQueriesEnabled()) {
            break;
          }
          if (!ConsumeIf(stream, has_references, has_font_units,
                         has_root_font_units, has_line_height_units,
                         has_dashed_functions, context)) {
            error = true;
          }
          if (!error) {
            context.Count(WebDXFeature::kIf);
          }
          has_references = true;
          continue;
#endif
        default:
          break;
      }
    }

    if (token.GetBlockType() == CSSParserTokenBlockType::Start) {
      ++block_stack_size;
    } else if (token.GetBlockType() == CSSParserTokenBlockType::End) {
      if (block_stack_size == 0) {
        break;
      }
      --block_stack_size;
    } else {
      switch (token.GetType()) {
        case CSSParserTokenType::Delimiter: {
          if (token.Delimiter() == '!' && block_stack_size == 0) {
            return !error;
          }
          break;
        }
        case CSSParserTokenType::RightParenthesis:
        case CSSParserTokenType::RightBrace:
        case CSSParserTokenType::RightBracket:
        case CSSParserTokenType::BadString:
        case CSSParserTokenType::BadUrl:
          error = true;
          break;
        case CSSParserTokenType::Semicolon:
          if (block_stack_size == 0) {
            return !error;
          }
          break;
        case CSSParserTokenType::Comma:
          if (comma_ends_declaration && block_stack_size == 0) {
            return !error;
          }
          break;
        default:
          break;
      }
    }

    if (error && at_top_level) {
      // We cannot safely exit until we are at the top level; this is a waste,
      // but it's not a big problem since we need to fast-forward through error
      // recovery in nearly all cases anyway (the only exception would be when
      // we retry as a nested rule, but nested rules that look like custom
      // property declarations are illegal and cannot happen in legal CSS).
      return false;
    }

    // Now that we know this token wasn't an end-of-value marker,
    // check whether we are violating the rules for restricted values.
    if (restricted_value && at_top_level) {
      ++top_level_component_values;
      if (token.GetType() == CSSParserTokenType::LeftBrace) {
        has_top_level_brace = true;
      }
      if (has_top_level_brace && top_level_component_values > 1) {
        return false;
      }
    }

    stream.Consume();
    //stream.ConsumeRaw();
  }

  return !error;
}

A<CSSVariableData> CSSVariableParser::ConsumeUnparsedDeclaration(
    CSSParserTokenStream& stream,
    bool allow_important_annotation,
    bool is_animation_tainted,
    bool must_contain_variable_reference,
    bool restricted_value,
    bool comma_ends_declaration,
    bool& important/*,
    const CSSParserContext& context*/) {
  // Consume leading whitespace and comments, as required by the spec.
  stream.ConsumeWhitespace();
  size_t value_start_offset = stream.PreviousOffset();

  bool has_references = false;
  bool has_font_units = false;
  bool has_root_font_units = false;
  bool has_line_height_units = false;
  bool has_dashed_functions = false;
  if (!ConsumeUnparsedValue(stream, restricted_value, comma_ends_declaration,
                            has_references, has_font_units, has_root_font_units,
                            has_line_height_units, has_dashed_functions/*,
                            context*/)) {
    return nullptr;
  }

  if (must_contain_variable_reference && !has_references) {
    return nullptr;
  }

  size_t value_end_offset = stream.PreviousOffset();

  important = css_parsing_utils::MaybeConsumeImportant(
      stream, allow_important_annotation);
  if (!stream.AtEnd() &&
      !(comma_ends_declaration && stream.Peek().GetType() == CSSParserTokenType::Comma)) {
    return nullptr;
  }

  std::u16string original_text = stream.StringRangeAt(
      value_start_offset, value_end_offset - value_start_offset);

  if (original_text.length() > CSSVariableData::kMaxVariableBytes) {
    return nullptr;
  }

#if 0
  original_text =
      CSSVariableParser::StripTrailingWhitespaceAndComments(original_text);
#endif
  return CSSVariableData::Create(std::move(original_text), is_animation_tainted, false,
                                 /*needs_variable_resolution=*/has_references,
                                 has_font_units, has_root_font_units,
                                 has_line_height_units, has_dashed_functions);
}

}
